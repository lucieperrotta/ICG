Kim Lan Phan Hoang, Lucie Perrotta, Tim Nguyen

Introduction to Computer Graphics

Homework 2 - Perspective, Trackball and Animations

2.1 Perspective Projection

The function begins with a variable displayFactor which actually determines the sizes of the printed picture. We arbitrary set it to 5 so it looks nice and big enough. The display factor behaves like a resizing matrix, i.e. it multiplies the diagonal values by a scalar (5).
We then compute the 4 directions that are not argument already of the function, that is, left right top bottom. According to the webpage you gave us, left and right, respectively top and bottom, have to be of equal magnitude and opposite sign. Noe we just need to compute 1 out of 2 directions and get the other directly by changing the sign.
The top coordinate is first computed using fovy (the aperture angle) which tan will be multiplied by near, to get the actual height at the start distance and hence know how big in height our display will have to be. The left coordinate is then computed in order to respect the aspect ratio we get as an argument. By a easy cross-multiplication we get the computation from top to right : right = top*aspect.
Finally, we simply put the coordinates into the matrix as explained on the webpage http://www.songho.ca/opengl/files/gl_projectionmatrix_eq16.png , and we're good to go.

2.2 Trackball

2.3 Triangle Grid And Animation