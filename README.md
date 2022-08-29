## Introduction
NOTE: This is a class project from an intro to data-structures class. 

This program is a limited version of a text-editor that can be used to create art from text.

## Functions
| Name         | Function |
|--------------|----------|
| Animate      |Toggle that will animate other functions as they happen.                            |
| Undo         |Undo previous actions. Each time an action is done, the undo will be saved.         |
| Redo         |Redos an action. Doing a another action besides undo will reset the redo counter.    
| Clip         |Saves the current canvas and after a minimum of two clips, enables play function.   |
| Play         |Plays all canvas's that have been clipped as an animation. Requires at least two clips.          |
| Edit         |Allows user to change individual characters on canvas however they'd like.          |
| Move         |Shifts the text on the canvas horizontally and vertically as desired. Text that leaves the screen is not saved.          |
| Replace      |Replaces a character with another on the entire canvas.          |
| Fill         |Fills entire (closed off) region with a character.           |
| Clear        |Clears the canvas.          |
| Load         |Loads text from a text-file in SavedFiles to be displayed.          |
| Save         |Saves current canvas to a text-file.          |
| Line         |Draws a line from one spot to another as indicated by the user.          |
| Box          |Draws a box with the center being what the user selects.          |
| Nested Boxes |Draws boxes in a nested format with the center being as the user selects.          |
| Tree         |Draws a tree with the base being where the user selects.          |
| Main Menu    |Returns to the main menu from the draw menu.          |
| Draw         |Goes to the draw menu from the main menu.          |
| Quit         |Quits the program.          |

## Specifications
Linked-lists are used to store undo's and redo's, with both being nodes that are inserted upon creation. They are also used in the clip action to store canvas's as desired by the user. The play action iterates through the list to create the animated effect.

The draw functions are done recursively. This is most evident with the tree function and the nested-box function.