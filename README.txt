Assignment 7

Part 1: RayTracer

For part 1.1 we added shades to the rayTracer by creating a shadow ray from a point point towards that light, and checking if
the point sees the light or hits another object before it does so.

For part 1.2 (reflections) 




Part 2 : Extra Credit
We added support for adding textures(box and sphere) and refractions to our rayTracer

For Creative Modeling we have created a nature scene with a bear (made out of several spheres), a body of water made using a box object,
a couple of rocks (boxes), a flat mirror (box), a mood inside a transparent bubble(spheres) and a transparent bubble(sphere).



-- Note about the scenegraph: all the objects in the scenegraph have textures added, 
    the water as well as the box on the side have a high reflective value and act as mirrors,
    the bubble above the water is transparent and refractive, similarly the bubble surrounding the moon is also refractive.
    you can see the shadows produced by the lights on the bear as welll as the square blocks in the back.

-- note: one spotlight is added to display the ability to handle spotlights and two point light.

Tasks:
Ghalia Abu-Nowar: textures + scenegraph
Dharmil Yogesh Karia: did all the heavy lifting
