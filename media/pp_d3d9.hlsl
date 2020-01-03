
// Texture size to calculate texel center
float2 TextureSize;

// Vertex shader output struct
struct VS_OUTPUT
{
    float4 Position  : POSITION0;
    float2 TexCoords : TEXCOORD0;
};

VS_OUTPUT vertexMain(
    float4 Position  : POSITION0, 
    float2 TexCoords : TEXCOORD0
    )
{
    VS_OUTPUT OUT;
    OUT.Position = Position;
    
    // In practice, instead of passing in TextureSize,
    // pass 1.0 / TextureSize for better performance
    
    // TexCoords is set to the texel center
    OUT.TexCoords = TexCoords + 1.0 / TextureSize / 2.0;
    
    return OUT;
}

// Texture sampler
sampler2D TextureSampler;

float4 pixelMain ( float2 Texcoords : TEXCOORD0 ) : COLOR0
{
    // Texture is sampled at Texcoords using tex2D
    float4 Color = tex2D(TextureSampler, Texcoords);
    
    // Change Texcoords to sample pixels around
    
    // Inverse the color to produce negative image effect
    Color.rgb = 1.0 - Color.rgb;
    
    return Color;
};
