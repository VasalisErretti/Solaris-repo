
#version 330

uniform sampler2D tex1;
uniform vec4 u_lightPos;

// Fragment Shader Inputs
in VertexData
{
	vec3 normal;
	vec3 texCoord;
	vec4 colour;
	vec3 eyePos;
} vIn;

layout(location = 0) out vec4 FragColor; //FragColor response to GL_COLOR_ATTACHMENT0 [location = 0 so GL_COLOR_ATTACHMENT0]

void main()
{
	FragColor = vec4(vIn.normal * 0.5 + 0.5, 1.0f);
	//
	float shininess = 100.0; //make this a uniform
	vec3 N = normalize(vIn.normal);
	vec3 E = normalize(-vIn.eyePos);
	vec3 L_01 = normalize(u_lightPos.xyz - vIn.eyePos);
	vec3 L_02 = normalize((u_lightPos.xyz- vec3(0.0, 50.0, 0.0)) - vIn.eyePos);
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
	//Output 
	vec4 out_Color_01 = (diffuse + ambient + specular);


	//diffuse conponent
	lightIntensity = clamp(dot(vIn.normal, L_02), 0.0, 1.0);
	diffuse = textureColor * lightIntensity;
	//ambient conponent
	ambient = textureColor * 0.05;
	//specular conponent
	specularCoefficient = pow(max(dot(N, H_02), 0.0), shininess);
	specular = textureColor * specularCoefficient;
	//Output
	vec4 out_Color_02 = (diffuse + ambient + specular);



	FragColor = (out_Color_01 + out_Color_02)*0.5; FragColor.w = 1.0;

	FragColor = vec4(vIn.normal * 0.5 + 0.5, 1.0f);
	FragColor.a = textureColor.a;
}