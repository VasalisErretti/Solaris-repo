#version 400

uniform sampler2D u_tex;

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

vec3 boxBlur(sampler2D tex)
{
	
	vec2 uv = vIn.texCoord.xy;

	vec2 offsetCoordinates[9];

	//Top Row
	offsetCoordinates[0] = vec2(-u_texelSize.x,	u_texelSize.y) + uv;	//Left	//Top
	offsetCoordinates[1] = vec2( 0.0,			u_texelSize.y) + uv;	//Midd	//Top
	offsetCoordinates[2] = vec2( u_texelSize.x,	u_texelSize.y) + uv;	//Righ	//Top
	//Middle Row
	offsetCoordinates[3] = vec2(-u_texelSize.x,			  0.0) + uv;	//Left	//Mid
	offsetCoordinates[4] = vec2( 0.0,					  0.0) + uv;	//Midd	//Mid
	offsetCoordinates[5] = vec2( u_texelSize.x,			  0.0) + uv;	//Righ	//Mid
	//Bottom Row
	offsetCoordinates[6] = vec2(-u_texelSize.x,	-u_texelSize.y) + uv;	//Left	//Bot
	offsetCoordinates[7] = vec2( 0.0,			-u_texelSize.y) + uv;	//Midd	//Bot
	offsetCoordinates[8] = vec2( u_texelSize.x,	-u_texelSize.y) + uv;	//Righ	//Bot
	
	vec3 blurred = vec3(0.0);
	//Top Row
	blurred += texture(tex, offsetCoordinates[0]).rgb;	//Left	//Top
	blurred += texture(tex, offsetCoordinates[1]).rgb;	//Midd	//Top
	blurred += texture(tex, offsetCoordinates[2]).rgb;	//Righ	//Top
	//Middle Row
	blurred += texture(tex, offsetCoordinates[3]).rgb;	//Left	//Mid
	blurred += texture(tex, offsetCoordinates[4]).rgb;	//Midd	//Mid
	blurred += texture(tex, offsetCoordinates[5]).rgb;	//Righ	//Mid
	//Bottom Row
	blurred += texture(tex, offsetCoordinates[6]).rgb;	//Left	//Bot
	blurred += texture(tex, offsetCoordinates[7]).rgb;	//Midd	//Bot
	blurred += texture(tex, offsetCoordinates[8]).rgb;	//Righ	//Bot

	blurred = blurred*0.11111111;

	return blurred;
}

void main()
{
	vec3 blurred = boxBlur(u_tex);
	FragColor = vec4(blurred, 1.0);
}