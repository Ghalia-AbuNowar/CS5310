Assignment 6

Part 1: Ray Tracing

For part 1.1 we created a small scene and created new classes for the Ray and the HitRecord.

For part 1.2 to set up basic ray tracing we created a method in the view to help us
do ray tracing by creating an image array and runnig a for loop on every pixel. 

For part 1.3 we set up a rayCastRenderer which is essentially a visitor going through each 
node in our scenegraph and checking for the ray intersecting at each leaf node.


We also had to add a shade function to calculate the color of each pixel based on the hitRecord.

For 1.4 we had intersects functions in the leafnode and also for the group and transform we handled it accordingly.

We added support for calculating Ray intersections with sphere and a box and added them in the LeafNode 
so according to the ObjectInstance type the appropriate function would be called.


Part 2 : Extra Credit
We added support for calculating ray intersections for Cone and Cylinder and also included them in our
LeafNode class. 
For Creative Modeling we have created a scene with a box as the base, a snowman with a bear sitting besides a lake.
There are a few snowballs next to the snowman with a tree in the background. overall our scenegraph contains atleast 15 object istances 
all the supported shape instersections are present in the scenegraph.

-- note: two spotlights are added to display the ability to handle spotlights and one point light.

Tasks:
Ghalia Abu-Nowar: worked on the ray, hitRecord and rayRenderer class
Dharmil Yogesh Karia: worked on the shading function and the intersection functoins
