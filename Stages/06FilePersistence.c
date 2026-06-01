#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <ncurses.h>
#define bufferRows 256
#define bufferColumns 256
#define maximumFileNameLength 19
int min(int first, int second) {
    if(first < second) {
        return first;
    }
    return second;
}

// This function creates a temporary window, where-in the user is asked for a file name, and we open that file!
// the file is created in-case it doesn't already exist!
void promptFileName(char fileName[256]) {
    // Initiatine the prompt window!
    int height = 3, width = 40;
    int startX = 3, startY = 1;
    WINDOW *promptWindow = newwin(height, width, startY, startX);
    box(promptWindow, 0, 0);

    // Printing the prompt on-to the window!
    mvwprintw(promptWindow, 1, 1, "Enter file name: ");
    wrefresh(promptWindow);
    echo();

    // Moving the cursor!
    wmove(promptWindow, 1, maximumFileNameLength);
    wrefresh(promptWindow);

    // Getting user-input, then deleting this prompt window!
    wgetnstr(promptWindow, fileName, 19);
    noecho();
    delwin(promptWindow);

    // to clear up any residual-text!
    clear();
    refresh();
}

// This function will open a file, write the contents from the window onto the file, and close it!
// It'll be called only when user hits Ctrl + S!
void saveFile(char textBuffer[bufferRows][bufferColumns], int totalCount, char fileName[256]) {
    FILE *filePointer = fopen(fileName, "w");
    if(filePointer == NULL) {
        return;
    }
    for(int index = 0; index < totalCount; index++) {
        fprintf(filePointer, "%s\n", textBuffer[index]);
    }
    fclose(filePointer);
}

// This function will load the contents from Note.txt, and put them into the editor!
int loadFile(char textBuffer[bufferRows][bufferColumns], int lineLengths[bufferRows], int *lastRow, int *lastColumn, char fileName[256]) {
    FILE *filePointer = fopen(fileName, "r");
    if(filePointer == NULL) {
        return 0;
    }
    int index = 0;
    while(fgets(textBuffer[index], bufferColumns, filePointer)) {
        // This removes the trailing new-line added by fgets, the editor places a null-terminator at the end of every line!
        textBuffer[index][strcspn(textBuffer[index], "\n")] = '\0';

        // Updating the length of the current row!
        lineLengths[index] = strlen(textBuffer[index]);
        index++;
    }

    // Placing the cursor at the end of the last line!
    if(index > 0) {
        *lastRow = index - 1;
        *lastColumn = lineLengths[index - 1];
    }
    fclose(filePointer);
    return index;
}

