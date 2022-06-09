#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "PieceTable.h"

//#define DEBUG
#define MSG_LEN 250
#define LOG_TARGET stdout


Logger *InitLogger() {
    Logger *l = malloc(sizeof(Logger));
    l->top = NULL;
    return l;
};

void Log(Logger *L, int level, char *msg, ...) {
    va_list args;
    va_start(args, msg);
    struct Message *M = malloc(sizeof(struct Message));
    M->msg = malloc(sizeof(char)*MSG_LEN);
    vsprintf(M->msg, msg, args);
    va_end(args);
    M->level = level;
    MessageWrapper *w = malloc(sizeof(MessageWrapper));
    w->m = M;
    w->next = NULL;
    MessageWrapper *p = L->top;
    if (p == NULL) L->top = w;
    else {
        while (p->next != NULL)
            p = p->next;
        p->next = w;
    }
}

void PrintLog(Logger *L) {
    MessageWrapper *p = L->top;
    if (p == NULL) return;
    for (struct Message *M; p != NULL; p = p->next) {
        M = p->m;
        switch (M->level) {
            case 1:
                printf("DEBUG: ");
                break;
            case 2:
                printf("WARNING: ");
                break;
            case 3:
                printf("ERROR: ");
                break;
            default:
                printf("Code %d: ", M->level);
                break;
        }
        fprintf(LOG_TARGET, M->msg);
        fprintf(LOG_TARGET, "\n");
    }
}

void ClearLog(Logger *L) {
    MessageWrapper *p = L->top;
    if (p == NULL) return;
    for (MessageWrapper *w = p; p != NULL; w = p) {
        p = p->next;
        free(w->m);
        w->m = NULL;
        free(w);
        w = NULL;
    }
    L->top = NULL;
}


extern Logger *L;


struct Queue *MakeQueue(int capacity) {
    struct Queue *q = malloc(sizeof(struct Queue));
    q->tree = malloc(sizeof(struct Tree)*capacity);
    q->back = q->size = 0;
    q->front = -1;
    q->capacity = capacity;
    return q;
}

void enqueue(struct Queue *q, struct Tree *node) {
    if (q->size == q->capacity) {
        Log(L, 2, "queue is full");
    } else {
        if (q->front == q->capacity)
            q->front = 0;
        else
            q->front++;
        q->tree[q->front] = node;
        q->size++;
    }
}

struct Tree *dequeue(struct Queue *q) {
    if (q->size == 0) {
        Log(L, 2, "queue is empty");
    } else {
        struct Tree *ret = q->tree[q->back];
        if (q->back == q->capacity)
            q->back = 0;
        else
            q->back++;
        q->size--;
        return ret;
    }
}

void PrintTree(struct Tree *t) {
    struct Queue *q = MakeQueue(1000);
    enqueue(q, t);
    struct Tree *tmp;
    struct Tree *last = NULL;
    while (q->size > 0) {
        tmp = dequeue(q);
        if (tmp == last) printf("\n");
        printf("(%s, %ld, %ld, %ld) ", tmp->piece->start, tmp->piece->length, tmp->size_left, tmp->size_right);
        if (tmp->left != NULL) enqueue(q, tmp->left); last = tmp->left;
        if (tmp->right != NULL) enqueue(q, tmp->right); last = tmp->right;
    }
}

struct Piece *MakePiece(char *start, unsigned long int length) {
    struct Piece *NewPiece = malloc(sizeof(NewPiece));
    NewPiece->start = malloc(sizeof(char)*(length + 1));
    strncpy(NewPiece->start, start, length);
    NewPiece->start[length] = '\0';
    NewPiece->length = length;
    return NewPiece;
}


struct Tree *MakeNode(struct Piece *piece) {
    /* takes piece, returns node that points to piece */
    struct Tree *NewNode = malloc(sizeof(struct Tree));
    NewNode->piece = piece;
    NewNode->left = NewNode->right = NewNode->parent = NULL;
    NewNode->size_right = NewNode->size_right = 0;
    return NewNode;
}


