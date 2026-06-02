#include <stdbool.h>
#include <ncurses.h>
#include "../Include/ui.h"
#define maximumFileNameLength 60

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