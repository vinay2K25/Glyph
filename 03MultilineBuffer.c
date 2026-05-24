#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <ncurses.h>
#define bufferRows 256
#define bufferColumns 256
int main()
{
    // Initialize the screen, and allow the user to exit the same by pressing Ctrl + C!
    initscr();
    cbreak();

    keypad(stdscr, TRUE); // Enable the arrow keys!

    int row = 0, column = 0; // Co-ordinates of the cursor!

    // Finally, setting up a text buffer, we'll re-draw the screen every-time!
    char textBuffer[bufferRows][bufferColumns] = {};

    // An array to store the length of different lines!
    int lineLengths[bufferRows] = {};

    // "totalCount" maintains a count of total valid lines!
    int totalCount = 1;

    while(true) {
        // Printing out the contents of the text buffer, that is, re-drawing the screen at every frame!
        clear();
        for(int index = 0; index < totalCount; index++) {
            // Cursor is automatically moved forward by n-curses, so we use mvprintw() to ensure each line is printed at column 0 of its row!
            mvprintw(index, 0, "%s", textBuffer[index]);
        }

        move(row, column);
        refresh();
        int userInput = getch();
        if(userInput == KEY_UP) {
            row--;
        }
        else if(userInput == KEY_RIGHT) {
            column++;
        }
        else if(userInput == KEY_DOWN) {
            row++;
        }
        else if(userInput == KEY_LEFT) {
            column--;
        }

        // Handling the case when the user presses enter!
        else if((userInput == KEY_ENTER || userInput == '\n') && totalCount < bufferRows - 1) {
            // Do not handle previousRow, previousColumn here, because it's not necessary the user always edits the current line, hits enter and moves on to next line, sometimes the user may have to move between different lines, and that messes up the values of previousRow and previousColumn!

            // Before copying over remaining characters to next row, move all rows below next row one unit below!
            for(int index = bufferRows - 1; index > row + 1; index--) {
                strcpy(textBuffer[index], textBuffer[index - 1]);
                lineLengths[index] = lineLengths[index - 1];
            }

            // Now copy over all characters to the right on-to the new line!
            strcpy(textBuffer[row + 1], &textBuffer[row][column]);

            // Set the null-terminator for the current row!
            textBuffer[row][column] = '\0';

            // Updating the length of lines row and row + 1 now!
            lineLengths[row] = strlen(textBuffer[row]);
            lineLengths[row + 1] = strlen(textBuffer[row + 1]);

            // Moving the cursor to the start of the newly created row!
            row++;
            column = 0;

            // Updating the total count!
            totalCount++;
        }

        // Handling typing, user should be able to type only printable ASCII-values!
        else if(userInput > 31 && userInput < 127){
            // mvprintw(row, column, "%c", userInput); won't be used, we'll be re-drawing the screen at every frame instead!
            // Before inserting the new character, shift all characters present on the right one unit right-ward, then insert the new character. This preserves characters when use types in-between the sentence!
            for(int index = lineLengths[row]; index > column - 1; index--) {
                textBuffer[row][index + 1] = textBuffer[row][index];
            }

            // Inserting the next character!
            textBuffer[row][column] = userInput;
            column++;

            // textBuffer[row][column] = '\0'; is no longer required, since we handle shifting the null-terminator ourselves!

            lineLengths[row]++;
        }
    }
    endwin(); // Terminate the screen! 
    return 0;
}