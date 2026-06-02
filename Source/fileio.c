#include <stdio.h>
#include <string.h>
#include "../Include/fileio.h"

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