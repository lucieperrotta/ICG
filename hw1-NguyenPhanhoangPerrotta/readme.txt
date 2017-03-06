Kim Lan Phan Hoang, Lucie Perrotta, Tim Nguyen

Introduction to Computer Graphics

Homework 1 - 2D ransformations and GLSL

1.1 Triangle Spirals

First of all, we began by initializing the different rotation, scaling and translation matrices as identity matrices in the Display() method.
We've created two functions, spiral(R,S,T) and fermat(RST) (just uncomment and comment the other function to see how it works) to switch between the two versions of the triangle spirals

-- Normal Spiral --
For the normal spiral, we created a for loop that iterates 60 times to create a pattern with multiple triangles.
Then, at each iteration, we apply a rotation with an angle of 0.3*i (where i = the i-th iteration) to create this gradual effect of triangles that rotate.
In addition to this, our triangles started with a size of 0.02 and at each iteration we scale them by additioning 0.001*i, which reveal this impression of triangle that grows gradually.
Finally, the effect of our translation matrix at each iteration adapts the position of each triangle to put them one after the other to give this spiral effect.

-- Fermat Spiral --
For the fermat spiral, we do the same by creating a for loop but this time we made it iterate 500 instead of 60 to draw lots of triangles
The fermat formula says that the correct angle to use is n*2.4 radians, so we just set it to an angle of 2.4*i 
For the scaling part, we just arbitrary choosed a constant nice looking size and set it to 0.02, which sized our triangles as wished.
After that, the translation matrix is a bit different than in the normal spiral:
the fermat formula says that the distance to the center of the spiral is a constant multiplied by the square root of our i-th triangle.
So we choosed an arbitrary value 0.03 and set our translation matrix with the values 0.03*sqrt(i) (i = i-th iteration as always)

1.2 Checkerboard

The checkerboard was the simplest one to implement, and here how we did it.


1.3 Solar System

This part of the homework took a several steps to achieve and was the hardest exercise.
First of all, we created our Quads and initialize them by setting the ".tga" images to each quad corresponding respectively to the moon, the earth and the sun.
Then, before beginning into the transformations part, we set a few constants like:
- time_secs : this one was already given and is used to measure the time that passes
- sun_angle : this represents the angle of rotation of the sun
- distanceToSun : this is the distance from the earth and the moon to the sun
- distanceToEarth : this represents the distance between the moon and the Earth
We also changed the size of the window so the display would be more pleasant.

-- The Sun --
For the sun we only had to make it rotate.