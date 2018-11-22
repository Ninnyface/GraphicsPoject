# version 150 core

uniform samplerCube cubeTex ;
uniform vec3 cameraPos ;

in Vertex {
	vec3 normal ;
} IN ;

out vec4 fragColour ;

void main(void) {
	//vec4 temp = texture(cubeTex, normalize(IN.normal));
	fragColour = texture(cubeTex, normalize(IN.normal));
	//fragColour = vec4(1, 0 ,0, 1);
}