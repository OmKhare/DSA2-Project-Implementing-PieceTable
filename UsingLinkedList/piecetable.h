#ifndef PIECETABLE_H_INCLUDED
#define PIECETABLE_H_INCLUDED

extern char* added;
extern char* original;
extern int addedIndex;

typedef struct pieceNode{
    int start;
    int end;
    int lineCount;
    int *lineBreak;             // Stores Offset from where Newline Starts in CurrNode
    char* buffer;               // Char Pointer Pointing either Added or Original
    int bufferType;             // Original(0)    Or    Added(1)
    struct pieceNode* next;
    struct pieceNode* prev;
}pieceNode;


typedef struct pieceTable{
    pieceNode* head;
    pieceNode* tail;
}pieceTable;

typedef struct cursorPosition{
    int lineNo;
    int col;
}cursorPosition;


void init(pieceTable* PT);

pieceNode* newPieceNode(char* buffer, int start, int end, int bufferType);

void splitNodeForInsert(pieceNode* node, int splitIndex, int midNodelen);

void splitNodeForDelete(pieceNode* node, int splitIndex);

void insertCharAt(pieceTable PT, int lineNo, int position);

void deleteCharAt(pieceTable PT, int lineNo, int position);

void insertLineAt(pieceTable PT, char* line,int lineLen, int lineNo, int position);

char* copyLine(pieceTable PT, int lineNo);

char* copyLineFrom(pieceTable PT, int startLine, int startCol,int copiedSize);

int writeToFile(pieceTable PT, FILE* fp);

int getIndexInNode(pieceTable PT, int lineNo, int position, int* count);

cursorPosition* searchInPT(pieceTable PT, char* str);

void deletePieceTable(pieceTable* PT);

void printPieceTable(pieceTable PT, FILE* fp);

#endif