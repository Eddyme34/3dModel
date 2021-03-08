#version 330 core

// Interpolated values from the vertex shaders
in vec3 Ocolor;
in vec2 UV;
// Ouput data
out vec3 color;
void main(){
	//color = texture( myTextureSampler, UV ).rgb + Ocolor;
	color = Ocolor;
}
