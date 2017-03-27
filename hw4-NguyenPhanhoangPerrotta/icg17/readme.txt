Kim Lan Phan Hoang, Lucie Perrotta, Tim Nguyen

Introduction to Computer Graphics

Homework 4 - Framebuffers

4.1 Fast Gaussian Blur



4.2 Screen Space Reflections

To display a reflected cube in the framebuffer, we need to mirror the cam_pos and the cam_up vectors by inversing their z values. Then we compute a new view_projection matrix using these mirrored vectors. At the end we can display our cube in the framebuffer (using bind and unbind functions around it). 

In the floor_fshader, we get the window height and width from the texture (using textureSize function), use gl_FragCoord to obtain the new coordinates u and v, normalized using window width/height. We can at the end display both the floor texture with the cube texture using the mix function (THE V COORDINATE IS FLIPPED USING....).

4.3 Motion Blur

We compute the velocity vector by substracting the previous position from the current one. But before that, we transform the vectors into the screen-space by multiplying them with their respective MVP inverted matrices. Then we only keep the x and y axis. (!!! A CHECKER)

For the ... fshader, we compute all colors we will mix together depending on a constant N parameter and the velocity previously computed. 
