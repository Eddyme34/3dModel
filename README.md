# 3dModel
Uses ImGui to make a 3d model
Algorithms Implemented
Phong Lighting Model(shading_vertex.glsl line 27): The vertex shader takes in front
position, light position, vertex position, IA, IL, ka, kd, ks, and Phong constant. I start off
by turning the vertex position, normal, front, and light position into cameraspace. I then
calculate the view, light vector, and distance between light source and vector. After this,
we have everything we need and only need to put them together to get the intensity.
The intensity is mixed with the color calculated from the normal.
Gouraud Shading(shading_vertex.glsl line 44): The Phong model is done in the
vertex shader. Once the intensity for each vertex is computed. It can be sent to the
rasterizer where linear interpolation occurs and the fragment shader receives the
interpolated values.
Half-Toning(main.cpp line 602): We first generate our frame buffer and resize it to ⅓
of our window. We then render our model to our frame buffer texture and now we can
half-tone to the actual screen. The second fragment shader(titled
shader_wobbly_texture.glsl) does the actual half-toning. It first grabs the fragment
coordinates and modules it in order to map its position on the megapixel. It then takes
the color of the fragment and uses it to calculate the intensity by multiplying max of rgb
with the number of mini pixels, which is 9.
Now we will determine if the current pixel should be turned on based on its position on
the mega-pixel and its intensity. If the pixel is on the right mega-pixel in an appropriate
intensity, it will turn on. If it is off, the fragment shader will color black.
Now that the pixel is on, we can now find what its color is. We first calculate the color
ratio. The color is determined by the ratio, position on the megapixel, and intensity.
Using the color ratio, I created a new vector of colors with the ratio. Red is the same as
the ratio, green is red ratio plus green ratio, and blue is the sum of all three ratios. This
is so I can keep track of which color on the megapixel is already colored.
I then check every intensity by incrementing to each intensity starting from 1.0. I
compared each color to the intensity and if it exceeds that intensity, that color will be
chosen.
Once we have the color it will be placed on the pixel.
Painter’s Algorithm(main.cpp line 306): Based on the chosen axis, each triangle will
be bubble sorted. This means we will continuously compare each triangle to the one in
front of it. If the current axis is xy, the sort will be determined by the minimum z depth of
each triangle. If the current axis is xz, the sort will be determined by the minimum y
depth of each triangle. If the current axis is yz, the sort will be determined by the
minimum x. depth of each triangle. After all triangles are sorted, the buffer will be
updated and we can render the objects.
Orthogonal projections(main.cpp line 30): Three cameras are defined. Each
representing an xy, xz, and yz axis using the provided camera functions and get
different view matrices. Through GUI interaction, the user can choose what axis to go to
and that will choose the view matrix used as the camera angle.
How to Use: On Windows
I ran the code by compiling it in a CMake Application using Visual Studio Compiler to
create the CMake Text file. Then created a launch file to input the text file on Visual
Studio Code.
Warning: My code has an issue where it cannot find the obj file. This seems to be
because of how it’s compiled. The same thing happened on the
d11_rendertotexture demo code. To fix this, I copied and pasted the obj file in the
same folder as where project three runs. You may not have this problem but I do.
The GUI is simple to understand because it’s all slider floats. The first is the general
ambient intensity IA and the second one is the general light intensity IL.
The next three: Ambientx, Ambienty, Ambientz, represents the ambient color coefficient
ka. The next three: Diffusex, Diffusey, Diffusez, represents the diffuse color coefficient
kd. The next three: Specularx, Speculary, Specularz, represents the specular color
coefficient ks.
The next three is the light position. The next three is the position of the front point. The
final slider represents the size of the light source.
The three button on the bottom is to switch the camera to xy, xz, and yz positions.
