/*
* This program allows display and editing of text art (also called ASCII art).
*/

#include <iostream>
#include <fstream>
#include <cctype>
#include <windows.h>
#include <conio.h>
#include "Definitions.h"
using namespace std;


int main()
{
	//linkedLists
	List redoList, undoList, clips;

	//current canvas node
	Node* current = newCanvas();

	//input
	char choiceUpper('p'), oldCh, newCh;
	int rowInput, colInput;

	//saveCanvas Variables
	char fName[FILENAMESIZE - 17];
	char fLoc[FILENAMESIZE];

	//loadCanvas Variables
	// NOTE: fileName size is FILENAMESIZE-16 to account for the 12 (includes both \'s) in the path, plus the 4 chars of ".txt" that must be appended to it.
	char fileName[FILENAMESIZE - 16], relativePath[FILENAMESIZE];

	// bools
	bool animate(false);
	while (choiceUpper != 'Q')
	{
		gotoxy(menuRow, menuLength - 15);
		for (int x = 0; x < 15; x++)
			cout << " ";
		gotoxy(menuRow-1, 37);
		for (int x = 0; x < 15; x++)
			cout << " ";

		char choice('X');

		displayCanvas(current->item);

		printf("<A>nimate: %c / <U>ndo: %d ", (animate ? 'Y' : 'N'), undoList.count);
		if (redoList.count > 0) { printf("/ Red<O>: %d ", redoList.count); }

		printf("/ Cl<I>p: %d ", clips.count);
		if (clips.count > 1) { printf(" / <P>lay"); }
		
		printf("\n");

		printf("%s", menu);
		cin.get(choice);
		while (cin.fail())
		{
			cin.clear();
			cin.ignore((numeric_limits<streamsize>::max)(), '\n');

			gotoxy(menuRow, menuLength - 15);
		for (int x = 0; x < 15; x++)
			cout << " ";
		gotoxy(menuRow-1, 37);
		for (int x = 0; x < 15; x++)
			cout << " ";

			printf("%s", menu);
			cin.get(choice);
		}

		choiceUpper = toupper(choice);
		switch (choiceUpper)
		{
		case 'A': // Animate
			if (animate == true) { animate = false; }
			else if (animate == false) { animate = true; }
			break;

		case 'E': // Edit

			clearLine(menuRow, menuLength);
			clearLine(menuRow - 1, menuLength);

			addUndoState(undoList, redoList, current);
			editCanvas(current->item);
			break;

		case 'M': // Move
			//copyCanvas(canvasBackup, canvas);

			clearLine(menuRow, menuLength);
			clearLine(menuRow - 1, menuLength);

			printf("Please select a row value: ");
			cin >> rowInput;
			printf("Please selected a col value: ");
			cin >> colInput;

			addUndoState(undoList, redoList, current);
			moveCanvas(current->item, rowInput, colInput);
			break;

		case 'R': // Replace
			
			clearLine(menuRow, menuLength);
			clearLine(menuRow-1, menuLength);

			cin.ignore((numeric_limits<streamsize>::max)(), '\n');
			printf("Enter character to replace: ");
			cin.get(oldCh);
			if (cin.fail())
			{
				cin.clear();
				cin.ignore((numeric_limits<streamsize>::max)(), '\n');
			}

			cin.ignore((numeric_limits<streamsize>::max)(), '\n');
			printf("Enter character to replace with: ");
			cin.get(newCh);
			if (cin.fail())
			{
				cin.clear();
				cin.ignore((numeric_limits<streamsize>::max)(), '\n');
			}
			addUndoState(undoList, redoList, current);
			replace(current->item, oldCh, newCh);
			break;

		case 'D':
			clearLine(menuRow, menuLength);
			clearLine(menuRow - 1, menuLength);

			menuTwo(current, undoList, redoList, clips, animate);
			break;

		case 'C':
			addUndoState(undoList, redoList, current);
			initCanvas(current->item);
			break;

		case 'U': // Undo
			restore(undoList, redoList, current);
			break;

		case 'O':
			restore(redoList, undoList, current);
			break;
		case 'L': // Load
			
			clearLine(menuRow, menuLength);
			clearLine(menuRow - 1, menuLength);

			cin.clear();
			cin.ignore((numeric_limits<streamsize>::max)(), '\n');
			printf("<C>anvas or <A>nimation ? "); // Single canvas or multiple
			cin.get(choice);
			choice = toupper(choice);
			if (cin.fail())
			{
				cin.clear();
				cin.ignore((numeric_limits<streamsize>::max)(), '\n');
				break;
			}
			if (choice == 'C' || choice == 'A')
			{
				clearLine(menuRow - 1, menuLength);
				printf("Enter the filename (don't enter 'txt'): ");

				cin.clear();
				cin.ignore((numeric_limits<streamsize>::max)(), '\n');
				cin.getline(fileName, FILENAMESIZE - 16);
				if (cin.fail())
				{
					cin.clear();
					cin.ignore((numeric_limits<streamsize>::max)(), '\n');
				}

				// Concatanating the path together forming SavedFiles\\<fileName>.txt
				snprintf(relativePath, FILENAMESIZE, "SavedFiles\\%s.txt", fileName);
			}

			if (choice == 'C') // Checking for c choice. 
			{

				addUndoState(undoList, redoList, current);
				if (!loadCanvas(current->item, relativePath))
				{
					cerr << "Error! File not loaded." << endl;
					system("pause");
				}

				clearLine(menuRow, menuLength);
				clearLine(menuRow - 1, menuLength);
				clearLine(menuRow + 1, menuLength);
				break;
			}
			else
			{
				if (loadClips(clips, fileName))
				{
					printf("Clips Loaded!\n");
					system("pause");
				}
				clearLine(menuRow+1, menuLength);
			}
			break;

		case 'S': // Save
			clearLine(menuRow, menuLength);
			clearLine(menuRow - 1, menuLength);

			cin.clear();
			cin.ignore((numeric_limits<streamsize>::max)(), '\n');
			printf("<C>anvas or <A>nimation ? "); // Single canvas or multiple
			cin.get(choice);
			choice = toupper(choice);
			if (cin.fail())
			{
				cin.clear();
				cin.ignore((numeric_limits<streamsize>::max)(), '\n');
				break;
			}
			if (choice == 'C' || choice == 'A')
			{
				clearLine(menuRow - 1, menuLength);
				cin.clear();
				cin.ignore((numeric_limits<streamsize>::max)(), '\n');
				printf("Type your desired file name (without .txt): ");
				cin.getline(fName, FILENAMESIZE - 17);
				if (cin.fail())
				{
					cin.clear();
					cin.ignore((numeric_limits<streamsize>::max)(), '\n');
				}

				if (choice == 'C')
				{
					snprintf(fLoc, FILENAMESIZE, "SavedFiles\\%s.txt", fName);
					if (!saveCanvas(current->item, fLoc))
					{
						cerr << "Error! File not saved." << endl;
						system("pause");
					}
					else
					{
						cout << "File saved!" << endl;
						system("pause");
					}
				}
				else
				{
					saveClips(clips, fName);
					printf("Clips Saved!\n");
					system("pause");
				}
				clearLine(menuRow + 1, menuLength);
			}
			break;
		
		case 'I': // Clip
			addNode(clips, newCanvas(current));
			break;

		case 'P': // Play
			clearLine(menuRow, menuLength);
			clearLine(menuRow - 1, menuLength);
			play(clips);
			break;

		default:
			break;
		}
	}
	// Delete all three lists and the current Node
	deleteList(undoList);
	deleteList(redoList);
	deleteList(clips);
	delete current;

	return 0;
}


