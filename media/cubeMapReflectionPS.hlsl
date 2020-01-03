sampler cubeTex: register(s0);
float Roughness;
	
float4 PS( float4 uvwTex : TEXCOORD0 ) : COLOR
{
	uvwTex.w = Roughness;
	//return texCUBEbias( cubeTex, uvwTex);
	return texCUBElod( cubeTex, uvwTex);
	//return texCUBE( cubeTex, uvwTex);
}
