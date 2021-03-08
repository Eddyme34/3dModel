#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;

// Values that stay constant for the whole mesh.
uniform mat4 Matrix;

void main(){

	// Output position of the vertex, in clip space : Matrix * position
	gl_Position =  Matrix * vec4(vertexPosition_modelspace,1);

}