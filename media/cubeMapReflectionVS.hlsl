int StyleUVW ; // 0 = specular reflection, 1 = diffuse reflection, 2 = use model vertex coordinates for uvw.
float4x4 WorldViewProj;
float4x4 World;
float3 CameraPos;

// Vertex Shader
void VS( 
	in float4 VPos	: POSITION,
	in float3 VNorm	: NORMAL,
	in float3 VTex  : TEXCOORD0,
	out float4 outPos	: POSITION,
	out float4 outTex	: TEXCOORD0 )
{
	// vertex position from model-space to view-space
    outPos = mul( VPos, WorldViewProj );
	
	if ( StyleUVW == 0 )
	{
		// create ray from camera position to the vertex, in world space
		float4 worldPos = mul(float4(VPos.x, VPos.y, VPos.z, 1.0), World);
		float3 view = CameraPos - worldPos.xyz;
		
		float4 normWorld = normalize(mul(float4(VNorm.x, VNorm.y, VNorm.z, 0.0), World));	// TODO: when objects are scaled non-uniform we need to multiply by WorldInverseTranspose instead
		
		// compute the reflection vector, and assign it to texcoord 0		
		outTex.xyz = reflect( -normalize(view), normWorld.xyz );
	}
	else if ( StyleUVW == 1 )
	{
		// just use the normal for the reflection vector
		outTex.xyz = normalize(VNorm);
	}
	else if ( StyleUVW == 2 )
	{
		// use vertex-coordinates for texture coordinates
		outTex.xyz = VPos.xyz;
	}
}
