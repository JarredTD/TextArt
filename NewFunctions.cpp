#include <iostream>
#include <windows.h>
#include <conio.h>
#include "Definitions.h"
using namespace std;


Point::Point(DrawPoint p)
{
	row = (int)round(p.row);
	col = (int)round(p.col);
}

// https://math.stackexchange.com/questions/39390/determining-end-coordinates-of-line-with-the-specified-length-and-angle
DrawPoint findEndPoint(DrawPoint start, int len, int angle)
{
	DrawPoint end;
	end.col = start.col + len * cos(degree2radian(angle));
	end.row = start.row + len * sin(degree2radian(angle));
	return end;
}


// Use this to draw characters into the canvas, with the option of performing animation
void drawHelper(char canvas[][MAXCOLS], Point p, char ch, bool animate)
{
	// Pause time between steps (in milliseconds)
	const int TIME = 50;

	// Make sure point is within bounds
	if (p.row >= 0 && p.row < MAXROWS && p.col >= 0 && p.col < MAXCOLS)
	{
		// Draw character into the canvas
		canvas[p.row][p.col] = ch;

		// If animation is enabled, draw to screen at same time
		if (animate)
		{
			gotoxy(p.row, p.col);
			printf("%c", ch);
			Sleep(TIME);
		}
	}
}


// Fills gaps in a row caused by mismatch between match calculations and screen coordinates
// (i.e. the resolution of our 'canvas' isn't very good)
void drawLineFillRow(char canvas[][MAXCOLS], int col, int startRow, int endRow, char ch, bool animate)
{
	// determine if we're counting up or down
	if (startRow <= endRow)
		for (int r = startRow; r <= endRow; r++)
		{
			Point point(r, col);
			drawHelper(canvas, point, ch, animate);
		}
	else
		for (int r = startRow; r >= endRow; r--)
		{
			Point point(r, col);
			drawHelper(canvas, point, ch, animate);
		}
}


// Draw a single line from start point to end point
void drawLine(char canvas[][MAXCOLS], DrawPoint start, DrawPoint end, bool animate)
{
	char ch;

	Point scrStart(start);
	Point scrEnd(end);

	// vertical line
	if (scrStart.col == scrEnd.col)
	{
		ch = '|';

		drawLineFillRow(canvas, scrStart.col, scrStart.row, scrEnd.row, ch, animate);
	}
	// non-vertical line
	else
	{
		int row = -1, prevRow;

		// determine the slope of the line
		double slope = (start.row - end.row) / (start.col - end.col);

		// choose appropriate characters based on 'steepness' and direction of slope
		if (slope > 1.8)  ch = '|';
		else if (slope > 0.08)  ch = '`';
		else if (slope > -0.08)  ch = '-';
		else if (slope > -1.8) ch = '\'';
		else ch = '|';

		// determine if columns are counting up or down
		if (scrStart.col <= scrEnd.col)
		{
			// for each column from start to end, calculate row values
			for (int col = scrStart.col; col <= scrEnd.col; col++)
			{
				prevRow = row;
				row = (int)round(slope * (col - start.col) + start.row);

				// draw from previous row to current row (to fill in row gaps)
				if (prevRow > -1)
				{
					drawLineFillRow(canvas, col, prevRow, row, ch, animate);
				}
			}
		}
		else
		{
			// for each column from start to end, calculate row values
			for (int col = scrStart.col; col >= scrEnd.col; col--)
			{
				prevRow = row;
				row = (int)round(slope * (col - start.col) + start.row);

				// draw from previous row to current row (to fill in row gaps)
				if (prevRow > -1)
				{
					drawLineFillRow(canvas, col, prevRow, row, ch, animate);
				}
			}
		}
	}
}


