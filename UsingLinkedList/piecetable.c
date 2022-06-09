#include <stdio.h>
#include <stdlib.h>
#include "piecetable.h"
#include <string.h>
#include <limits.h>
#include "gui.h"

char *added;
char *original;
int addedIndex;
Editor E;

// Reverse Array utility function
void revArr(int *arr, int size)
{
  int temp;
  for (int i = 0, j = size - 1; i < j; i++, j--)
  {
    temp = arr[i];
    arr[i] = arr[j];
    arr[j] = temp;
  }
}

void init(pieceTable *PT)
{
  (*PT).head = newPieceNode(NULL, -1, -2, 0);
  (*PT).tail = newPieceNode(NULL, -1, -2, 0);
  (*PT).head->next = (*PT).tail;
  (*PT).tail->prev = (*PT).head;
}

pieceNode *newPieceNode(char *buffer, int start, int end, int bufferType)
{
  pieceNode *newNode = (pieceNode *)malloc(sizeof(pieceNode));
  newNode->buffer = buffer;
  newNode->start = start;
  newNode->end = end;
  newNode->bufferType = bufferType;
  newNode->lineBreak = NULL;
  int count = 0, offset = 1;
  for (int i = start; i <= end; i++)
  {
    if (buffer[i] == '\n')
    {
      newNode->lineBreak = (int *)realloc(newNode->lineBreak, (count + 1) * sizeof(int));
      newNode->lineBreak[count++] = offset;
      offset = 0;
    }
    offset++;
  }
  newNode->lineCount = count;
  newNode->next = newNode->prev = NULL;
  return newNode;
}

void splitNodeForInsert(pieceNode *node, int splitIndex, int midNodeLen)
{
  pieceNode *previous = node->prev;
  pieceNode *next = node->next;

  pieceNode *leftPiece = newPieceNode(node->buffer, node->start, node->start + splitIndex - 1, node->bufferType);
  pieceNode *midPiece = newPieceNode(added, addedIndex, addedIndex + midNodeLen - 1, 1);
  pieceNode *rightPiece = newPieceNode(node->buffer, node->start + splitIndex, node->end, node->bufferType);

  leftPiece->next = midPiece;
  midPiece->next = rightPiece;
  midPiece->prev = leftPiece;
  rightPiece->prev = midPiece;

  previous->next = leftPiece;
  leftPiece->prev = previous;

  next->prev = rightPiece;
  rightPiece->next = next;

  free(node);
}

void splitNodeForDelete(pieceNode *node, int splitIndex)
{
  pieceNode *previous = node->prev;
  pieceNode *next = node->next;
  if (splitIndex == 0)
  {
    return;
  }
  else
  {
    pieceNode *leftPiece = newPieceNode(node->buffer, node->start, node->start + splitIndex - 2, node->bufferType);
    pieceNode *rightPiece = newPieceNode(node->buffer, node->start + splitIndex, node->end, node->bufferType);
    if (node->buffer[node->start + splitIndex - 1] == '\n')
    {
      if (E.y > 0)
        E.y--;
      E.numrows--;
      E.x = E.rowLen[E.y] > 1 ? E.rowLen[E.y] : 0;
    }
    leftPiece->next = rightPiece;
    rightPiece->prev = leftPiece;
    rightPiece->next = next;
    leftPiece->prev = previous;
    previous->next = leftPiece;
    next->prev = rightPiece;
    free(node);
  }
}

