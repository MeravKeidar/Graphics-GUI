#version 330


in  vec3 vPosition;
in  vec3 vNormal;

out vec4 vfragColor;

uniform mat4 projection;
uniform mat4 modelview;
uniform mat4 normalMat;
uniform float normal_scale;
uniform vec4 color;

void main()
{
    vec4 view_pos = modelview * vec4(vPosition, 1.0);
    vec4 view_normal = normalMat * vec4(vNormal, 0.0);

    gl_Position = projection * (view_pos+ normal_scale*view_normal);
    vfragColor = color;
}