int is_split(unsigned long int offset, unsigned long int index, unsigned long int parent_length) {
    /* takes offset and index of node to be inserted, returns 1
     * if a split is needed, 0 otherwise. */
    unsigned long int relative_left_bound = offset - parent_length;
    unsigned long int relative_right_bound = parent_length;
    unsigned long int relative_index = index - relative_left_bound;
    /*if ((offset - parent_length) < (index - (offset-parent_length)) && (index - (offset-parent_length)) < (offset + parent_length))*/
    if (relative_left_bound < relative_index && relative_index < relative_right_bound)
        return 1;
    else
        return 0;
}

unsigned long int node_size(struct Tree *node) {
    if (node == NULL)
        return 0; /* we don't consider strings of length 0 to be meaningful */
    else
        return node->size_left + node->size_right + node->piece->length;
}

unsigned long int node_offset(struct Tree *node) {
    if (node == NULL)
        return 0;
    else
        return node->size_left;
        /*return node->size_left + node->piece->length;*/
}


struct Tree *BSTInsert(struct Tree *tree, struct Tree **inserted, struct Piece *piece, unsigned long int index) {

    unsigned long int offset = 0;
    unsigned long int root_offset = 0;
    struct Tree *nodeptr = tree;
    if (nodeptr == NULL) {
        tree = MakeNode(piece);
        *inserted = tree;
        return tree;
    } else { 
        offset += node_offset(tree);
        while (1) {
            if (index == offset) {
                struct Tree *tmp = nodeptr->left;
                nodeptr->left = MakeNode(piece);
                nodeptr->left->parent = nodeptr;
                *inserted = nodeptr->left;
                nodeptr->left->left = tmp;
                nodeptr->left->size_left = node_size(tmp);
                nodeptr->size_left = node_size(nodeptr->left);
                if (tmp != NULL) tmp->parent = nodeptr;
                return tree;
            }
            else if (index < offset) { 
                if (nodeptr->left != NULL) {
                    nodeptr = nodeptr->left;
                    offset = node_offset(nodeptr) + root_offset;
                }
                else {
                    if (is_split(offset, index, nodeptr->piece->length)) {
                        unsigned long int total_length = nodeptr->piece->length;
                        unsigned long int last_half_length = offset - index;
                        nodeptr->piece->length = last_half_length;
                        nodeptr->left = MakeNode(piece);
                        nodeptr->left->parent = nodeptr;
                        struct Tree *first_half = MakeNode(MakePiece( nodeptr->piece->start, total_length - last_half_length));
                        nodeptr->piece->start += first_half->piece->length + 1;
                        nodeptr->left->left = first_half;
                        first_half->parent = nodeptr->left;
                        nodeptr->left->size_left = node_size(nodeptr->left->left);
                        nodeptr->size_left = node_size(nodeptr->left);
                        *inserted = nodeptr->left;
                        return tree;
                    } else {
                        nodeptr->left = MakeNode(piece);
                        nodeptr->left->parent = nodeptr;
                        nodeptr->size_left = node_size(nodeptr->left);
                        *inserted = nodeptr->left;
                        return tree;
                    }
                }
            }
            else if (index > offset) {
                if (nodeptr->right != NULL) {
                    offset += nodeptr->piece->length;
                    if (nodeptr == tree) root_offset = offset;
                    nodeptr = nodeptr->right;
                    offset += node_offset(nodeptr);
                }
                else {
                    if (is_split(offset, index, nodeptr->piece->length)) {
                        unsigned long int total_length = nodeptr->piece->length;
                        unsigned long int last_half_length = total_length - offset + index;
                        nodeptr->piece->length -= last_half_length;
                        nodeptr->right = MakeNode(piece);
                        nodeptr->right->parent = nodeptr;
                        struct Tree *last_half = MakeNode(MakePiece( nodeptr->piece->start + nodeptr->piece->length + 1, last_half_length));
                        nodeptr->right->right= last_half;
                        last_half->parent = nodeptr->right;
                        nodeptr->right->size_left = node_size(nodeptr->right->right);
                        nodeptr->size_right = node_size(nodeptr->right);
                        *inserted = nodeptr->right;
                        return tree;
                    } else {
                        nodeptr->right = MakeNode(piece);
                        nodeptr->right->parent = nodeptr;
                        nodeptr->size_right = node_size(nodeptr->right);
                        *inserted = nodeptr->right;
                        return tree;
                    }
                }
            }
        }
    }
}


