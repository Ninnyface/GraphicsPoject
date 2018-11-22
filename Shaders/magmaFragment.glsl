#version 150 core

uniform sampler2D diffuseTex ;
uniform samplerCube cubeTex ;

uniform vec4 lightColour ;
uniform vec3 lightPos ;
uniform vec3 cameraPos ;
uniform float lightRadius ;

in Vertex {
	vec4 colour ;
	vec2 texCoord ;
	vec3 normal ;
	vec3 worldPos ;
} IN ;

out vec4 fragColour ;

void main ( void ) {
	vec4 diffuse = texture(diffuseTex, IN.texCoord ) * IN.colour ;
	vec3 incident = normalize( IN.worldPos - cameraPos );
	float dist = length( lightPos - IN.worldPos );
	

	fragColour = ( lightColour * diffuse );
}