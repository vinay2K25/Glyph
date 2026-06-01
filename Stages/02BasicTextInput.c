#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <ncurses.h>

int main()
{
    // Initialize the screen, and allow the user to exit the same by pressing Ctrl + C!
    initscr();
    cbreak();
    // curs_set(0); // This hides the cursor!
    keypad(stdscr, TRUE); // Enable the arrow keys!

    int row = 0, column = 0; // Co-ordinates of the cursor!

    while(true) {
        // clear(); // To remove the trailing X's!
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
        else {
            mvprintw(row, column, "%c", userInput);
            column++;
        }
    }
    endwin(); // Terminate the screen! 
    return 0;
}