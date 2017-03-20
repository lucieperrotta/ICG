Kim Lan Phan Hoang, Lucie Perrotta, Tim Nguyen

Introduction to Computer Graphics

Homework 3 - Shading

3.1 Phong Shading

Phong shading follows the same idea as the gouraud shading algorithm.
The only difference is that the normal vector is computed for each pixel for the phong shading,
rather than for each vector like in the gouraud shading one.
Thus we stop the algorithm just before computing the normal_mv vector in the vshader file,
and continue in the fshader one. 

3.2 Toon Shading
For the Toon shading, we first needed to bind the texture in 1D in mesh.h for the toon_program_id. 
Then for the mapping part, when we computed the diffuse part, instead of multiplying by the Lambert expression (which is n dot l),
we multiply by texture(tex1D, lambert).x, which do our wished mapping.
We add '.x' because we wanted the mapping to be only on one axis, that means the x-axis.
For the toon_vshader.glsl, we kept the same code as in the Phong exercise, as indicated.

3.3 Artistic Shading
We began like the Toon Shader, by binding the texture in 2D with the art_program_id.
After that, we have this time to map our texture in two dimensions.
We computed our Lambert expression and our 'r dot v to the power of alpha' like usual.
Then we used them to create a 2D vector, vec2(lambert, r_v_alpha), and mapped them to the sampler2D 'tex2D'
(same procedure as in the Toon Shader).
For the art_vshader.glsl, we kept the same code as in the Phong exercise, as indicated.

3.4 Flat Shading

Flat shading is based on triangles. Thus we need to compute the normal vector depdending on the triangle shape. For that, we use the functions dFdx and dFdy which will give us derivatives of the x and y axis. We then use theses results to compute the normal vector with a cross function. The remaining computations follow the phong shading one.

3.5 Spot Light Shading

For the spot light (per prixel) shading, we reused the same code as in the phong shading, excepted that if the pixel is located withen the range designed by the cirle of aperture angle spot_cos_cuttof, we light it according to the formula given in the paper. For the pixels outside this circle, only the ambient light is displayed. The spot_dir vector is mouse movable, as it is equal to the view_vector. By the way, we increased the spot_cos_cuttof and the spot_exp values so that it looks more like the given example.