#version 330 core

// uniform data
uniform mat4 uProjectionMatrix;
uniform mat4 uModelViewMatrix;

// viewspace data 
in VertexData {
	vec3 position;
	vec3 normal;
} f_in;

// framebuffer output
out vec4 fb_color;

void main() {
	// calculate shading
	vec3 surfaceColor = vec3(0.066, 0.341, 0.215);
	vec3 eye = normalize(-f_in.position); // direction towards the eye
	float light = abs(dot(normalize(f_in.normal), eye)); // difference between the surface normal and direction towards the eye
	vec3 finalColor = mix(surfaceColor / 4, surfaceColor, light);

	// output to the frambuffer
	fb_color = vec4(finalColor, 1);
}