int main()
{
    // Initialize the screen, and allow the user to exit the same by pressing Ctrl + C!
    initscr();

    // Allows us to detect Ctrl + S/Q combinations!
    raw();

    // Setting up a theme - black back-ground and blue fore-ground!
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    wbkgd(stdscr, COLOR_PAIR(1));
    clear();
    refresh();

    keypad(stdscr, TRUE); // Enable the arrow keys!    
    noecho(); // Avoid printing key-strokes to the screen!
    // curs_set(0); // Hide the real cursor!

    // We'll create a temporary window here, one that prompts the user for a file name!
    char fileName[256] = {};
    promptFileName(fileName);

    // Finally, setting up a text buffer, we'll re-draw the screen every-time!
    char textBuffer[bufferRows][bufferColumns] = {};

    int row = 0, column = 0;

    // Replaced this with the totalCount logic!
    int maximumRow, maximumColumn; // To prevent the user from dragging this character off the screen!
    getmaxyx(stdscr, maximumRow, maximumColumn);

    // To get rid of the warning!
    maximumColumn = maximumColumn;

    // An array to store the length of different lines!
    int lineLengths[bufferRows] = {};

    // Loading contents from the text file on-to the editor!
    // "totalCount" maintains a total count of valid lines!
    int totalCount = loadFile(textBuffer, lineLengths, &row, &column, fileName);

    if(totalCount == 0) {
        totalCount = 1;
    }

    char statusMessage[256] = "";

    while(true) {
        // mvprintw(row, column, "X");

        // Printing out the contents of the text buffer, that is, re-drawing the screen at every frame!
        clear();
        for(int index = 0; index < totalCount; index++) {
            // Cursor is automatically moved forward by n-curses, so we use mvprintw() to ensure each line is printed at column 0 of its row!
            mvprintw(index, 0, "%s", textBuffer[index]);
        }

        mvprintw(maximumRow - 2, 0, "%s", statusMessage);
        mvprintw(maximumRow - 1, 0, "%s | Line: %d | Column: %d", fileName, row, column);

        move(row, column);
        refresh();

        statusMessage[0] = '\0';
        int userInput = getch();

        // Check the two Ctrl + S/Q blocks!
        // Handle the user entering Ctrl + S at the start! ASCII-value for Ctrl + S is 19!
        if(userInput == 19) {
            saveFile(textBuffer, totalCount, fileName);
            strcpy(statusMessage, "Saved successfully! (Press any key to remove this message)");
        }

        // Handle exitng gracefully! ASCII-value of Ctrl + Q is 17!
        else if(userInput == 17) {
            break;
        }

        // Handling moving around the cursor!
        // If user presses up-arrow while row = 0, then do nothing! This prevents printing abstract characters!
        else if(userInput == KEY_UP && row == 0) {
            continue;
        }

        else if(userInput == KEY_UP && row > 0) {
            row--;
            // Moving from a longer line upward to a shorter line must move cursor to end of shorter line! 
            column = min(column, lineLengths[row]);
        }

        // To prevent printing abstract characters when user hits right arrow at last row, last column!
        else if(userInput == KEY_RIGHT && row == totalCount - 1 && column == lineLengths[row]) {
            continue;
        }

        // When user hits the right key at the end of the current row, the cursor must be moved to the start of the next row!
        else if(userInput == KEY_RIGHT && column == lineLengths[row]) {
            row++;
            column = 0;
        }
        
        else if(userInput == KEY_RIGHT && column < lineLengths[row]) {
            column++;
        }

        // If user tries to go below (totalCount - 1)'th line by pressing down-arrow key, do nothing! This prevents the insertion of abstract characters!
        else if(userInput == KEY_DOWN && row == totalCount - 1) {
            continue;
        }

        else if(userInput == KEY_DOWN && row < totalCount - 1) {
            row++;
            // Moving from a longer line downward to a shorter line must move cursor to end of shorter line! 
            column = min(column, lineLengths[row]);
        }

        // If user presses left key on row 0 and column 0, do nothing!
        else if(userInput == KEY_LEFT && row == 0 && column == 0) {
            continue;
        }

        // If user presses left key while on column 0, move cursor to end of previous row! This avoids printing abstract characters!
        else if(userInput == KEY_LEFT && column == 0) {
            row--;
            column = lineLengths[row];
        }
        
        else if(userInput == KEY_LEFT && column > 0) {
            column--;
        }
        
        // Handling the different cases when user hits back-space!
        // If user hits back-space when at row 0 and column 0, do nothing! This prevents the insertion of abstract characters!
        else if((userInput == KEY_BACKSPACE || userInput == 127) && column == 0 && row == 0) {
            continue;
        }
        
        else if((userInput == KEY_BACKSPACE || userInput == 127) && column == 0 && row > 0) {
            // If the user hits backspace when at column 0 of the current row, move him to the previous row, but only do so if the current row > 0!
            // row--;
            // column = lineLengths[row];

            // In actual text editors, all characters in the current row are copied over to the end of the previous row!
            // Copying over character's from current row into the end of the previous row!
            int previousLength = lineLengths[row - 1];
            strcpy(&textBuffer[row - 1][previousLength], textBuffer[row]);
            lineLengths[row - 1] = strlen(textBuffer[row - 1]);

            // Shifting all rows below one unit upward!
            for(int index = row; index < bufferRows - 1; index++) {
                strcpy(textBuffer[index], textBuffer[index + 1]);
                lineLengths[index] = lineLengths[index + 1];
            }

            // Emptying the last row!
            textBuffer[bufferRows - 1][0] = '\0';
            lineLengths[bufferRows - 1] = 0;
            
            // Updating cursor co-ordinates!
            row--;
            column = previousLength;

            // Updating the count of total valid lines!
            totalCount--;
        }
        else if((userInput == KEY_BACKSPACE || userInput == 127) && column > 0){
            column--;
            for(int index = column; index < lineLengths[row]; index++) {
                textBuffer[row][index] = textBuffer[row][index + 1];
            }
            // textBuffer[row][column] = '\0';
            lineLengths[row]--;
            move(row, column);
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

            // Updating the count of total valid lines!
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