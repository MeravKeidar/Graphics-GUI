#version 330 core

in vec4 view_pos;
in vec3 view_normal;
in vec2 texCoord;

in vec4 emissive_color;
in vec4 diffuse_color;
in vec4 specular_color;
in float shininess_coefficient;

uniform mat4 cameraMat;
uniform int nLights;
uniform vec4 ambient_color;

struct Light {
    vec4 color;
    float intensity;
    vec4 direction;
    int light_type;
    vec4 position;
};

uniform Light lights[15];


vec4 calcColor(vec3 lightDir, vec3 normal, vec3 pos, float light_intensity, vec4 light_color, vec4 diffuse_color, vec4 specular_color, float shininess_coefficient)
{
    float LN = max(0.0, dot(lightDir, normal));
    vec3 r = normalize((2*normal*LN)-lightDir);
    vec3 v = normalize(-pos);

    vec4 diffuse = light_color * (diffuse_color * (LN * light_intensity));

    float Shininess = pow(max(dot(r, v), 0.0), shininess_coefficient);
    vec4 specular = light_color * (specular_color * Shininess * light_intensity);
    
    return diffuse + specular;
}

out vec4 FragColor;

void main() 
{ 
    vec4 color = emissive_color+ambient_color;
    
    for (int i = 0; i < nLights; i++)
    {
        vec3 l;
        if (lights[i].light_type == 0)
        {
            vec4 lightpos = (cameraMat * lights[i].position);
            l = normalize((lightpos - view_pos).xyz);
        }
        else if (lights[i].light_type == 1)
        {
            vec4 lightdir = (cameraMat * lights[i].direction);
            l = normalize(lightdir.xyz);
        }
        color += calcColor(l, view_normal, view_pos.xyz, lights[i].intensity, lights[i].color,diffuse_color,specular_color,shininess_coefficient);
    }
    color.x = min(color.x,1);
    color.y = min(color.y,1);
    color.z = min(color.z,1);
    color.w = 1;
    FragColor  = color;
} 
