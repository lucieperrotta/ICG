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
Finally, we simply put the coordinates into the matrix as explained on the webpage http://www.songho.ca/opengl/files/gl_projectionmatrix_eq16.png ,
and we're good to go.

2.2 Trackball

First we completed the ProjectOntoSurface() function by simply adding an if-else statement and using the formuly of a circle :
- if the position at the mouse cursor is INSIDE the trackball, that means, if (p.x)^2 + (p.y)^2 <= 1 (with p.x and p.y the coordinates of point p),
then we apply the corresponding formula given in the homework which is z(x,y) = sqrt(r^2 - x^2 + y^2).
- otherwise, the position is OUTSIDE the trackball, we apply a formula also found on this website : https://www.opengl.org/wiki/Object_Mouse_Trackball
corresponding to the hypersheet we should use and which formula is : r*r/2 / sqrt(x^2+x^2).

TODO3 is still not finished so not possible to describe it.

Then in the Init() function in main.cpp we just had to uncomment a line as said in the project and in MousePos(), we added as asked the computation of
trackball_matrix by using the function Drag() (trackback.Drag(...)) that we previously implemented, then we multiplied it by the previous state of the trackball
to get a new state for our trackball.

After that, we implemented the zooming. We first get the position of our mouse cursor and transformed it into screen coordinates
(we did the same way as in the previous implementation for the left click).
Then we change the value of our view_matrix by I DON'T KNOW WHY WE DID THIS XD

2.3 Triangle Grid And Animation

To create our grid of triangles, we surrounded the code that was already there with a double loop.
Our 'd' value corresponds to the length of a side of a triangle. 'x' and 'y' are the first lengths we will add on values -1.0 and 1.0 iterately,
and 'x1' and 'y1' are the next lengths (that's why we add 1.0 to them so it will be used to add a point just next to the ones created with 'x' and 'y').
We then filled the functions 'vertices_pushback(...). The first coordinate starts at -1.0 for the x-axis and at 1.0 for the y-axis.
We add at each iteration on the values described above, and then set the indices to take the points with this order :
the first one is on the upper left, the second on the upper right, the third on the lower left and the last one on the lower right (it creates a 'Z').
We also used a 'count' value to iterately go to the next indices, since we want to have a lot of triangles.

For the next part, in grid_vshader.glsl, we first set a few values :
- v : this is the constant 2*PI we need for our computation, since we use a sine function
- amplitude : determines the height of our bumps
- acc : this represents the accuracy, the bigger is this value, the more bumps we should have.

Then we compute the height as a sine function as asked. (A FINIR JE COMPRENDS PAS XD)