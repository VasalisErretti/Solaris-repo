
#version 330

uniform sampler2D tex1;
uniform vec4 u_lightPos_01;
uniform vec4 u_lightPos_02;

uniform mat4 localTransform;
uniform mat4 mvm; // modelview matrix

// Fragment Shader Inputs
in VertexData
{
	vec3 normal;
	vec2 texCoord;
	vec4 colour;
	vec3 eyePos;
	vec3 vertex;
} vIn;

layout(location = 0) out vec4 FragColor; //FragColor response to GL_COLOR_ATTACHMENT0 [location = 0 so GL_COLOR_ATTACHMENT0]

void main()
{
	FragColor = vec4(vIn.normal * 0.5 + 0.5, 1.0f);

	//
	float shininess = 10.0; //make this a uniform
	vec3 N = normalize(vIn.normal);
	vec3 E = normalize(-vIn.eyePos);
	//vec3 L_01 = normalize(u_lightPos_01.xyz - vIn.eyePos);
	//vec3 L_02 = normalize(u_lightPos_02.xyz - vIn.eyePos);
	vec4 modelViewSpace = mvm * (vec4(localTransform * vec4(vIn.vertex, 1.0)));
	vec3 L_01 = normalize(u_lightPos_01.xyz - modelViewSpace.xyz);
	vec3 L_02 = normalize(u_lightPos_02.xyz - modelViewSpace.xyz);
	vec3 H_01 = normalize(L_01 + E);
	vec3 H_02 = normalize(L_02 + E);
	vec4 textureColor = texture2D(tex1, vIn.texCoord.st);

	//Transparent
	//if (textureColor.a < 0.7){ discard; }

	//diffuse conponent
	float lightIntensity = clamp(dot(vIn.normal, L_01), 0.0, 1.0);
	vec4 diffuse = textureColor * lightIntensity; diffuse.w = 1.0f;
	//ambient conponent
	vec4 ambient = textureColor * 0.05; ambient.w = 1.0f;
	//specular conponent
	float specularCoefficient = pow(max(dot(N, H_01), 0.0), shininess);
	vec4 specular = textureColor * specularCoefficient; specular.w = 1.0f;
	//Output // + specular
	vec4 out_Color_01 = (diffuse + ambient);


	//diffuse conponent
	lightIntensity = clamp(dot(vIn.normal, L_02), 0.0, 1.0);
	diffuse = textureColor * lightIntensity; diffuse.w = 1.0f;
	//ambient conponent
	ambient = textureColor * 0.05; ambient.w = 1.0f;
	//specular conponent
	specularCoefficient = pow(max(dot(N, H_02), 0.0), shininess);
	specular = textureColor * specularCoefficient;
	//Output // + specular
	vec4 out_Color_02 = (diffuse + ambient);



	FragColor = (out_Color_01 + out_Color_02)*0.50;
	FragColor.w = 1.0;
	if (textureColor.a < 0.9){ FragColor.a = (textureColor.a*0.5); }
	if (textureColor.a < 0.3){ discard; }
	
	
}