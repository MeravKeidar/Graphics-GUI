#version 330 core

in vec3 vPosition;
in vec3 vNormal;
in vec2 vTextureCoord; // Add texture coordinates input
in vec3 vFacePosition;
in vec3 vFaceNormal;

in vec4 vEmissive_color;
in vec4 vDiffuse_color;
in vec4 vSpecular_color;
in float vShininess_coefficient;

out vec4 vfragColor;
out vec2 TexCoord; // Output texture coordinates to fragment shader

uniform mat4 projection;
uniform mat4 modelview;
uniform mat4 normalMat;
uniform mat4 cameraMat;
uniform int nLights;
uniform vec4 ambient_color;
uniform sampler2D textureMap;

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
    vec4 view_pos = modelview * vec4(vPosition, 1.0);
    gl_Position = projection * view_pos;
    vec3 face_view_normal = normalize((normalMat * vec4(vFaceNormal, 0.0)).xyz);
    vec4 face_view_pos = modelview * vec4(vFacePosition, 1.0);

    // Pass texture coordinates to fragment shader
    TexCoord = vTextureCoord;

    //vec4 texColor = texture(textureMap, vTextureCoord);
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
}
