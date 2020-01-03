
// Texture sampler
uniform sampler2D TextureSampler;

// TexCoords from vertex shader
varying vec2 TexCoords;

void main (void)
{
    // Texture is sampled at Texcoords using texture2D
    vec4 Color = texture2D(TextureSampler, TexCoords);
    
    // Inverse the color to produce negative image effect
    Color.rgb = 1.0 - Color.rgb;
    
    gl_FragColor = Color;
}