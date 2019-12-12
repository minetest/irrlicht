// Part of the Irrlicht Engine Shader example.
// Simple GLSL vertex shader
// Please note that these example shaders don't do anything really useful.
// They only demonstrate that shaders can be used in Irrlicht.


uniform mat4 mWorldViewProj;
uniform mat4 mInvWorld;
uniform mat4 mTransWorld;
uniform vec3 mLightPos;	// actually just camera-pos in this case
uniform vec4 mLightColor;

void main(void)
{
	gl_Position = mWorldViewProj * gl_Vertex;
	
	// transform normal somehow (NOTE: for the real vertex normal you would use an inverse-transpose world matrix instead of mInvWorld)
	vec4 normal = vec4(gl_Normal, 0.0);
	normal = mInvWorld * normal;
	normal = normalize(normal);
	
	// (NOTE: not sure why transposed world is used instead of world?)
	vec4 worldpos = gl_Vertex * mTransWorld;
	
	vec4 lightVector = worldpos - vec4(mLightPos,1.0);
	lightVector = normalize(lightVector);
	
	float tmp2 = dot(-lightVector, normal);
	
	vec4 tmp = mLightColor * tmp2;
	gl_FrontColor = gl_BackColor = vec4(tmp.x, tmp.y, tmp.z, 0.0);
	
	gl_TexCoord[0] = gl_MultiTexCoord0;
}
