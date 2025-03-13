Assignment 4

Part 1: 
For part 1 we create the scenegraph for the drone (drone.txt) and added it to the assignment3_main.txt
For efficiency a seperate scenegraph was made for the propellers (propellers.txt) this creates a scenegraph for 4 propeller blades.
this is then imported to the drone.txt scenegraph

Part 2: 
In this part we added the animation to increase or decrease the speed of the properllors for the drone.
We also added an animation in which the drone rolls over its "front-back" axis.

Note: the minimum speed for the propellers was clamped so that the propellers never reach a speed of zero.

Part 3:
In this part we added functionality for the user to control the drone using key presses.
The user can now move the drone forward and backward. They can also turn the drone left, right, top and bottom
using the arrow keys. The user can also now reset the drone position using the 'j' key.

Part 4:
In this part we created an option for user to use any of the following 3 camera positions.
Camera 1: Global camera. A camera location from which the entire scene is seen suitably.

Camera 2: Chopper camera: A rotating camera above the scene hovering over looking downwards on the scene.

Camera 3: A first person camera attached to the front of the drone

The user can switch to Cameras 1, 2 and 3 using the 1, 2 and 3 keys respectively.


Part 5:
Camera Visualization:
For this part we created a frustum model and added it to the existing scene. This frustum shows the camera position and its
fied of view. The angle of this field can be changed using the 'z' and 'y' keys.

Tasks:
Ghalia Abu-Nowar: worked on the drone scenegrphs and position updates + frustum
Dharmil Yogesh Karia: worked on the cameras and their integration with the scene.