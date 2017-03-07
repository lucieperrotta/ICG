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

The checkerboard was the simplest one to implement, and here how we did it. As mention in the slides, we mostly use a sinus function to obtain a variation between -1 and 1 and only modified the fshader.glsl file.
To draw enough squares on the checkerboard it is necessary to use a high period for the sinus function. So we apply the sinus function to each component of the uv coordinate times a period 5 pi. We then multiply the two sinus times 0.5 and add 0.5 to map it correctly all results in the range [0, 1] (as the texture function needs to).


1.3 Solar System

This part of the homework took a several steps to achieve and was the hardest exercise.
First of all, we created our Quads and initialize them by setting the ".tga" images to each quad corresponding respectively to the moon, the earth and the sun.
Then, before beginning into the transformations part, we set a few constants like:
- time_secs : this one was already given and is used to measure the time that passes
- SUN_SPEED : this is the rotation speed of the Sun on itself.
- SUN_ROTATION : this is the rotation speed of the Earth around the Sun.
- EARTH_SPEED : this is the rotation speed of the Earth on itself.
- EARTH_ROTATION : this is the rotation speed of the Moon around the Earth.
- MOON_SPEED :  this is the rotation speed of the Moon on itself.
- DISTANCE_SUN : this is the distance from the earth and the moon to the sun
- DISTANCE_EARTH : this represents the distance between the moon and the Earth
- ACCELERATION : this is an acceleration constant which makes the simulation go faster.
We also changed the size of the window so the display would be more pleasant.

We can use only one matrix for all transformation for one object. The difficulty was to compute transformations into the right order (since translation*rotation doesn't provide the same result as rotation*translation). 

-- The Sun --
We only had to make it rotate on the z axis, by SUN_SPEED * time_secs * ACCELERATION.

-- The Earth --
The Earth is first rotated on the z axis, by SUN_ROTATION * time_secs * ACCELERATION. This represents the rotation around the Sun. It is then translated at a distance DISTANCE_SUN with a translation matrix. Once done, we can make it rotate on itself, on the z axis, by EARTH_SPEED * time_secs * ACCELERATION. It is finally scaled to make it little than the sun.

-- The Moon --
The Moon is rotated by SUN_ROTATION*time_secs*ACCELERATION on the z axis, which represents the rotation around the sun. It is then translated by a distance DISTANCE_SUN and is rotated on the z axis, by EARTH_ROTATION * time_secs * ACCELERATION, which represents the rotation around the Earth. It is then translated by a distance DISTANCE_EARTH and make it rotate on itself by MOON_SPEED * time_secs * ACCELERATION. It is finally scaled to make it smaller than both Sun and Earth.