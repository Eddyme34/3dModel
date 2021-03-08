#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal_modelspace;

// Output data ; will be interpolated for each fragment.
out vec3 Ocolor;
out vec2 UV;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;
uniform mat4 MV;
uniform vec3 f;
uniform vec3 x;
uniform float IA;
uniform vec3 ka;
uniform float IL;
uniform vec3 kd;
uniform vec3 ks;
uniform int n;
void main(){
//phong model is calculated in the shaders
	gl_Position =  MVP * vec4(vertexPosition_modelspace,1);
	//changes position, normal, camera, and light to cameraspace
	vec3 vertexPosition_eyespace = normalize(vec3(MV * vec4(vertexPosition_modelspace, 1)));
	vec3 vertexNormal_eyespace = normalize(vec3(transpose(inverse(MV)) * vec4(vertexNormal_modelspace,1)));
	vec3 f_eyespace = normalize(vec3(MV * vec4(f, 1)));
	vec3 x_eyespace = normalize(vec3(MV * vec4(x, 1)));

	vec3 Normal_modelspace = normalize(vertexNormal_modelspace);
	//calculates view and light vector
	float Light = length(f_eyespace-vertexPosition_eyespace) + length(f_eyespace-vertexPosition_eyespace);
	vec3 Light_vector = (x_eyespace-vertexPosition_eyespace)/length(x_eyespace-vertexPosition_eyespace);
	vec3 View_vector = (f_eyespace-vertexPosition_eyespace)/length(f_eyespace-vertexPosition_eyespace);
	vec3 Iamb = ka * IA;
	float intense = IL/Light;
	vec3 Reflection_vector = reflect(Light_vector, vertexNormal_eyespace);
	UV = vertexUV;
	vec3 getColor = Normal_modelspace * 0.5f + 0.5f;
	//phong lighting calculations are done in the vertex shader so they can be interpolated in the rasterizer
	//this effectively emulating gourand shading
	Ocolor = Iamb + intense * ((kd * dot(Light_vector, vertexNormal_eyespace)) + (ks * pow(dot(Reflection_vector, View_vector), n))) + getColor;
}
