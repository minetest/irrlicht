#version 130
uniform samplerCube cubeTex;
uniform float Roughness;

void main( void )
{
//	gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
	vec3 uvw = vec3(gl_TexCoord[0]);
	//gl_FragColor = textureCube( cubeTex, uvw );
	gl_FragColor = textureLod( cubeTex, uvw, Roughness );
	//gl_FragColor = textureCube( cubeTex, uvw, Roughness );
}

