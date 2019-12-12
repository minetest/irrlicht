
// Pass to fragment shader with the same name
varying vec2 TexCoords;

void main(void)
{
    gl_Position = gl_Vertex;
    
    // The origin of the texture coordinates locates at bottom-left 
    // corner rather than top-left corner as defined on screen quad.
    // Instead of using texture coordinates passed in by OpenGL, we
    // calculate TexCoords based on vertex position as follows.
    //
    // Vertex[0] (-1, -1) to (0, 0)
    // Vertex[1] (-1,  1) to (0, 1)
    // Vertex[2] ( 1,  1) to (1, 1)
    // Vertex[3] ( 1, -1) to (1, 0)
    // 
    // Texture coordinate system in OpenGL operates differently from 
    // DirectX 3D. It is not necessary to offset TexCoords to texel 
    // center by adding 1.0 / TextureSize / 2.0
    
    TexCoords = (gl_Vertex.xy * 0.5 + 0.5);
}