void insertCharAt(pieceTable PT, int lineNo, int position)
{
  int currLine = 1;
  pieceNode *currNode = PT.head->next;

  while (currNode != PT.tail && currLine + currNode->lineCount < lineNo)
  {
    currLine += currNode->lineCount;
    currNode = currNode->next;
  }

  int offset = 0, splitIndex;

  for (int i = 0; i < lineNo - currLine; i++)
    offset += currNode->lineBreak[i];

  if (offset == 0 && position == 0)
  {
    pieceNode *newNode = newPieceNode(added, addedIndex, addedIndex, 1);
    newNode->next = currNode;
    newNode->prev = currNode->prev;
    currNode->prev->next = newNode;
    currNode->prev = newNode;
  }

  else if (currLine + currNode->lineCount == lineNo)
  {

    int offset_from_end = currNode->end - currNode->start + 1 - offset - position;

    int flag = 0;

    while (currNode != PT.tail && offset_from_end < 0)
    {
      flag = 1;
      currNode = currNode->next;
      if (currNode->lineCount != 0)
        offset_from_end += currNode->lineBreak[0];
      else
        offset_from_end += currNode->end - currNode->start + 1;

    }

    if (!currNode->lineCount || !flag)
      splitIndex = currNode->end - currNode->start + 1 - offset_from_end;
    else
      splitIndex = currNode->lineBreak[0] - offset_from_end;

    if (currNode->bufferType == 1 && offset_from_end == 0)
    {
      if (currNode->end + 1 == addedIndex)
      {
        if (currNode->buffer[addedIndex] == '\n')
        {
          int lastLineOffset = 0;
          currNode->lineCount++;
          currNode->lineBreak = (int *)realloc(currNode->lineBreak, currNode->lineCount * sizeof(int));
          currNode->end++;
          for (int i = 0; i < currNode->lineCount - 1; i++)
            lastLineOffset += currNode->lineBreak[i];
          currNode->lineBreak[currNode->lineCount - 1] = currNode->end - currNode->start + 1 - lastLineOffset;
        }
        else
          currNode->end++;
      }
      else
      {
        pieceNode *newNode = newPieceNode(added, addedIndex, addedIndex, 1);
        newNode->next = currNode->next;
        currNode->next->prev = newNode;
        currNode->next = newNode;
        newNode->prev = currNode;
      }
    }
    else if (offset_from_end == 0)
    {
      pieceNode *newNode = newPieceNode(added, addedIndex, addedIndex, 1);
      newNode->next = currNode->next;
      currNode->next->prev = newNode;
      currNode->next = newNode;
      newNode->prev = currNode;
    }
    else
    {
      splitNodeForInsert(currNode, splitIndex, 1);
    }
  }
  else
  {
    splitIndex = offset + position;
    splitNodeForInsert(currNode, splitIndex, 1);
  }
}

void deleteCharAt(pieceTable PT, int lineNo, int position)
{
  int currLine = 1;
  pieceNode *currNode = PT.head->next;
  while (currNode != PT.tail && currLine + currNode->lineCount < lineNo)
  {
    currLine += currNode->lineCount;
    currNode = currNode->next;
  }

  int offset = 0, splitIndex;

  for (int i = 0; i < lineNo - currLine; i++)
    offset += currNode->lineBreak[i];

  if (offset == 0 && position == 0)
  {
    return;
  }
  else if (currLine + currNode->lineCount == lineNo)
  {
    int offset_from_end = currNode->end - currNode->start + 1 - offset - position;

    int flag = 0;

    while (currNode != PT.tail && offset_from_end < 0)
    {
      flag = 1;
      currNode = currNode->next;
      if (currNode->lineCount != 0)
        offset_from_end += currNode->lineBreak[0];
      else
        offset_from_end += currNode->end - currNode->start + 1;

    } 
    if (!currNode->lineCount || !flag)
      splitIndex = currNode->end - currNode->start + 1 - offset_from_end;
    else
      splitIndex = currNode->lineBreak[0] - offset_from_end;

    if (offset_from_end == 0)
    {
      if (currNode->start - currNode->end == 0)
      {
        if (currNode->buffer[currNode->end] == '\n')
        {
          if (E.y > 0)
            E.y--;
          E.numrows--;
          E.x = E.rowLen[E.y] > 1 ? E.rowLen[E.y] : 0;
        }
        currNode->prev->next = currNode->next;
        currNode->next->prev = currNode->prev;
        free(currNode);
      }
      else
      {
        if (currNode->buffer[currNode->end] == '\n')
        {
          if (E.y > 0)
            E.y--;
          E.numrows--;
          E.x = E.rowLen[E.y] > 1 ? E.rowLen[E.y] : 0;
          currNode->lineCount--;
          currNode->lineBreak = (int *)realloc(currNode->lineBreak, currNode->lineCount * sizeof(int));
        }
        currNode->end--;
      }
    }
    else if (splitIndex == 1)
    {
      if (currNode->buffer[currNode->start] == '\n')
      {
        if (E.y > 0)
          E.y--;
        E.numrows--;
        E.x = E.rowLen[E.y] > 1 ? E.rowLen[E.y] : 0;
        revArr(currNode->lineBreak, currNode->lineCount);
        currNode->lineCount--;
        currNode->lineBreak = (int *)realloc(currNode->lineBreak, currNode->lineCount * sizeof(int));
        revArr(currNode->lineBreak, currNode->lineCount);
      }
      else
      {
        if (currNode->lineCount)
          currNode->lineBreak[0]--;
      }
      currNode->start++;
    }
    else
    {
      splitNodeForDelete(currNode, splitIndex);
    }
  }
  else
  {
    splitIndex = offset + position;
    if (splitIndex == 1)
    {
      if (currNode->buffer[currNode->start] == '\n')
      {
        if (E.y > 0)
          E.y--;
        E.numrows--;
        E.x = E.rowLen[E.y] > 1 ? E.rowLen[E.y] : 0;
        revArr(currNode->lineBreak, currNode->lineCount);
        currNode->lineCount--;
        currNode->lineBreak = (int *)realloc(currNode->lineBreak, currNode->lineCount * sizeof(int));
        revArr(currNode->lineBreak, currNode->lineCount);
      }
      else
      {
        if (currNode->lineCount)
          currNode->lineBreak[0]--;
      }
      currNode->start++;
    }
    else
      splitNodeForDelete(currNode, splitIndex);
  }
}

