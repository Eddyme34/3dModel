#version 330 core

in vec2 UV;

out vec3 color;

uniform sampler2D renderedTexture;
uniform sampler2D depthTexture;
uniform float time;

uniform float near;
uniform float far;

uniform bool render_depth;
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main() {

	if (!render_depth) 
	{
		color = texture(renderedTexture, UV + 0.005 * vec2(sin(time + 1024.0 * UV.x), cos(time + 768.0 * UV.y))).xyz;
	}
	else {
		float d = texture(depthTexture, UV).x;
		color = vec3(LinearizeDepth(d));
	}

}
