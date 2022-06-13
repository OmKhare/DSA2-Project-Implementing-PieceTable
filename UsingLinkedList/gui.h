#ifndef GUI_H_INCLUDED
#define GUI_H_INCLUDED

#include<termios.h>
#include<time.h>
#include "piecetable.h"


typedef struct writeBuffer{
    int size;                   // Size of buf
    char* buf;                  // Char array
}writeBuffer;


typedef struct Editor{
    struct termios terminal_orig;   // Stores Terminal Attributes before program execution
    int screenrows, screencols;     // Terminal Size as rows and cols
    pieceTable PT;                  // PieceTable Data Type for file contents
    int numrows;                    // Total number of lines in file or Piece Table
    int rowOffset;                  // Stores offset by which rows are scolled(used for vertical scrolling)
    int colOffset;                  // Stores offset by which cols are scolled(used for horizontal scrolling)
    int x, y;                       // Used for Cursor position considering screen as co-ordinate axes
    int* rowLen;                    // Store lenght of each row displayed on screen(start from 0)
    char* filename;                 // Store name of file opened
    char message[100];              // Used for status messsage
    time_t message_time;            // Time for which status message display
    char* copyBuff;                 // Stores copied content when CTRL-c pressed
    int copyBuffLen;                // Length of copied buffer
    int fileChanged;                // 1 if File has been changed 0 if file has no changes
}Editor;

// Declaring Editor as Extern
extern Editor E;


// Error Handling
void returnError(char* s);

// Low level terminal handling
void rawModeON();
void rawModeOFF();
int readCharFromTerminal();
int getWindowSize(int *rows, int* cols);
void openEditor(char *filename);
void saveFile();

// Updating Terminal View
void initWriteBuffer(writeBuffer* wb);
void appendToWriteBuffer(writeBuffer* wb, char* s, int size);
void destroyWriteBuffer(writeBuffer* wb);
void refreshScreen();
void displayRows(writeBuffer* wb);
void printLinesToWriteBuffer(writeBuffer* wb,pieceTable PT, int firstLine, int lastLine,int selectStartRow, int selectStartCol);
void drawStatusBar(writeBuffer* wb);
void drawMessageBar(writeBuffer* ab);
void setStatusMessage(const char *fmt, ...);
char *prompt(char* prompt, void (*callback)(char *, int));

// Help Mode
void helpDisplay(writeBuffer* wb);

// Editor events handling
void cursorMovement(int key);
void scrollScreen();
void processKeyInput();

// Go to Line Number Section
void editorGoToLineCallBack(char* query, int key);
void editorGoToLine();

void initEditor();
void deleteEditor();
void test();
#endif