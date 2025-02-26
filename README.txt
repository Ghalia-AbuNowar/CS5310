Assignment 3

Part 1: 
For part 1 we understood the example from the repository and implemented a new visitor
SceneGraphTextRenderer.txt which implements the SGNodeVisitor and uses it to create a text 
rendering for a specific scenegraph.
The part included creating a callTextRenderer method in the view to create a text rendering for the provided 
scenegraph.

Part 2: 
The program creates multiple scenegraphs used in the rendering of the final model these include:
1) Assignment3_main.txt: this is the main scenegraph that combines all scenegraphs together

2) building.txt:  A scene graph for a single building, which includes:
    - Multiple windows.
    - The building-roof.txt file, which adds a roof structure to the building.
    
3) turret.txt: this creats the scenegraph for the turret structure

4) tree.txt: creats a tree structure 

5) hummanoid1 and hummanoid2: are the scenegraphs for the posed humans

for part 2 the command language was used to create the different scenegraphs, and create a meanigful structure that includes a flat surface, buildings that contain 10 instrances,
a turret structur as well as 2 posed humanoid figures

part3:
A trackball interface was implemented for camera rotation. To avoid gimbal lock, rotation matrices were used to update the camera position dynamically.

Trackball controlls:
    - pressing "r" or "R" resets the camera to initial position
    - mouse dragging rotates the camera around the scene

note: The program can load different scenegraphs using the command line by using something similar to this example: make run file="scenegraphmodels/assignment3_main.txt"
However, incase a file is not specified the default scenegraph will be rendered.

Tasks:
Ghalia Abu-Nowar: worked on the scenegraph creation + the trackball
Dharmil Yogesh Karia: worked on the Text rendering visitor + the 2 posed humanoids + the CLI implementation