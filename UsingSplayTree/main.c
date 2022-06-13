#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include "types.h"
#include "PieceTable.h"
#include <ncurses.h>

int main() {
    struct Tree *tree = NULL;
    char buffer[800];

    initscr();
    noecho();

    keypad(stdscr, TRUE);
    int ch;
    int x,y;
    int cx;
    int cy;
    int start = 0;
    mvaddstr(3, 0, "Tree Contents:");
    move(0,0);
    for(;;) {
        if ((ch = getch()) != ERR) {
            switch(ch) {
                case KEY_BACKSPACE:
                case KEY_DC:
                case 127:
                    break;
                case KEY_LEFT:
                    getyx(stdscr, y, x);
                    move(y, x-1);
                    break;
                case KEY_RIGHT:
                    getyx(stdscr, y, x);
                    move(y, x+1);
                    break;
                default:
                    getyx(stdscr, cy, cx);
                    tree = Insert(tree, MakePiece((char *) (&ch), 1), cx);
                    getyx(stdscr, cy, cx);
                    PrintS(tree, buffer, &start);
                    buffer[start] = '\0';
                    mvaddstr(4, 0, buffer);
                    mvaddstr(0, 0, buffer);
                    move(cy, cx+1);
                    start = 0;
                    break;
            }
        }
    }
}