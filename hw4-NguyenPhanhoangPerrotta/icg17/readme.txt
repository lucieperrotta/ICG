Kim Lan Phan Hoang, Lucie Perrotta, Tim Nguyen

Introduction to Computer Graphics

Homework 4 - Framebuffers

4.1 Fast Gaussian Blur



4.2 Screen Space Reflections

To display a reflected cube in the framebuffer, we need to mirror the cam_pos and the cam_up vectors by inversing their z values. Then we compute a new view_projection matrix using these mirrored vectors. At the end we can display our cube in the framebuffer (using bind and unbind functions around it). 

In the floor_fshader, we get the window height and width from the texture (using textureSize function), use gl_FragCoord to obtain the new coordinates u and v, normalized using window width/height. We can at the end display both the floor texture with the cube texture using the mix function (THE V COORDINATE IS FLIPPED USING....).

4.3 Motion Blur

