#ifndef FILEIO_H
#define FILEIO_H

#define bufferRows 256
#define bufferColumns 256

void saveFile(char textBuffer[bufferRows][bufferColumns], int totalCount, char fileName[256]);

int loadFile(char textBuffer[bufferRows][bufferColumns], int lineLengths[bufferRows], int *lastRow, int *lastColumn, char fileName[256]);

#endif