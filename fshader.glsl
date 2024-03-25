#version 330 core

in vec4 view_pos;
in vec4 view_normal;
in vec2 texCoord;

in vec4 emissive_color;
in vec4 diffuse_color;
in vec4 specular_color;
in float shininess_coefficient;

out vec4 fColor;

void main() 
{ 
   fColor = view_pos;
} 
