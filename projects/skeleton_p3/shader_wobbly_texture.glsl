#version 330 core

in vec2 UV;
in vec4 gl_FragCoord;
out vec3 color;

uniform sampler2D renderedTexture;
  
//series of functions check if pixel should be turned on or off
bool check00(vec2 mega){
	if(mega.x == 0.0){
		if(mega.y == 0.0){
			return true;
		}
	}
	return false;
}
bool check10(vec2 mega){
	if(mega.x == 1.0){
		if(mega.y == 0.0){
			return true;
		}
	}
	return false;
}
bool check20(vec2 mega){
	if(mega.x == 2.0){
		if(mega.y == 0.0){
			return true;
		}
	}
	return false;
}
bool check01(vec2 mega){
	if(mega.x == 0.0){
		if(mega.y == 1.0){
			return true;
		}
	}
	return false;
}
bool check11(vec2 mega){
	if(mega.x == 1.0){
		if(mega.y == 1.0){
			return true;
		}
	}
	return false;
}
bool check21(vec2 mega){
	if(mega.x == 2.0){
		if(mega.y == 1.0){
			return true;
		}
	}
	return false;
}
bool check02(vec2 mega){
	if(mega.x == 0.0){
		if(mega.y == 2.0){
			return true;
		}
	}
	return false;
}
bool check12(vec2 mega){
	if(mega.x == 1.0){
		if(mega.y == 2.0){
			return true;
		}
	}
	return false;
}
bool check22(vec2 mega){
	if(mega.x == 2.0){
		if(mega.y == 2.0){
			return true;
		}
	}
	return false;
}
//returns true or false based on intensity and position on megapixel
bool on(vec2 mega, float intensity){
	if(intensity == 0.0){
		return false;
	}
	else if(intensity == 1.0){
		return check11(mega);
	}
	else if(intensity == 2.0){
		bool isitOn = check11(mega);
		if(isitOn == true){
			return true;
		}
		isitOn = check12(mega);
		if(isitOn == true){
			return true;
		}
		return false;
	}
	else if(intensity == 3.0){
		bool isitOn = check11(mega);
		if(isitOn == true){
			return true;
		}
		isitOn = check00(mega);
		if(isitOn == true){
			return true;
		}
		isitOn = check22(mega);
		if(isitOn == true){
			return true;
		}
		return false;
	}
	else if(intensity == 4.0){
		bool isitOn = check11(mega);
		if(isitOn == true){
			return true;
		}
		isitOn = check01(mega);
		if(isitOn == true){
			return true;
		}
		isitOn = check02(mega);
		if(isitOn == true){
			return true;
		}
		isitOn = check12(mega);
		if(isitOn == true){
			return true;
		}
		return false;
	}
	else if(intensity == 4.0){
		bool isitOn = check11(mega);
		if(isitOn == true){
			return true;
		}
		isitOn = check01(mega);
		if(isitOn == true){
			return true;
		}
		isitOn = check02(mega);
		if(isitOn == true){
			return true;
		}
		isitOn = check12(mega);
		if(isitOn == true){
			return true;
		}
		return false;
	}
	else if(intensity == 5.0){
		bool isitOn = check11(mega);
		if(isitOn == true){
			return true;
		}
		isitOn = check00(mega);
		if(isitOn == true){
			return true;
		}
		isitOn = check20(mega);
		if(isitOn == true){
			return true;
		}
		isitOn = check02(mega);
		if(isitOn == true){
			return true;
		}
		isitOn = check22(mega);
		if(isitOn == true){
			return true;
		}
		return false;
	}
	else if(intensity == 6.0){
		bool isitOn = check00(mega);
		if(isitOn == true){
			return true;
		}
		isitOn = check10(mega);
		if(isitOn == true){
			return true;
		}
		isitOn = check20(mega);
		if(isitOn == true){
			return true;
		}
		isitOn = check01(mega);
		if(isitOn == true){
			return true;
		}
		isitOn = check11(mega);
		if(isitOn == true){
			return true;
		}
		isitOn = check21(mega);
		if(isitOn == true){
			return true;
		}
		return false;
	}
	else if(intensity == 7.0){
		bool isitOn = check00(mega);
		if(isitOn == true){
			return true;
		}
		isitOn = check10(mega);
		if(isitOn == true){
			return true;
		}
		isitOn = check20(mega);
		if(isitOn == true){
			return true;
		}
		isitOn = check01(mega);
		if(isitOn == true){
			return true;
		}
		isitOn = check11(mega);
		if(isitOn == true){
			return true;
		}
		isitOn = check21(mega);
		if(isitOn == true){
			return true;
		}
		isitOn = check02(mega);
		if(isitOn == true){
			return true;
		}
		return false;
	}
	else if(intensity == 8.0){
		bool isitOn = check00(mega);
		if(isitOn == true){
			return true;
		}
		isitOn = check10(mega);
		if(isitOn == true){
			return true;
		}
		isitOn = check20(mega);
		if(isitOn == true){
			return true;
		}
		isitOn = check01(mega);
		if(isitOn == true){
			return true;
		}
		isitOn = check11(mega);
		if(isitOn == true){
			return true;
		}
		isitOn = check21(mega);
		if(isitOn == true){
			return true;
		}
		isitOn = check02(mega);
		if(isitOn == true){
			return true;
		}
		isitOn = check12(mega);
		if(isitOn == true){
			return true;
		}
		return false;
	}
	else{
		return true;
	}
}

