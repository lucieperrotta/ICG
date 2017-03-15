Kim Lan Phan Hoang, Lucie Perrotta, Tim Nguyen

Introduction to Computer Graphics

Homework 2 - Perspective, Trackball and Animations

2.1 Perspective Projection

The function begins with a variable displayFactor which actually determines the sizes of the printed picture.
We arbitrary set it to 5 so it looks nice and big enough. The display factor behaves like a resizing matrix, i.e.
it multiplies the diagonal values by a scalar (5).
We then compute the 4 directions that are not argument already of the function, that is, left right top bottom.

According to the webpage you gave us, left and right, respectively top and bottom, have to be of equal magnitude and opposite sign.
Noe we just need to compute 1 out of 2 directions and get the other directly by changing the sign.
The top coordinate is first computed using fovy (the aperture angle) which tan will be multiplied by near,
to get the actual height at the start distance and hence know how big in height our display will have to be.

The left coordinate is then computed in order to respect the aspect ratio we get as an argument.
By a easy cross-multiplication we get the computation from top to right : right = top*aspect.
Finally, we simply put the coordinates into the matrix as explained on the webpage http://www.songho.ca/opengl/files/gl_projectionmatrix_eq16.png ,and we're good to go.

2.2 Trackball

First we completed the ProjectOntoSurface() function by simply adding an if-else statement and using the formuly of a circle :
- if the position at the mouse cursor is INSIDE the trackball, that means, if (p.x)^2 + (p.y)^2 <= 1 (with p.x and p.y the coordinates of point p),
then we apply the corresponding formula given in the homework which is z(x,y) = sqrt(r^2 - x^2 + y^2).
- otherwise, the position is OUTSIDE the trackball, we apply a formula also found on this website : https://www.opengl.org/wiki/Object_Mouse_Trackball
corresponding to the hypersheet we should use and which formula is : r*r/2 / sqrt(x^2+x^2).

For the TODO 3, we simply use a rotation formula. We first compute k=a x b / |a x b| with a x b beeing the vec3 called vector in our code. The float angle is the formula for computing the angle between 2 vectors (the factor 5 is just here to offer a more comfortable interface for the user).

We then rotate an identity matrix with our angle and according to the vector to get the correct rotation matrix.

Finally we use the following formula : https://wikimedia.org/api/rest_v1/media/math/render/svg/9f2d7e8421cb977b8d1c7d8ee0966603478849f4 to compute the rotation matrix according to our parameters.

Then in the Init() function in main.cpp we just had to uncomment a line as said in the project and in MousePos(), we added as asked the computation of
trackball_matrix by using the function Drag() (trackback.Drag(...)) that we previously implemented, then we multiplied it by the previous state of the trackball
to get a new state for our trackball.

For the TODO 4 we simply get the Y position of the cursor and similarily to the code given in the mouseButton function, we set the view_matrix accordingly (to the Z coordinate). Up is further, down is nearer.

2.3 Triangle Grid And Animation

To create our grid of triangles, we surrounded the code that was already there with a double loop.
Our 'd' value corresponds to the length of a side of a triangle. 'x' and 'y' are the first lengths we will add on values -1.0 and 1.0 iterately,
and 'x1' and 'y1' are the next lengths (that's why we add 1.0 to them so it will be used to add a point just next to the ones created with 'x' and 'y').
We then filled the functions 'vertices_pushback(...). The first coordinate starts at -1.0 for the x-axis and at 1.0 for the y-axis.
We add at each iteration on the values described above, and then set the indices to take the points with this order :
the first one is on the upper left, the second on the upper right, the third on the lower left and the last one on the lower right (it creates a 'Z').
We also used a 'count' value to iterately go to the next indices, since we want to have a lot of triangles.
A little trick had to be add to avoid intermediate lines between rows by adding twice the last vertex and one more time the first vertex of the next line.

For the next part, in grid_vshader.glsl, we first set a few values :
- v : reprensents the frequency of the sinus function. Higher is the value, more bumps there will be.
- amplitude : determines the height of our bumps
- acc : this represents the acceleration

Then we compute the waves. We use:
- The time to make it moves, at speed acc
- sinus or cosinus functions to obtain waves, which move up and down more or less depending on the amplitude
- the coordinates of each point of the grid

A lot of combinations exist to obtain waves. 

2.4 Water animation

Here we compute it so that it looks like water. We use a small amplitude, and a sinus + cosinus functions with small frequencies so that waves are bigger. We also change the color using the same function as for the height in the vshader file, defining that light blue are p while darker blue are at the bottom of the waves. 