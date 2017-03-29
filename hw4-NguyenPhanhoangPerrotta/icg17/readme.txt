Kim Lan Phan Hoang, Lucie Perrotta, Tim Nguyen

Introduction to Computer Graphics

Homework 4 - Framebuffers

4.1 Fast Gaussian Blur

We duplicated the texture to gave 2 copies of it, as explained in the pdf, and then modified the uses of the texture in accordance to that edit. Then we wrote the kernel we would use in the f_shader, and edited the gaussian_convolution so that it processes only the x coordinate or the y coordinate, according to a parameter named "axis" (need only one for loop, use a kernel array instead of weight, ...). The right texture to use is also bind or unbind depending on the parameter "axis".

4.2 Screen Space Reflections

To display a reflected cube in the framebuffer, we need to mirror the cam_pos and the cam_up vectors by inversing their z values. Then we compute a new view_projection matrix using these mirrored vectors. At the end we can display our cube in the framebuffer (using bind and unbind functions around it). 

In the floor_fshader, we get the window height and width from the texture (using textureSize function), use gl_FragCoord to obtain the new coordinates u and v, normalized using window width/height. We can at the end display both the floor texture with the cube texture using the mix function. The v coordinate, once normalized, need to be flipped (1-v) to obtain the right rotation direction.

4.3 Motion Blur

Quad fshader:
We simple compute the difference of the 2 positions (already given in pixels) and we get a vector in pixels (still with 4 coordinates). Then we only take the x and y coord as we are working in 2 dimensions.

Screenquad fshader:
We first compute the color of the current pixel using the texture function. Then we set the size of the array to use. And then we get the velocity vector using texture are well but only taking the x and y coord since we just wanna use it as a 2d direction.

Now time for the actual formula given in the slides. In a loop (because of the sum) from 1 (we don't wanna count the original pixel twice) to N, we first compute the ith pixel's color using the same method as we did for comnputing the original pixel's color, i.e. the texture function (but shifted by the velocity*i). Then we add this color to a sum. And finally, out of the loop, we divide the total sum by N to get the average color.