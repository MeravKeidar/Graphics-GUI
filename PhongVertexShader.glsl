#version 330 core

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTextureCoord;
layout(location = 3) in vec3 vFacePosition;
layout(location = 4) in vec3 vFaceNormal;
layout(location = 5) in vec4 vEmissive_color;
layout(location = 6) in vec4 vDiffuse_color;
layout(location = 7) in vec4 vSpecular_color;
layout(location = 8) in float vShininess_coefficient;


uniform mat4 projection;
uniform mat4 modelview;
uniform mat4 normalMat;

out vec4 view_pos;
out vec3 view_normal;
out vec2 vTexCoord;

out vec4 emissive_color;
out vec4 diffuse_color;
out vec4 specular_color;
out float shininess_coefficient;


void main()
{
    view_pos = modelview * vec4(vPosition, 1.0);
    gl_Position = projection * view_pos;
    view_normal = normalize((normalMat * vec4(vNormal, 0.0)).xyz);

    emissive_color = vEmissive_color; 
    diffuse_color = vDiffuse_color;
    specular_color = vSpecular_color;
    shininess_coefficient = vShininess_coefficient;

    vTexCoord = vTextureCoord;
}