/*  Function writes all content in PieceTable into file
 *  Returns written filesize
 */
int writeToFile(pieceTable PT, FILE *fp)
{
  pieceNode *currNode = PT.head;
  int fileSize = 0;
  while (currNode != PT.tail)
  {
    fprintf(fp, "%.*s", currNode->end - currNode->start + 1, currNode->start + currNode->buffer);
    fileSize += currNode->end - currNode->start + 1;
    currNode = currNode->next;
  }
  return fileSize;
}

/* Function converts provided lineNo and position into position relative to nodes and changes count to required node count
 */
int getIndexInNode(pieceTable PT, int lineNo, int position, int *count)
{
  int currLine = 1;
  pieceNode *currNode = PT.head->next;

  while (currNode != PT.tail && currLine + currNode->lineCount < lineNo)
  {
    currLine += currNode->lineCount;
    currNode = currNode->next;
    (*count)++;
  }

  int offset = 0, splitIndex = 0;

  for (int i = 0; i < lineNo - currLine; i++)
    offset += currNode->lineBreak[i];

  if (currLine + currNode->lineCount == lineNo)
  {

    int offset_from_end = currNode->end - currNode->start + 1 - offset - position;

    int flag = 0;

    while (currNode != PT.tail && offset_from_end < 0)
    {
      flag = 1;
      currNode = currNode->next;
      (*count)++;
      if (currNode->lineCount != 0)
        offset_from_end += currNode->lineBreak[0];
      else
        offset_from_end += currNode->end - currNode->start + 1;

    }

    if (!currNode->lineCount || !flag)
      splitIndex = currNode->end - currNode->start + 1 - offset_from_end;
    else
      splitIndex = currNode->lineBreak[0] - offset_from_end;
  }
  else
  {
    splitIndex = offset + position;
  }
  if (splitIndex == currNode->end - currNode->start + 1)
  {
    splitIndex = 0;
    (*count)++;
  }
  return splitIndex;
}

void deletePieceTable(pieceTable *PT)
{
  pieceNode *currNode = PT->head;
  PT->head = NULL;
  pieceNode *prev = NULL;
  while (currNode != PT->tail)
  {
    prev = currNode;
    currNode = currNode->next;
    free(prev->lineBreak);
    free(prev);
  }
  free(PT->tail);
  PT->tail = NULL;
}

/*                      DEBUG PIECETABLE                       */

void printPieceTable(pieceTable PT, FILE *fp)
{
  pieceNode *currNode = PT.head->next;
  int count = 0;
  while (currNode != PT.tail)
  {
    count++;
    fprintf(fp, "\n--------------------------------------------------------\n");
    fprintf(fp, "Node %d: \n%.*s\n", count, currNode->end - currNode->start + 1, currNode->start + currNode->buffer);
    fprintf(fp, "Buffer Type : %d\nLine Count : %d\n", currNode->bufferType, currNode->lineCount);
    fprintf(fp, "Start : %d\nEnd : %d\n", currNode->start, currNode->end);
    fprintf(fp, "\n--------------------------------------------------------\n");
    currNode = currNode->next;
  }
}