uniform int StyleUVW ; // 0 = specular reflection, 1 = diffuse reflection, 2 = use model vertex coordinates for uvw.
uniform vec3  CameraPos;
uniform mat4 World;

void main(void)
{
	gl_Position = ftransform();	// same as gl_ModelViewProjectionMatrix * gl_Vertex;
	
    // compute the reflection vector, and assign it to texcoord 0
	if ( StyleUVW == 0 )
	{
		vec4 worldPos = World*gl_Vertex;
		vec3 viewNormal = normalize(worldPos.xyz - CameraPos);	// view vector
	
		gl_TexCoord[0] = vec4( reflect( viewNormal, normalize(gl_Normal) ), 1.0 );
	}
	else if ( StyleUVW == 1 )
	{
		// just use the normal for the reflection vector
		gl_TexCoord[0] = vec4(normalize(gl_Normal), 1.0);
	}
	else if ( StyleUVW == 2 )
	{
		// use vertex-coordinates for texture coordinates
		gl_TexCoord[0] = normalize(gl_Vertex);
	}	
}