vec3 colorHelper(vec3 color, float number){
	if(color.x >= number){
		return vec3(1,0,0);
	}
	else if(color.y >= number){
		return vec3(0,1,0);
	}
	else{
		return vec3(0,0,1);
	}
}
//calculates color based on intensity, position on megapixel, and color ratio
vec3 findColor(vec3 ratio, vec2 mega, float intensity){
	//new value for red, green, and blue is calculated to help determine if certain colors are already used
	float red = ratio.x;
	float green = ratio.x + ratio.y;
	float blue = ratio.x + ratio.y + ratio.z;
	vec3 color = vec3(red,green,blue);
	if(intensity == 1.0){
		return colorHelper(color, 1.0);
	}
	if(intensity == 2.0){
		if(mega == vec2(1.0, 1.0)){
			return colorHelper(color, 1.0);
		}
		else{
			return colorHelper(color, 2.0);
		}
	}
	if(intensity == 3.0){
		if(mega == vec2(0.0,0.0)){
			return colorHelper(color, 1.0);
		}
		if(mega == vec2(1.0, 1.0)){
			return colorHelper(color, 2.0);
		}
		else{
			return colorHelper(color, 3.0);
		}
	}
	if(intensity == 4.0){
		if(mega == vec2(0.0,1.0)){
			return colorHelper(color, 1.0);
		}
		if(mega == vec2(1.0, 1.0)){
			return colorHelper(color, 2.0);
		}
		if(mega == vec2(0.0, 2.0)){
			return colorHelper(color, 3.0);
		}
		else{
			return colorHelper(color, 4.0);
		}		
	}
	if(intensity == 5.0){
		if(mega == vec2(0.0,0.0)){
			return colorHelper(color, 1.0);
		}
		if(mega == vec2(2.0, 0.0)){
			return colorHelper(color, 2.0);
		}
		if(mega == vec2(1.0, 1.0)){
			return colorHelper(color, 3.0);
		}
		if(mega == vec2(0.0, 2.0)){
			return colorHelper(color, 4.0);
		}
		else{
			return colorHelper(color, 5.0);
		}		
	}
	if(intensity == 6.0){
		if(mega == vec2(0.0,0.0)){
			return colorHelper(color, 1.0);
		}
		if(mega == vec2(1.0, 0.0)){
			return colorHelper(color, 2.0);
		}
		if(mega == vec2(2.0, 0.0)){
			return colorHelper(color, 3.0);
		}
		if(mega == vec2(0.0, 1.0)){
			return colorHelper(color, 4.0);
		}
		if(mega == vec2(1.0, 1.0)){
			return colorHelper(color, 5.0);
		}
		else{
			return colorHelper(color, 6.0);
		}		
	}
	if(intensity == 7.0){
		if(mega == vec2(0.0,0.0)){
			return colorHelper(color, 1.0);
		}
		if(mega == vec2(1.0, 0.0)){
			return colorHelper(color, 2.0);
		}
		if(mega == vec2(2.0, 0.0)){
			return colorHelper(color, 3.0);
		}
		if(mega == vec2(0.0, 1.0)){
			return colorHelper(color, 4.0);
		}
		if(mega == vec2(1.0, 1.0)){
			return colorHelper(color, 5.0);
		}
		if(mega == vec2(2.0, 1.0)){
			return colorHelper(color, 6.0);
		}
		else{
			return colorHelper(color, 7.0);
		}		
	}
	if(intensity == 8.0){
		if(mega == vec2(0.0,0.0)){
			return colorHelper(color, 1.0);
		}
		if(mega == vec2(1.0, 0.0)){
			return colorHelper(color, 2.0);
		}
		if(mega == vec2(2.0, 0.0)){
			return colorHelper(color, 3.0);
		}
		if(mega == vec2(0.0, 1.0)){
			return colorHelper(color, 4.0);
		}
		if(mega == vec2(1.0, 1.0)){
			return colorHelper(color, 5.0);
		}
		if(mega == vec2(2.0, 1.0)){
			return colorHelper(color, 6.0);
		}
		if(mega == vec2(0.0, 2.0)){
			return colorHelper(color, 7.0);
		}
		else{
			return colorHelper(color, 8.0);
		}		
	}
	else{
		if(mega == vec2(0.0,0.0)){
			return colorHelper(color, 1.0);
		}
		if(mega == vec2(1.0, 0.0)){
			return colorHelper(color, 2.0);
		}
		if(mega == vec2(2.0, 0.0)){
			return colorHelper(color, 3.0);
		}
		if(mega == vec2(0.0, 1.0)){
			return colorHelper(color, 4.0);
		}
		if(mega == vec2(1.0, 1.0)){
			return colorHelper(color, 5.0);
		}
		if(mega == vec2(2.0, 1.0)){
			return colorHelper(color, 6.0);
		}
		if(mega == vec2(0.0, 2.0)){
			return colorHelper(color, 7.0);
		}
		if(mega == vec2(1.0, 2.0)){
			return colorHelper(color, 8.0);
		}
		else{
			return colorHelper(color, 9.0);
		}	
	}
}
void main() {

	vec2 position = gl_FragCoord.xy - 0.5; //gets position of fragment
	vec2 mega = mod(position, 3); //maps position to a mega-pixel
	vec3 initColor = texture(renderedTexture, UV).xyz;
	//calculates intensity
	float intensityXy = max(initColor.x, initColor.y);
	float intensity = round(9.0 * max(intensityXy, initColor.z));
	//checks if pixel should be turned on or off
	bool checkPixel = on(mega, intensity);
	if (checkPixel == false){
		color = vec3(0, 0, 0);
	}
	else{
		//calculates color ratio
		float red = round(intensity * (initColor.x/(initColor.x+initColor.y+initColor.z)));
		float green = round(intensity * (initColor.y/(initColor.x+initColor.y+initColor.z)));
		float blue = round(intensity * (initColor.z/(initColor.x+initColor.y+initColor.z)));
		vec3 ratio = vec3(red, green, blue);
		//uses the color ratio to find appropriate color for pixel
		color = findColor(ratio, mega, intensity);
	}

}