// Draws a single box around a center point
void drawBox(char canvas[][MAXCOLS], Point center, int height, bool animate)
{
	int sizeHalf = height / 2;
	int ratio = (int)round(MAXCOLS / (double)MAXROWS * sizeHalf);

	// Calculate where the four corners of the box should be
	DrawPoint points[4];
	points[0] = DrawPoint(center.row - sizeHalf, center.col - ratio);
	points[1] = DrawPoint(center.row - sizeHalf, center.col + ratio);
	points[2] = DrawPoint(center.row + sizeHalf, center.col + ratio);
	points[3] = DrawPoint(center.row + sizeHalf, center.col - ratio);

	// Draw the four lines of the box
	for (int x = 0; x < 3; x++)
	{
		drawLine(canvas, points[x], points[x + 1], animate);
	}
	drawLine(canvas, points[3], points[0], animate);

	// Replace the corners with a better looking character
	for (int x = 0; x < 4; x++)
	{
		drawHelper(canvas, points[x], '+', animate);
	}
}


// Menu for the drawing tools
void menuTwo(Node*& current, List& undoList, List& redoList, List& clips, bool& animate)
{
	// Inputs
	char choice, choiceUpper, newCh;
	int size, angle;

	// Points
	Point start, end, center;
	do
	{
		// Clear the menu lines but cleanly
		gotoxy(menuTwoRow, menuTwoLength - 25);
		for (int x = 0; x < 15; x++)
			cout << " ";
		gotoxy(menuTwoRow - 1, 37);
		for (int x = 0; x < 15; x++)
			cout << " ";

		displayCanvas(current->item);

		// Prints first half dependent on the animate boolean
		printf("<A>nimate: %c / <U>ndo: %d ", (animate ? 'Y' : 'N'), undoList.count);
		// print redo there are any nodes in redoList
		if (redoList.count > 0) { printf("/ Red<O>: %d ", redoList.count); }

		printf("/ Cl<I>p: %d ", clips.count);
		// print play if more than 1 node in clips
		if (clips.count > 1) { printf(" / <P>lay"); }

		printf("\n");
		printf("%s",menuTwoText);

		cin.get(choice);
		while (cin.fail())
		{
			cin.clear();
			cin.ignore((numeric_limits<streamsize>::max)(), '\n');

			clearLine(menuTwoRow, menuTwoLength);
			printf("%s",menuTwoText);
			cin.get(choice);
		}
		// After input validation, convert to uppercase
		choiceUpper = toupper(choice);

		switch (choiceUpper)
		{
		case 'A': // Animate
			if (animate == true) { animate = false; }
			else if (animate == false) { animate = true; }
			break;

		case 'F': // Fill
			clearLine(menuRow, menuLength);
			clearLine(menuRow - 1, menuLength);
			printf("Enter character to fill with from current location / <ESC> to cancel");
			newCh = getPoint(start);
			if (newCh != ESC)
			{
				addUndoState(undoList, redoList, current);
				fillRecursive(current->item, start.row, start.col, current->item[start.row][start.col], newCh, animate);
			}
			clearLine(menuRow, menuLength);
			clearLine(menuRow - 1, menuLength);
			break;

		case 'L': // Line
			clearLine(menuRow, menuLength);
			clearLine(menuRow - 1, menuLength);
			printf("Type any letter to choose start point / <ESC> to cancel");
			if (getPoint(start) != ESC)
			{
				clearLine(menuRow - 1, menuLength);
				printf("Type any letter to choose end point / <ESC> to cancel");
				if (getPoint(end) != ESC)
				{ 
					addUndoState(undoList, redoList, current);
					drawLine(current->item, DrawPoint(start), DrawPoint(end), animate);
				}

			}
			clearLine(menuRow, menuLength);
			clearLine(menuRow - 1, menuLength);
			break;

		case 'B': // Box
			clearLine(menuRow, menuLength);
			clearLine(menuRow - 1, menuLength);
			printf("Enter size: ");
			cin >> size;

			clearLine(menuRow - 1, menuLength);
			printf("Type any letter to choose box center, or <C> for screen center / <ESC> to cancel");
			choice = getPoint(center);
			if (choice != ESC)
			{
				choice = toupper(choice);
				if (choice == 'C')
				{
					center.row = rowCenter;
					center.col = colCenter;
				}
				addUndoState(undoList, redoList, current);
				drawBox(current->item, center, size, animate);
			}

			clearLine(menuRow, menuLength);
			clearLine(menuRow - 1, menuLength);
			break;

		case 'N': // Nested Boxes
			clearLine(menuRow, menuLength);
			clearLine(menuRow - 1, menuLength);
			printf("Enter the size of the largest box: ");
			cin >> size;

			clearLine(menuRow - 1, menuLength);
			printf("Type any letter to choose box center, or <C> for screen center / <ESC> to cancel");
			choice = getPoint(center);
			if (choice != ESC)
			{
				choice = toupper(choice);
				if (choice == 'C')
				{
					center.row = rowCenter;
					center.col = colCenter;
				}
				addUndoState(undoList, redoList, current);
				drawBoxesRecursive(current->item, center, size, animate);
			}

			clearLine(menuRow, menuLength);
			clearLine(menuRow - 1, menuLength);
			break;

		case 'T': // Tree
			clearLine(menuRow, menuLength);
			clearLine(menuRow - 1, menuLength);
			printf("Enter approximate tree height: ");
			cin >> size;

			clearLine(menuRow - 1, menuLength);
			printf("Enter branch angle: ");
			cin >> angle;

			clearLine(menuRow - 1, menuLength);
			printf("Type any letter to choose start point, or <C> for bottom center / <ESC> to cancel");

			choice = getPoint(start);
			if (choice != ESC)
			{
				choice = toupper(choice);
				if (choice == 'C')
				{
					start.row = MAXROWS-1;
					start.col = colCenter;
				}
				addUndoState(undoList, redoList, current);
				// Tree starts by going up
				treeRecursive(current->item, start, size, 270, angle, animate);
			}
			clearLine(menuRow - 1, menuLength);
			clearLine(menuTwoRow, menuTwoLength);
			break;

		case 'I': // Clip
			addNode(clips, newCanvas(current));
			break;

		case 'P': // Play
			clearLine(menuRow, menuLength);
			clearLine(menuRow - 1, menuLength);
			play(clips);
			break;

		case 'U': // Undo
			restore(undoList, redoList, current);
			break;

		case 'O': // Redo
			restore(redoList, undoList, current);
			break;

		default:
			break;
		}

	} while (choiceUpper!='M');
	return;
}


