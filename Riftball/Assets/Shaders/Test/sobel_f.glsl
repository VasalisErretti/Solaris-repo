#version 400

uniform sampler2D u_rgb;
uniform sampler2D u_normals;
uniform vec4 u_texelSize; // (1.0 / windowWidth, 1.0 / windowHeight)

// Fragment Shader Inputs
in VertexData
{
	vec3 normal;
	vec3 texCoord;
	vec4 colour;
	vec3 posEye;
} vIn;

layout(location = 0) out vec4 FragColor;

float horizontalSobel(sampler2D tex) // returns gx
{
	vec2 uv = vIn.texCoord.xy;

	vec2 offsetCoordinates[6];

	//Top Row
	offsetCoordinates[0] = vec2(-u_texelSize.x,	u_texelSize.y) + uv;	//Left	//Top
	offsetCoordinates[1] = vec2( 0.0,			u_texelSize.y) + uv;	//Midd	//Top
	offsetCoordinates[2] = vec2( u_texelSize.x,	u_texelSize.y) + uv;	//Righ	//Top
	//Bottom Row
	offsetCoordinates[3] = vec2(-u_texelSize.x,	-u_texelSize.y) + uv;	//Left	//Bot
	offsetCoordinates[4] = vec2( 0.0,			-u_texelSize.y) + uv;	//Midd	//Bot
	offsetCoordinates[5] = vec2( u_texelSize.x,	-u_texelSize.y) + uv;	//Righ	//Bot

	float sobel[6];
	//Top Row
	sobel[0] = -1.0;	//Left	//Top
	sobel[1] = -2.0;	//Midd	//Top
	sobel[2] = -1.0;	//Righ	//Top 
	//Bottom Row
	sobel[3] =  1.0;	//Left	//Bot
	sobel[4] =  2.0;	//Midd	//Bot
	sobel[5] =  1.0;	//Righ	//Bot
	
	vec3 gx = vec3(0.0);
	gx += texture(tex, offsetCoordinates[0]).rgb * sobel[0];
	gx += texture(tex, offsetCoordinates[1]).rgb * sobel[1];
	gx += texture(tex, offsetCoordinates[2]).rgb * sobel[2];
	gx += texture(tex, offsetCoordinates[3]).rgb * sobel[3];
	gx += texture(tex, offsetCoordinates[4]).rgb * sobel[4];
	gx += texture(tex, offsetCoordinates[5]).rgb * sobel[5];

	float sqrMag = dot(gx, gx);

	if (sqrMag > 0.5){ return 0.0; } //distance threshold
	else { return 1.0; }
}

float verticalSobel(sampler2D tex) // returns gy
{
	vec2 uv = vIn.texCoord.xy;

	vec2 offsetCoordinates[6];

	//Left Col
	offsetCoordinates[0] = vec2(-u_texelSize.x,	 u_texelSize.y) + uv;	//Top	/Left
	offsetCoordinates[1] = vec2(-u_texelSize.x,			   0.0) + uv;	//Mid	/Left
	offsetCoordinates[2] = vec2(-u_texelSize.x,	-u_texelSize.y) + uv;	//Bot	/Left
	//Right Col
	offsetCoordinates[3] = vec2( u_texelSize.x,	 u_texelSize.y) + uv;	//Top	//Righ
	offsetCoordinates[4] = vec2( u_texelSize.x,			   0.0) + uv;	//Mid	//Righ
	offsetCoordinates[5] = vec2( u_texelSize.x,	-u_texelSize.y) + uv;	//Bot	//Righ

	float sobel[6];
	//Top Row
	sobel[0] = -1.0;	//Top	/Left
	sobel[1] = -2.0;	//Mid	/Left
	sobel[2] = -1.0;	//Bot	/Left 
	//Bottom Row
	sobel[3] =  1.0;	//Top	//Righ
	sobel[4] =  2.0;	//Mid	//Righ
	sobel[5] =  1.0;	//Bot	//Righ
	
	vec3 gy = vec3(0.0);
	gy += texture(tex, offsetCoordinates[0]).rgb * sobel[0];
	gy += texture(tex, offsetCoordinates[1]).rgb * sobel[1];
	gy += texture(tex, offsetCoordinates[2]).rgb * sobel[2];
	gy += texture(tex, offsetCoordinates[3]).rgb * sobel[3];
	gy += texture(tex, offsetCoordinates[4]).rgb * sobel[4];
	gy += texture(tex, offsetCoordinates[5]).rgb * sobel[5];

	float sqrMag = dot(gy, gy);

	if (sqrMag > 0.5){ return 0.0; } //distance threshold
	else { return 1.0; }
}

void main()
{
	float sobel = horizontalSobel(u_normals) * verticalSobel(u_normals);
	vec2 uv = vIn.texCoord.xy;
	FragColor.xyz = texture(u_rgb, uv).xyz * sobel;
	FragColor.a = 1.0;
}