void gotoxy(short row, short col)
{
	COORD pos = { col, row };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}


void clearLine(int lineNum, int numOfChars)
{
	// Move cursor to the beginning of the specified line on the console
	gotoxy(lineNum, 0);

	// Write a specified number of spaces to overwrite characters
	for (int x = 0; x < numOfChars; x++)
		cout << " ";

	// Move cursor back to the beginning of the line
	gotoxy(lineNum, 0);
}


void replace(char canvas[][MAXCOLS], char oldCh, char newCh)
{
	for (int row = 0; row < MAXROWS; row++)
		for (int col = 0; col < MAXCOLS; col++)
			if (canvas[row][col] == oldCh)
				canvas[row][col] = newCh;
}


void editCanvas(char canvas[][MAXCOLS])
{
	char input;
	int row = 0, col = 0;

	printf("Press <ESC> to stop editing");
	// Move cursor to row,col and then get
	// a single character from the keyboard
	gotoxy(row, col);
	input = _getch();

	while (input != ESC)
	{
		if (input == SPECIAL)
		{
			input = _getch();
			switch (input)
			{
			case LEFTARROW:
				if (col > 0)
				{
					col--;
				}
				break;
			case RIGHTARROW:
				if (col < MAXCOLS - 1)
				{
					col++;
				}
				break;
			case UPARROW:
				if (row > 0)
				{
					row--;
				}
				break;
			case DOWNARROW:
				if (row < MAXROWS - 1)
				{
					row++;
				}
				break;
			}
		}
		//If a key was a special condition.
		else if (input == '\0')

		{
			input = _getch();
		}
		// Prints a letter in the position of the cursor
		else if (32 <= input && input <= 126)
		{
			canvas[row][col] = input;
			printf("%c", input);
		}
		gotoxy(row, col);
		input = _getch();
	}

}


