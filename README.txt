Assignment 4

Part 1: Lights 
For part 1.1 the SGNODE was edited so it can incorporate adding multipule lights to any node in the scenegraph,
this also included editing the ScenegraphImporter so it can properly parse and assign lights 
and all their relative properites (ambient, diffuse, specular, position, direction and angle)

the material in the scenegraph was also changed so it can pass all the properties of the material.

The shader used was the phong shader, when the light is a spotlight it only calculates the fcolor inside the cone
 and assumes no light outside the cone of light, otherwise it assumes a point light and calculates the fcolor as normal

 we have added two spoint lights to the scene and a couple of spotlights:
 - one point light infront of the building
 - one point light above the building
 -two spotlights pointing at the huminoid figures
 - one spotlight pointing at the building
 - one spotlight attatched to the drone and will move according to the drone's movements

 the windows have a high shineness, this can be observed by comparing the windows that have a light directed at them and the ones that dont


Part 2: Toon Shading
For Part 2 we created the toon shaders and incorparated them with the current structure by
ading functionality to chnage the Shaders using the 'P' key

Part 3: Texture Mapping
texture was added to all the objects on scene the main objects 
that have visible texture are the building, ground and turret structures.

Tasks:
Ghalia Abu-Nowar: worked on the light and texture
Dharmil Yogesh Karia: worked on the light transformations and toon shading 