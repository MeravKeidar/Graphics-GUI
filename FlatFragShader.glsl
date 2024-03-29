#version 330 core

in vec4 vfragColor;
in vec2 vTexCoord; 

out vec4 FragColor;

uniform sampler2D u_Texture; 
uniform int use_texture;


void main() 
{ 
  if (use_texture==1)
  {
	vec4 textureColor = texture(u_Texture, vTexCoord); 
	FragColor  = vfragColor * textureColor;
	}
	else
	{
	 FragColor  = vfragColor;
	}
}