enum side ZigSide(struct Tree *tree) {
    if (tree->parent->right == tree)
        return right;
    else if (tree->parent->left == tree)
        return left;
    else
        return serror;
}

enum casetype ZigZigOrZigZag(struct Tree *tree) {
    if (tree->parent->left == tree && tree->parent->parent->left == tree->parent)
        return zigzigleft;
    if (tree->parent->right == tree && tree->parent->parent->right == tree->parent)
        return zigzigright;
    if (tree->parent->left == tree && tree->parent->parent->right == tree->parent)
        return zigzagleft;
    if (tree->parent->right == tree && tree->parent->parent->left == tree->parent)
        return zigzagright;
    else {
#ifdef DEBUG
        Log(L, 3, "couldn't determine case of zig-zig left/right or zig-zag left/right");
#endif
        return cerror;
    }
}


struct Tree *Splay(struct Tree *tree, struct Tree *node) {
    /* three cases: zig, zig-zig, zig-zag. We check
     * for these cases starting at the last inserted
     * node, and move up the tree. */
    while (node->parent != NULL) { 
        if (node->parent->parent == NULL) {
            struct Tree *parent = node;
            struct Tree *child = node->parent;
            struct Message *m;
            switch (ZigSide(node)) { 
                case left:
                    child->left = parent->right;
                    child->size_left = node_size(child->left);
                    if (child->left != NULL)
                        child->left->parent = child;
                    parent->parent = child->parent;
                    child->parent = parent;
                    parent->right = child;
                    parent->size_right = node_size(parent->right);
                    break;
                case right:
                    child->right = parent->left;
                    if (child->right != NULL)
                        child->right->parent = child;
                    child->size_right = node_size(child->right);
                    parent->parent = child->parent;
                    child->parent = parent;
                    parent->left = child;
                    parent->size_left = node_size(parent->left);
                    break;
                case serror:
                default:
                    Log(L, 3, "could not determine zig side");
                    return tree;
            }
            tree = parent;
        }
        else if (node->parent->parent != NULL) {
            struct Tree *parent = node;
            struct Tree *child = node->parent;
            struct Tree *grandchild = node->parent->parent;
            struct Tree *link = grandchild->parent;
            struct Message *m;
            enum side link_side = serror;
            if (link == NULL) {
                link_side = null;
            }
            else {
                if (link->left != NULL) {
                    if (link->left == grandchild) link_side = left;
                }
                if (link->right != NULL) {
                    if (link->right == grandchild) link_side = right;
                }
            }
            if (link_side == serror)
                Log(L, 3, "couldn't determine link side");
            switch (ZigZigOrZigZag(node)) {
                case zigzigleft:
                    grandchild->left = child->right;
                    grandchild->size_left = node_size(grandchild->left);
                    if (grandchild->left != NULL)
                        grandchild->left->parent = grandchild;
                    grandchild->parent = child;
                    child->left = parent->right;
                    child->size_left = node_size(child->left);
                    if (child->left != NULL)
                        child->left->parent = child;
                    child->right = grandchild;
                    child->size_right = node_size(child->right);
                    parent->right = child;
                    parent->size_right = node_size(parent->right);
                    child->parent = parent;
                    parent->parent = link;
                    switch (link_side) {
                        case null:
                            tree = parent;
                            break;
                        case left:
                            link->left = parent;
                            break;
                        case right:
                            link->right = parent;
                            break;
                        default:
                            Log(L, 3, "could not link subtree to rest of tree");
                            break;
                    }
                    break;
                case zigzagright:
                    grandchild->left = parent->right;
                    grandchild->size_left = node_size(grandchild->left);
                    if (grandchild->left != NULL)
                        grandchild->left->parent = grandchild;
                    child->right = parent->left;
                    child->size_right = node_size(child->right);
                    if (child->right != NULL)
                        child->right->parent = child;
                    parent->left = child;
                    parent->right = grandchild;
                    parent->size_left = node_size(parent->left);
                    parent->size_right = node_size(parent->right);
                    grandchild->parent = parent;
                    child->parent = parent;
                    parent->parent = link;
                    switch (link_side) {
                        case null:
                            tree = parent;
                            break;
                        case left:
                            link->left = parent;
                            break;
                        case right:
                            link->right = parent;
                            break;
                        default:
                            Log(L, 3, "could not link subtree to rest of tree");
                            break;
                    }
                    break;
                case zigzigright:
                    grandchild->right = child->left;
                    grandchild->size_right = node_size(grandchild->right);
                    if (grandchild->right != NULL)
                        grandchild->right->parent = grandchild;
                    grandchild->parent = child;
                    child->right = parent->left;
                    child->size_right = node_size(child->right);
                    if (child->right != NULL)
                        child->right->parent = child;
                    child->left = grandchild;
                    child->size_left = node_size(child->left);
                    parent->left = child;
                    parent->size_left = node_size(parent->left);
                    child->parent = parent;
                    parent->parent = link;
                    switch (link_side) {
                        case null:
                            tree = parent;
                            break;
                        case left:
                            link->left = parent;
                            break;
                        case right:
                            link->right = parent;
                            break;
                        default:
                            Log(L, 3, "could not link subtree to rest of tree");
                            break;
                    }
                    break;
                case zigzagleft:
                    grandchild->right = parent->left;
                    if (grandchild->right != NULL)
                        grandchild->right->parent = grandchild;
                    grandchild->size_right = node_size(grandchild->right);
                    child->left = parent->right;
                    if (child->left != NULL)
                        child->left->parent = child;
                    child->size_left = node_size(child->left);
                    parent->left = grandchild;
                    parent->right = child;
                    parent->size_left = node_size(parent->left);
                    parent->size_right = node_size(parent->right);
                    grandchild->parent = parent;
                    child->parent = parent;
                    parent->parent = link;
                    switch (link_side) {
                        case null:
                            tree = parent;
                            break;
                        case left:
                            link->left = parent;
                            break;
                        case right:
                            link->right = parent;
                            break;
                        default:
                            Log(L, 3, "could not link subtree to rest of tree");
                            break;
                    }
                    break;
                case cerror:
                    Log(L, 3, "problem with splaying index %ld\n", node->size_left + node->piece->length);
                    return tree;
            }
        }
    }
    return tree;
}

