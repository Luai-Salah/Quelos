#type vertex
#version 450

layout(std140, binding = 0) uniform Camera { mat4 u_ViewProjection; };
layout(std140, binding = 1) uniform Transform { mat4 u_Transform; };

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_TexCoords;

layout (location = 0) out vec3 v_Normal;
layout (location = 1) out vec3 v_FragPosition;
layout (location = 2) out vec2 v_TexCoords;

void main()
{
	gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
	v_FragPosition = vec3(u_Transform * vec4(a_Position, 1.0));
	v_Normal = mat3(transpose(inverse(u_Transform))) * a_Normal;
    v_TexCoords = a_TexCoords;
}

#type pixel
#version 450

layout (binding = 0) uniform sampler2D u_Textures[32];
layout(std140, binding = 2) uniform ViewPosition { vec3 u_ViewPosition; };

layout(std140, binding = 3) uniform MaterialUB
{
     vec4 Ambient;
     float Shininess;
} u_Material;

layout(std140, binding = 4) uniform LightUB
{
     //vec4 Position;
     vec4 Direction;
     
     vec4 Ambient;
     vec4 Diffuse;
     vec4 Specular;
} u_Light;

layout (location = 0) in vec3 v_Normal;
layout (location = 1) in vec3 v_FragPosition;
layout (location = 2) in vec2 v_TexCoords;

layout(location = 0) out vec4 o_Color;

void main()
{
	// Ambient
    vec3 ambient = u_Light.Ambient.xyz * u_Material.Ambient.xyz * vec3(texture(u_Textures[0], v_TexCoords));
    
    // Diffuse
    vec3 norm = normalize(v_Normal);
    //vec3 lightDir = normalize(u_Light.Position.xyz - v_FragPosition);
    vec3 lightDir = normalize(-u_Light.Direction.xyz);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = u_Light.Diffuse.xyz * diff * vec3(texture(u_Textures[0], v_TexCoords));
    
    // Specular
    vec3 viewDir = normalize(u_ViewPosition - v_FragPosition);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = u_Light.Specular.xyz * spec * vec3(texture(u_Textures[1], v_TexCoords));
    
    vec3 result = ambient + diffuse + specular;
    o_Color = vec4(result, 1.0f);
}