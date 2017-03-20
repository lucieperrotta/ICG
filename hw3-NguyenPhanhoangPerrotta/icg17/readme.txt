Kim Lan Phan Hoang, Lucie Perrotta, Tim Nguyen

Introduction to Computer Graphics

Homework 3 - Shading

3.1 Phong Shading

Phong shading follows the same idea as the gouraud shading algorithm. The only difference is that the normal vector is computed for each pixel for the phong shading, rather than for each vector like in the gouraud shading one. Thus we stop the algorithm just before computing the normal_mv vector in the vshader file, and continue in the fshader one. 

3.2 Toon Shading

3.3 Artistic Shading

3.4 Flat Shading

Flat shading is based on triangles. Thus we need to compute the normal vector depdending on the triangle shape. For that, we use the functions dFdx and dFdy which will give us derivatives of the x and y axis. We then use theses results to compute the normal vector with a cross function. The remaining computations follow the phong shading one.

3.5 Spot Light Shading

For the spot light (per prixel) shading, we reused the same code as in the phong shading, excepted that if the pixel is located withen the range designed by the cirle of aperture angle spot_cos_cuttof, we light it according to the formula given in the paper. For the pixels outside this circle, only the ambient light is displayed. The spot_dir vector is mouse movable, as it is equal to the view_vector. By the way, we increased the spot_cos_cuttof and the spot_exp values so that it looks more like the given example.