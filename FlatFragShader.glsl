#version 330 core

in vec4 vfragColor;
in vec2 TexCoord; // Input texture coordinates

out vec4 FragColor;

uniform sampler2D ourTexture; // Texture sampler uniform

void main() 
{ 
    //vec4 textureColor = texture(ourTexture, TexCoord); 
    //FragColor  = vfragColor * textureColor;
    FragColor  = vfragColor;
}