int max(int a, int b) {
    return a > b ? a : b;
}

struct Tree *Insert(struct Tree *tree, struct Piece *piece, unsigned long int index) {
    struct Tree *inserted = NULL;
    unsigned long int offset = 0;
    tree = BSTInsert(tree, &inserted, piece, index);
    return tree;
}

void RecursiveInorder(struct Tree *tree, unsigned long int *offset) {
    if (tree == NULL)
        return;
    RecursiveInorder(tree->left, offset);
    printf("%ld\t\t%ld\t\t%c\n", *offset, tree->piece->length, *tree->piece->start);
    *offset += tree->piece->length;
    RecursiveInorder(tree->right, offset);
}

void TraverseInorder(struct Tree *tree) {
    unsigned long int offset = 0;
    RecursiveInorder(tree, &offset);
    printf("%ld\n", offset);
}

void PrintInorder(struct Tree *tree, FILE *p) {
    if  (tree == NULL) return;
    PrintInorder(tree->left, p);
    fprintf(p, "%.*s", (int)tree->piece->length, tree->piece->start);
    PrintInorder(tree->right, p);
}

void FreeTree(struct Tree *tree) {
    if (tree == NULL) return;
    if (tree->right == NULL) {
        free(tree->right);
        tree->right = NULL;
        return;
    } else if (tree->left == NULL) {
        free(tree->left);
        tree->left = NULL;
        return;
    }
    FreeTree(tree->left);
    FreeTree(tree->right);
}


void print_to_file(double x, double y, FILE *p) {
    fprintf(p, "%lf %.10lf\n", x, y);
}