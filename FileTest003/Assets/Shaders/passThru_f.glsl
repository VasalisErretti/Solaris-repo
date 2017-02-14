/*
	Fragment Shader

	Sets the fragment colour

	//try not to do If statments in the shaders
*/

#version 330

in vec3 normal0;
in vec2 texCoord;
in vec3 color0;
in vec3 L_01;
in vec3 L_02;
//in vec3 eyePos;


uniform sampler2D tex1;
//uniform float dispNormals;

void main()
{
	vec4 out_Color;
	float shininess = 100.0; //make this a uniform
	vec3 N = normalize(normal0);

	vec4 textureColor = texture2D(tex1, texCoord.st);

	//Transparent
	if (textureColor.a < 0.7){ discard; }

	//diffuse conponent
	float lightIntensity = clamp(dot(normal0, L_01), 0.0, 1.0);
	vec4 diffuse = textureColor * lightIntensity;
	//ambient conponent
	vec4 ambient = textureColor * 0.05;
	//specular conponent
	//vec3 E = normalize(-eyePos);
	//vec3 H = normalize(L_1 + E);
	//float specularCoefficient = pow(max(dot(N, H), 0.0), shininess);
	//vec4 specular = textureColor * specularCoefficient;
	//Output
	out_Color = (diffuse + ambient);


	//diffuse conponent
	lightIntensity = clamp(dot(normal0, L_02), 0.0, 1.0);
	diffuse = textureColor * lightIntensity;
	//ambient conponent
	ambient = textureColor * 0.05;//specular conponent
	//E = normalize(-eyePos);
	//H = normalize(L_2 + E);
	//specularCoefficient = pow(max(dot(N, H), 0.0), shininess);
	//specular = textureColor * specularCoefficient;
	//Output
	out_Color += (diffuse + ambient);

	out_Color.w = 1.0;
	gl_FragColor = out_Color; //every frag shader must set gl_FragColor
}