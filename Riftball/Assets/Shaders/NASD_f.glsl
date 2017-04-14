
#version 330

uniform sampler2D tex1;
uniform sampler2D u_diffuseMap;
uniform sampler2D u_normalMap;
uniform sampler2D u_specularMap;
uniform vec4 u_lightPos;
uniform mat4 u_mv;
// Fragment Shader Inputs
in VertexData
{
	vec3 normal;
	vec3 texCoord;
	vec4 colour;
	vec3 eyePos;
} vIn;
layout(location = 0) out vec4 FragColor;

void main()
{
	FragColor = vec4(vIn.normal * 0.5 + 0.5, 1.0f);
	//textures
	vec4 diffuseTexture = texture(u_diffuseMap, vIn.texCoord.xy);
	vec4 normalTexture = texture(u_normalMap, vIn.texCoord.xy);
	vec4 specularTexture = texture(u_specularMap, vIn.texCoord.xy);
	//
	vec3 L = normalize(u_lightPos.xyz - vIn.eyePos);
	vec3 E = normalize(-vIn.eyePos);
	vec3 H = normalize(L + E);
	//normal
	vec3 N;
	if (true){
		N = normalize(vIn.normal);
	} else {
		N = normalTexture.xyz;
		N = ((N * 2.0) - 1.0);
		N = (u_mv * vec4(N, 0.0)).xyz;
		N = normalize(N);
	}
	//specular
	float specPower = 100.0;
	float specIntensity = 0.125;
	float NdotH = max(0.0, dot(N, H));
	float specularCoefficient = pow(NdotH, specPower);
	if (true){ specularTexture = vec4(1.0,1.0,1.0,1.0); }
	vec3 specular = specularCoefficient * specIntensity * specularTexture.xyz;
	//diffuse
	float NdotL = max(0.0, dot(N, L));
	float diffuseIntensity = clamp(dot(vIn.normal, L), 0.0, 1.0);
	if (true){ diffuseTexture = texture(tex1, vIn.texCoord.xy); }
	vec3 diffuse = NdotL * diffuseIntensity * diffuseTexture.xyz; 
	//ambient
	vec3 ambient = diffuseTexture.xyz * 0.2;
	//end colour
	FragColor = vec4(diffuse + specular + ambient, 1.0);
	FragColor.w = diffuseTexture.w;
	FragColor.a = diffuseTexture.a;
}