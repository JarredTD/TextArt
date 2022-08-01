#include <iostream>
#include <Windows.h>
#include "Definitions.h"
using namespace std;


Node* newCanvas()
// Creates new Node with default properties.
{
	Node* canvasNode = new Node;

	// Set attributes of Node structure to default.
	initCanvas(canvasNode->item);
	canvasNode->next = NULL;

	return canvasNode;
}


Node* newCanvas(Node* oldNode)
// Creates a new Node with the same canvas as an oldNode
{
	Node* canvasNode = newCanvas();

	copyCanvas(canvasNode->item, oldNode->item);
	canvasNode->next = NULL;

	return canvasNode;
}


void play(List& clips)
// Plays all the clips per while loop. Does not run if there is less than 2 clip.
{
	// If there are no clips.
	if (clips.count < 2)
	{
		return;
	}
	// loops as long as the ESCAPE key is not currently being pressed
	while (!(GetKeyState(VK_ESCAPE) & 0x8000))
	{
		playRecursive(clips.head, clips.count);
	}
}


void playRecursive(Node* head, int count)
// Plays each clip in the clips List
{
	// If the end is reached.
	if (head == NULL) { return; }
	if (GetKeyState(VK_ESCAPE) & 0x8000) { return; }

	playRecursive(head->next, count - 1);
	displayCanvas(head->item);

	printf("Hold <ESC> to stop     Clip: %3d", count);
	Sleep(100);
	return;
}


void addUndoState(List& undoList, List& redoList, Node* &current)
// Add a node into the undoList and deletes the nodes in the redo list.
{
	addNode(undoList, newCanvas(current));
	deleteList(redoList);
}


void restore(List& undoList, List& redoList, Node* &current)
// Takes a node from the undoList and makes it the current node. Moves the prior current node into the redoList
{	
	if (undoList.count == 0)
	{
		return;
	}
	addNode(redoList, current);
	current = removeNode(undoList);
}


void addNode(List& list, Node* nodeToAdd)
// adds node to the front of list count
{
	nodeToAdd->next = list.head;
	list.head = nodeToAdd;

	list.count++;
}


Node* removeNode(List& list)
// Removes a node from the front of a list
{
	if (list.head != NULL)
	{
		Node* temp = list.head;
		list.head = list.head->next;
		temp->next = NULL;
		list.count--;
		return temp;
	}
	return NULL;
} 
 

void deleteList(List& list)
// Deletes all Nodes in a list
{

	Node* tmp;
	while (list.head != NULL)
	{
		tmp = list.head;
		list.head = list.head->next;

		delete tmp;
	}
	tmp = NULL;
	list.head = NULL;
	list.count = 0;
}


bool loadClips(List& clips, char filename[])
// Loads a set of files into the clips list
{
	char relativePath[FILENAMESIZE];
	int count(1);
	bool pass;
	bool loop(true);
	do
	{
		snprintf(relativePath, FILENAMESIZE, "SavedFiles\\%s-%d.txt", filename, count);
		Node tmp;
		Node* ptr = &tmp;

		pass = loadCanvas(tmp.item, relativePath);
		
		if (pass != true) { loop = false; }
		else { addNode(clips, newCanvas(ptr)); }

		count++;
	} while (loop == true);

	if (clips.count == 0) { return false; }
	return true;
}


bool saveClips(List& clips, char filename[])
// Saves nodes within the clips list into a folder with a -<num>.txt format
{
	char fileLocation[FILENAMESIZE];
	int fileNum = clips.count;

	for (Node* current(clips.head); current != NULL; current = current->next)
	{
		snprintf(fileLocation, FILENAMESIZE, "SavedFiles\\%s-%d.txt", filename, fileNum);
		if (!saveCanvas(current->item, fileLocation))
		{
			return false;
		}
		fileNum--;
	}
	return true;
}