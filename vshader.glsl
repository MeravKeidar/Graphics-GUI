#version 330 core

in vec3 position;
in vec3 normal;
in vec2 textureCoord;

in vec4 emissive_color;
in vec4 diffuse_color;
in vec4 specular_color;
in float shininess_coefficient;

out vec4 view_pos;
out vec4 view_normal;
out vec2 texCoordOut; 


uniform mat4 projection;
uniform mat4 modelview;
uniform mat4 normalMat;
uniform vec3 light_direction; 
uniform vec4 light_color; 
uniform float light_intensity; 

void main()
{
    view_pos = modelview * vec4(position, 1.0);
    view_normal = normalize(normalMat * vec4(normal, 0.0));
    gl_Position = projection * view_pos;
    texCoordOut = textureCoord;
    fragColor = vec4(1.0,1.0,1.0,1.0);
}
