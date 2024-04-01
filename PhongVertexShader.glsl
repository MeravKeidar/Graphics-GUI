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
layout(location = 9) in vec3 vTangent;


uniform float time;
uniform int stretch_animation_x;
uniform int stretch_animation_y;
uniform int stretch_animation_z;
uniform int noisy_animation;
uniform mat4 projection;
uniform mat4 modelview;
uniform mat4 normalMat;

out vec4 view_pos;
out vec3 view_normal;
out vec2 vTexCoord;
out mat3 TBN;

out vec4 emissive_color;
out vec4 diffuse_color;
out vec4 specular_color;
out float shininess_coefficient;



void main()
{
    vec3 pos = vPosition;
    if (stretch_animation_x == 1)
    {
        pos.x = pos.x * 1.2 * sin(time);
    }
    if (stretch_animation_y == 1)
    {
        
        pos.y = pos.y * 1.2 * cos(time); 
    }
    if (stretch_animation_z == 1)
    {
        pos.y = pos.y * 1.2 * (cos(time) * sin(time)); 
    }
    if (noisy_animation == 1)
    {
      pos.x = pos.x + 0.25 * sin((2*pos.x + 3 * pos.y + pos.z) + time);
      pos.y = pos.y + 0.25 * sin((pos.x + pos.y + 3* pos.z) + time);
    }
   
    view_pos = modelview * vec4(pos , 1.0);
    gl_Position = projection * view_pos;
    view_normal = normalize((normalMat * vec4(vNormal, 0.0)).xyz);

    
    emissive_color = vEmissive_color; 
    diffuse_color = vDiffuse_color;
    specular_color = vSpecular_color;
    shininess_coefficient = vShininess_coefficient;

    vec3 T = normalize((normalMat * vec4(vTangent,0.0)).xyz);
    vec3 B = normalize(cross(view_normal, T));
    TBN = mat3(T, B, view_normal); 

   
    vTexCoord = vTextureCoord;
}
