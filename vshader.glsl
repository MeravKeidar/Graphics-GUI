#version 330 core

in vec3 vPosition;
in vec3 vNormal;
in vec2 vTextureCoord;

in vec4 vEmissive_color;
in vec4 vDiffuse_color;
in vec4 vSpecular_color;
in float vShininess_coefficient;

out vec4 view_pos;
out vec4 view_normal;
out vec2 texCoordOut; 

out vec4 emissive_color;
out vec4 diffuse_color;
out vec4 specular_color;
out float shininess_coefficient;

uniform mat4 projection;
uniform mat4 modelview;
uniform mat4 normalMat;
uniform vec3 light_direction; 
uniform vec4 light_color; 
uniform float light_intensity; 

void main()
{
    view_pos = modelview * vec4(vPosition, 1.0);
    gl_Position = projection * view_pos;
    gl_Position /= gl_Position.w;
    view_normal = normalize(normalMat * vec4(vNormal, 0.0));
    texCoordOut = vTextureCoord;
    emissive_color = vEmissive_color;
    diffuse_color = vDiffuse_color;
    specular_color = vSpecular_color;
    shininess_coefficient = vShininess_coefficient;
}
