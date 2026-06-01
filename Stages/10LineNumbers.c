#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <ncurses.h>
#define bufferRows 256
#define bufferColumns 256
#define maximumFileNameLength 60
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
    int height = 3, width = maximumFileNameLength + 25;
    int maximumX, maximumY;
    getmaxyx(stdscr, maximumY, maximumX); // It returns the maximum-height and width of the standard screen!

    if(width > maximumX) {
        width = maximumX;
    }

    // This will centralize the input-box!
    int startX = (maximumX - width)/2;
    int startY = (maximumY - height)/2;

    // Preventing negative starting co-ordinates!
    if(startX < 0) {
        startX = 0;
    }
    if(startY < 0) {
        startY = 0;
    }

    WINDOW *promptWindow = newwin(height, width, startY, startX);
    box(promptWindow, 0, 0);

    // Printing the prompt on-to the window!
    mvwprintw(promptWindow, 1, 1, "Enter file name: ");
    wrefresh(promptWindow);
    echo();

    // Moving the cursor!
    wmove(promptWindow, 1, 18);
    wrefresh(promptWindow);

    // Getting user-input, then deleting this prompt window!
    wgetnstr(promptWindow, fileName, maximumFileNameLength);
    noecho();
    delwin(promptWindow);

    // To clear up any residual-text!
    clear();
    refresh();
}

