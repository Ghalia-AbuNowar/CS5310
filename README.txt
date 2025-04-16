Assignment 7

The images can be found in the _RenderedImages folder

Part 1: RayTracer

For part 1.1 we added shades to the rayTracer by creating a shadow ray from a point point towards that light, and checking if
the point sees the light or hits another object before it does so.

For part 1.2 we made changes to the existing getColor function to calculate reflections by creating a ray and let it get the
color from that point. we set a maximum bounce of 5 fto limit the amount of reflection ray produced.


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
Dharmil Yogesh Karia: reflection + shadows + refraction 


texture images: all texture images were from freepik.com
fur: https://www.freepik.com/free-photo/pale-fluffy-texture-pattern_1037605.htm
water:www.freepik.com/free-photo/abstract-pool-water-surface-background-with-sun-light-reflection_4566962.htm
rock: www.freepik.com/free-photo/warm-limestone-texture_1035015.htm
grass: https://www.freepik.com/free-photo/green-grass-textures_3987198.htm
moon:www.freepik.com/free-photo/black-white-details-moon-texture-concept_29662106.htm