/* Attributes */

attribute vec3 inVertexPosition;
attribute vec3 inVertexNormal;
attribute vec4 inVertexColor;
attribute vec2 inTexCoord0;
attribute vec2 inTexCoord1;

/* Uniforms */

uniform mat4 uWVPMatrix;
uniform mat4 uWVMatrix;
uniform mat4 uNMatrix;

uniform vec4 uGlobalAmbient;
uniform vec4 uMaterialAmbient;
uniform vec4 uMaterialDiffuse;
uniform vec4 uMaterialEmissive;
uniform vec4 uMaterialSpecular;
uniform float uMaterialShininess;

uniform float uThickness;

/* Varyings */

varying vec2 vTextureCoord0;
varying vec4 vVertexColor;
varying vec4 vSpecularColor;
varying float vFogCoord;

void main()
{
	gl_Position = uWVPMatrix * vec4(inVertexPosition, 1.0);
	gl_PointSize = uThickness;

	vec3 Position = (uWVMatrix * vec4(inVertexPosition, 1.0)).xyz;
	vec3 P = normalize(Position);
	vec3 N = normalize(vec4(uNMatrix * vec4(inVertexNormal, 0.0)).xyz);
	vec3 R = reflect(P, N);

	float V = 2.0 * sqrt(R.x*R.x + R.y*R.y + (R.z+1.0)*(R.z+1.0));
	vTextureCoord0 = vec2(R.x/V + 0.5, R.y/V + 0.5);

	vVertexColor = inVertexColor.bgra;
	vSpecularColor = vec4(0.0, 0.0, 0.0, 0.0);

	vFogCoord = length(Position);
}