// Get a single point from screen, with character entered at that point
char getPoint(Point& pt)
{
	char input;
	int row = 0, col = 0;

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
		else if (32 <= input && input <= 126)
		{
			pt.row = row;
			pt.col = col;
			printf("%c", input);
			return input;
		}
		gotoxy(row, col);
		input = _getch();
	}
	return ESC;
}


// Recursively fill a section of the screen
void fillRecursive(char canvas[][MAXCOLS], int row, int col, char oldCh, char newCh, bool animate)
{
	{
		// Is within the bounds of the canvas
		if ((row < MAXROWS && row >= 0) && (col < MAXCOLS && col >= 0) && (canvas[row][col] == oldCh))
		{
			Point pt;
			pt.row = row;
			pt.col = col;
			
			
			drawHelper(canvas, pt, newCh, animate);
			fillRecursive(canvas, row - 1, col, oldCh, newCh, animate);
			fillRecursive(canvas, row + 1, col, oldCh, newCh, animate);
			fillRecursive(canvas, row, col - 1, oldCh, newCh, animate);
			fillRecursive(canvas, row, col + 1, oldCh, newCh, animate);
		}
		return;
	}
}


// Recursively draw a tree
void treeRecursive(char canvas[][MAXCOLS], DrawPoint start, int height, int startAngle, int branchAngle, bool animate)
{

	if (start.col < 0 || start.col > MAXCOLS || start.row < 0 || start.row > MAXROWS)
	{
		return;
	}

	if (height > 2)
	{
		int newStartAngle = startAngle % 360;
		DrawPoint endPoint = findEndPoint(start, height / 3, startAngle);
		drawLine(canvas, start, endPoint, animate);

		treeRecursive(canvas, endPoint, height - 2, startAngle + branchAngle, branchAngle, animate);
		treeRecursive(canvas, endPoint, height - 2, startAngle - branchAngle, branchAngle, animate);
	}
}


// Recursively draw nested boxes
void drawBoxesRecursive(char canvas[][MAXCOLS], Point center, int height, bool animate)
{
		if (height > 1)
		{
			// Draws box
			drawBox(canvas, center, height, animate);
			// Function called rescursively
			drawBoxesRecursive(canvas, center, height - 2, animate);
		}
	
}