// This function will prompt the user when they exit the app without saving their changes!
bool promptExitUnsaved() {
    int maximumX, maximumY;
    getmaxyx(stdscr, maximumY, maximumX);

    int height = 5, width = 45;
    int startX = (maximumX - width)/2;
    int startY = (maximumY - height)/2;

    // Prevent negative co-ordinates!
    if(startX < 0) {
        startX = 0;
    }
    if(startY < 0) {
        startY = 0;
    }

    WINDOW *exitWindow = newwin(height, width, startY, startX);
    box(exitWindow, 0, 0);

    // Printing the warning!
    wattron(exitWindow, COLOR_PAIR(3));
    mvwprintw(exitWindow, 1, 2, "Warning: You have unsaved changes!");
    wattroff(exitWindow, COLOR_PAIR(3));
    mvwprintw(exitWindow, 3, 2, "Quit anyway? (Y/N): ");
    wrefresh(exitWindow);

    // Taking input from the user!
    int choice = wgetch(exitWindow);

    // Closing the window!
    delwin(exitWindow);
    clear();
    refresh();

    // Returning true if user types 'Y' or 'y', false otherwise!
    if(choice == 'Y' || choice == 'y') {
        return true;
    }
    return false;
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

    // Setting up a theme - transparent back-ground and white fore-ground!
    start_color();
    use_default_colors();
    init_pair(1, COLOR_WHITE, -1);
    init_pair(2, COLOR_GREEN, -1); // Used for success!
    init_pair(3, COLOR_RED, -1); // Used for warnings!
    init_pair(4, COLOR_CYAN, -1); // Used for line numbering!
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

    // Decides which row will be shown at the top of the screen!
    int topVisibleRow = 0;

    // Decides which column will be shown on the left-side of the screen!
    int leftVisibleColumn = 0;

    // Gutter will contain the line numbers!
    int gutterWidth = 6;

    // Replaced this with the totalCount logic!
    int maximumRow, maximumColumn; // To prevent the user from dragging this character off the screen!
    getmaxyx(stdscr, maximumRow, maximumColumn);

    // Bottom 2 rows are used by status bar!
    int visibleRows = maximumRow - 3;

    // Maximum visible-columns available for text-rendering!
    int visibleColumns = maximumColumn - gutterWidth;

    // To get rid of the warning!
    maximumColumn = maximumColumn;

    // An array to store the length of different lines!
    int lineLengths[bufferRows] = {};

    // Loading contents from the text file on-to the editor!
    // "totalCount" maintains a total count of valid lines!
    int totalCount = loadFile(textBuffer, lineLengths, &row, &column, fileName);

    // Loads in all the lines at the end, and places cursor in correct position!
    // Now cursor does not appear over the "Column: Count" text!
    if(row > visibleRows - 1) {
        topVisibleRow = row - visibleRows + 1;
    }

    if(totalCount == 0) {
        totalCount = 1;
    }

    // To track if user tries exiting without saving!
    bool hasUnsavedChanges = false;

    char statusMessage[256] = "";

    while(true) {
        // Updating screen dimensions in-case user re-sizes the terminal!
        getmaxyx(stdscr, maximumRow, maximumColumn);
        visibleRows = maximumRow - 3;
        visibleColumns = maximumColumn - gutterWidth;

        // Calculating a dynamic left-margin! It uses ~2% of the screen-width, with a minimum padding of one character!
        int leftMargin = maximumColumn/50;
        if(leftMargin < 1) {
            leftMargin = 1;
        }

        // If topVisibleRow becomes negative, then it implies all content from screen was removed. so reset it!
        if(topVisibleRow < 0) {
            topVisibleRow = 0;
        }

        // Handling negative-index for horizontal-scrolling too!
        if(leftVisibleColumn < 0) {
            leftVisibleColumn = 0;
        }

        // Printing out the contents of the text buffer, that is, re-drawing the screen at every frame!
        clear();
        for(int screenRow = 0; screenRow < visibleRows; screenRow++) {
            // Row number which will be shown!
            int bufferRow = topVisibleRow + screenRow;

            // Line-number for current row!
            int lineNumber = bufferRow + 1;

            // If row-number exceeds the last valid row, then it's not meant to be shown!
            if(bufferRow > totalCount - 1) {
                break;
            }

            // Printing the line-number first!
            attron(COLOR_PAIR(4));
            mvprintw(screenRow, 0, "%4d  ", lineNumber);
            attroff(COLOR_PAIR(4));

            // Start printing these rows one by one on-to the screen, starting from the top of the screen!
            mvprintw(screenRow, leftMargin + gutterWidth, "%.*s", visibleColumns - leftMargin - 1, &textBuffer[bufferRow][leftVisibleColumn]);
        }

        // Status message displayed!
        attron(COLOR_PAIR(2));
        mvprintw(maximumRow - 2, leftMargin, "%s", statusMessage);
        attroff(COLOR_PAIR(2));

        // Un-comment these three lines to apply the A_REVERSE attribute on the entire line!
        // attron(A_REVERSE);
        // mvhline(maximumRow - 1, 0, ' ', maximumColumn);
        if(hasUnsavedChanges) {
            mvprintw(maximumRow - 1, leftMargin, "%s [*] | Line: %d | Column: %d | Ctrl+Q: Exit | Ctrl+S: Save", fileName, row, column);    
        }
        else  {
            mvprintw(maximumRow - 1, leftMargin, "%s | Line: %d | Column: %d | Ctrl+Q: Exit | Ctrl+S: Save", fileName, row, column);
        }
        // attroff(A_REVERSE);

        // Must handle maximumColumn now too!
        move(row - topVisibleRow, column - leftVisibleColumn + leftMargin + gutterWidth);
        refresh();

        statusMessage[0] = '\0';
        int userInput = getch();

        // Check the two Ctrl + S/Q blocks!
        // Handle the user entering Ctrl + S at the start! ASCII-value for Ctrl + S is 19!
        if(userInput == 19) {
            saveFile(textBuffer, totalCount, fileName);
            hasUnsavedChanges = false;
            strcpy(statusMessage, "Saved successfully! (Press any key to remove this message)");
        }

        // Handle exitng gracefully! ASCII-value of Ctrl + Q is 17!
        else if(userInput == 17) {
            if(hasUnsavedChanges) {
                if(promptExitUnsaved()) {
                    break;
                }
                else {
                    continue;
                }
            }
            else {
                break;
            }
        }

        // Handling moving around the cursor!
        // If user presses up-arrow while row = 0, then do nothing! This prevents printing abstract characters!
        else if(userInput == KEY_UP && row == 0) {
            continue;
        }

        else if(userInput == KEY_UP && row > 0) {
            row--;

            // Up-ward scrolling!
            if(row < topVisibleRow) {
                topVisibleRow--;
            }

            // Moving from a longer line upward to a shorter line must move cursor to end of shorter line! 
            column = min(column, lineLengths[row]);

            // Handling horizontal-scrolling!
            if(column < leftVisibleColumn) {
                leftVisibleColumn = column;
            }
            else if(column - leftVisibleColumn > visibleColumns - leftMargin - 2) {
                leftVisibleColumn = column - (visibleColumns - leftMargin - 2);
            }
        }

        // To prevent printing abstract characters when user hits right arrow at last row, last column!
        else if(userInput == KEY_RIGHT && row == totalCount - 1 && column == lineLengths[row]) {
            continue;
        }

        // When user hits the right key at the end of the current row, the cursor must be moved to the start of the next row!
        else if(userInput == KEY_RIGHT && column == lineLengths[row]) {
            row++;
            column = 0;
            leftVisibleColumn = 0;

            // Down-ward scrolling!
            if(row > topVisibleRow + visibleRows - 1) {
                topVisibleRow++;
            }
        }
        
        else if(userInput == KEY_RIGHT && column < lineLengths[row]) {
            column++;

            // Horizontal-scrolling to the right!
            if(column - leftVisibleColumn > visibleColumns - leftMargin - 2) {
                leftVisibleColumn++;
            }
        }

        // If user tries to go below (totalCount - 1)'th line by pressing down-arrow key, do nothing! This prevents the insertion of abstract characters!
        else if(userInput == KEY_DOWN && row == totalCount - 1) {
            continue;
        }

        else if(userInput == KEY_DOWN && row < totalCount - 1) {
            row++;

            // Down-ward scrolling!
            if(row > topVisibleRow + visibleRows - 1) {
                topVisibleRow++;
            }

            // Moving from a longer line downward to a shorter line must move cursor to end of shorter line! 
            column = min(column, lineLengths[row]);

            // Handling horizontal-scrolling!
            if(column < leftVisibleColumn) {
                leftVisibleColumn = column;
            }
            else if(column - leftVisibleColumn > visibleColumns - leftMargin - 2) {
                leftVisibleColumn = column - (visibleColumns - leftMargin - 2);
            }
        }

        // If user presses left key on row 0 and column 0, do nothing!
        else if(userInput == KEY_LEFT && row == 0 && column == 0) {
            continue;
        }

        // If user presses left key while on column 0, move cursor to end of previous row! This avoids printing abstract characters!
        else if(userInput == KEY_LEFT && column == 0) {
            row--;
            column = lineLengths[row];

            if(column < leftVisibleColumn) {
                leftVisibleColumn = column;
            }
            else if(column - leftVisibleColumn > visibleColumns - leftMargin - 2) {
                leftVisibleColumn = column -(visibleColumns - leftMargin - 2);
            }

            // Up-ward scrolling!
            if(row < topVisibleRow) {
                topVisibleRow--;
            }
        }
        
        else if(userInput == KEY_LEFT && column > 0) {
            column--;

            // Horizontal-scrolling to the left!
            if(column < leftVisibleColumn) {
                leftVisibleColumn = column;
            }
        }
        
        // Handling the different cases when user hits back-space!
        // If user hits back-space when at row 0 and column 0, do nothing! This prevents the insertion of abstract characters!
        else if((userInput == KEY_BACKSPACE || userInput == 127) && column == 0 && row == 0) {
            continue;
        }
        
        else if((userInput == KEY_BACKSPACE || userInput == 127) && column == 0 && row > 0) {
            hasUnsavedChanges = true;
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
            hasUnsavedChanges = true;
            column--;
            for(int index = column; index < lineLengths[row]; index++) {
                textBuffer[row][index] = textBuffer[row][index + 1];
            }
            // textBuffer[row][column] = '\0';
            lineLengths[row]--;

            // Handling horizontal-scrolling while deleting!
            if(column < leftVisibleColumn) {
                leftVisibleColumn = column;
            }

            move(row - topVisibleRow, column - leftVisibleColumn + leftMargin + gutterWidth);
        }

        // Handling the case when the user presses enter!
        else if((userInput == KEY_ENTER || userInput == '\n') && totalCount < bufferRows - 1) {
            hasUnsavedChanges = true;
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
            leftVisibleColumn = 0;

            // Down-ward scrolling!
            if(row > topVisibleRow + visibleRows - 1) {
                topVisibleRow++;
            }

            // Updating the count of total valid lines!
            totalCount++;
        }

        // Handling typing, user should be able to type only printable ASCII-values!
        else if(userInput > 31 && userInput < 127){
            hasUnsavedChanges = true;
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

            // Handling horizontal-scrolling while typing!
            if(column - leftVisibleColumn > visibleColumns - leftMargin - 2) {
                leftVisibleColumn++;
            }
        }
    }

    endwin(); // Terminate the screen!
    return 0;
}