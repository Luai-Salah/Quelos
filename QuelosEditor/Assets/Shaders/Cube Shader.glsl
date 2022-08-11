#type vertex
#version 450

layout (location = 0) in vec3 a_Position;

layout(std140, binding = 0) uniform Camera
{
	mat4 u_ViewProjection;
};

layout(std140, binding = 1) uniform Transform
{
	mat4 u_Transform;
};

void main()
{
	gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
}

#type fragment
#version 450

layout(std140, binding = 2) uniform ObjectColor
{
	vec4 u_ObjectColor;
};

layout(location = 0) out vec4 o_Color;

void main()
{
	o_Color = u_ObjectColor;
}