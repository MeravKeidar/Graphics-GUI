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


out vec4 vfragColor;
out vec2 vTexCoord;
out vec4 view_pos;
out vec3 oPosition;


uniform mat4 projection;
uniform mat4 modelview;
uniform mat4 normalMat;
uniform mat4 cameraMat;
uniform int nLights;
uniform vec4 ambient_color;
uniform sampler2D u_NormalMap;
uniform int use_normal_mapping; 
uniform samplerCube u_skybox; 
uniform int enviromental_mapping; 
uniform float time;
uniform int stretch_animation_x;
uniform int stretch_animation_y;
uniform int stretch_animation_z;
uniform int noisy_animation;

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

void main()
{
    vec3 pos = vPosition;
    oPosition = vPosition;
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
    vec3 face_view_normal = normalize((normalMat * vec4(vFaceNormal, 0.0)).xyz);
    mat3 TBN;
    if (use_normal_mapping == 1)
    {
        vec3 view_normal = normalize((normalMat * vec4(vNormal, 0.0)).xyz);
        vec3 T = normalize((normalMat * vec4(vTangent,0.0)).xyz);
        vec3 B = normalize(cross(view_normal, T));
        TBN = mat3(T, B, view_normal); 
        face_view_normal = TBN * (texture(u_NormalMap, vTexCoord).rgb * 2.0 -1.0);
    }

    vec4 face_view_pos = modelview * vec4(vFacePosition, 1.0);

    vTexCoord = vTextureCoord;

    vec4 color = vEmissive_color+ambient_color;
    

    for (int i = 0; i < nLights; i++)
    {
        vec3 l;
        if (lights[i].light_type == 0)
        {
            vec4 lightpos = (cameraMat * lights[i].position);
            l = normalize((lightpos - face_view_pos).xyz);
        }
        else if (lights[i].light_type == 1)
        {
            vec4 lightdir = (cameraMat * lights[i].direction);
            l = normalize(lightdir.xyz);
        }
        color += calcColor(l, face_view_normal, face_view_pos.xyz, lights[i].intensity, lights[i].color,vDiffuse_color,vSpecular_color,vShininess_coefficient);
    }
   
    vfragColor.x = min(color.x,1);
    vfragColor.y = min(color.y,1);
    vfragColor.z = min(color.z,1);
    vfragColor.w = 1;

    if (enviromental_mapping == 1)
   {
     vec3 R = reflect(normalize(face_view_pos.xyz), normalize(face_view_normal));
     vfragColor = vec4(texture(u_skybox, R).rgb, 1.0);
   }
}