void moveCanvas(char canvas[][MAXCOLS], int rowValue, int colValue)
{
	// Set up and initialize temp canvas (fill with spaces) 
	char temp[MAXROWS][MAXCOLS];
	initCanvas(temp);

	for (int row(0); row < MAXROWS; row++)
	{
		// Check to make sure new location row is within the confines of the canvas
		if (row + rowValue < MAXROWS && row + rowValue> -1)
		{
			for (int col(0); col < MAXCOLS; col++)
			{
				// Check to make sure new location col is within the confines of the canvas
				if (col + colValue < MAXCOLS && col + colValue > -1)
				{
					// Set the canvas value to the temp canvas after adding the 
					temp[row + rowValue][col + colValue] = canvas[row][col];
				}
			}
		}
	}
	copyCanvas(canvas, temp);
}


void initCanvas(char canvas[][MAXCOLS])
{
	for (int row(0); row < MAXROWS; row++)
	{
		for (int col(0); col < MAXCOLS; col++)
		{
			canvas[row][col] = ' ';
		}
	}
}


void displayCanvas(char canvas[][MAXCOLS])  //TODO: Redo function for effiency
{
	static char buffer[MAXROWS + 1][MAXCOLS + 2];
	
	for (int row(0); row < MAXROWS; row++)
	{
		memcpy(buffer[row], canvas[row], MAXCOLS);
		memcpy(&buffer[row][MAXCOLS], "|\n", 2);
	}

	// Print if at the end row, but not the end col
	for (int col(0); col < MAXCOLS + 1; col++)
	{
		buffer[MAXROWS][col] = '-';
	}
	buffer[MAXROWS][MAXCOLS + 1] = '\n';

	gotoxy(0, 0);
	cout.write(buffer[0], sizeof(buffer));

}


void copyCanvas(char to[][MAXCOLS], char from[][MAXCOLS])
{
	for (int i = 0; i < MAXROWS; i++)
		for (int j = 0; j < MAXCOLS; j++)
			to[i][j] = from[i][j];
}


bool saveCanvas(char canvas[][MAXCOLS], char filename[])
{
	//char tmp[MAXCOLS + 1];

	// opening the infile
	ofstream outFile;
	outFile.open(filename);

	// if the infile does not open
	if (!outFile)
	{
		cout << "ERROR";
		system("pause");
		return false;
	}
	else if (outFile.is_open())
	{
		for (int r = 0; r < MAXROWS; r++)
		{
			for (int c = 0; c < MAXCOLS; c++)
			{
				if (canvas[r][c] == '\0')
					outFile << " ";
				else
					outFile << canvas[r][c];
			}

			outFile << "\n";
		}

	}
	return true;
}


bool loadCanvas(char canvas[][MAXCOLS], char filename[])
{
	int lineLength, rowCount(0);

	// fileTextLine to capture each line, fileName to capture file name input, relativePath to store the path to the txt files.
	// NOTE: fileName size is FILENAMESIZE-16 to account for the 12 (includes both \'s) in the path, plus the 4 chars of ".txt" that must be appended to it.
	char fileTextLine[MAXCOLS + 1], fileName[FILENAMESIZE - 16];

	//Clear cin since there are issues if you don't.
	initCanvas(canvas);
	ifstream inFile(filename);
	if (!inFile)
	{
		return false;
	}

	do
	{
		inFile.getline(fileTextLine, MAXCOLS + 1);

		// Clears the buffer if a \n is never found, which causes a fail. 
		if (inFile.fail())
		{
			inFile.clear();
			inFile.ignore((numeric_limits<streamsize>::max)(), '\n');
		}

		lineLength = strnlen(fileTextLine, MAXCOLS + 1);
		for (int x(lineLength); x < MAXCOLS; x++)
		{
			fileTextLine[x] = ' '; // TODO: Investigate this warning.
		}
		for (int col(0); col < MAXCOLS; col++)
		{
			canvas[rowCount][col] = fileTextLine[col];
		}
		rowCount++;

	} while ((!inFile.eof()) && (rowCount < MAXROWS));
	return true;
}