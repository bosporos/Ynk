#version 410

layout (location = 0) in vec2 ya_cvs_position;
layout (location = 1) in vec4 ya_cvs_color;

out vec4 ya_cfs_color;

void main()
{
    gl_Position = vec4(ya_cvs_position, 0.0, 1.0);
    ya_cfs_color = ya_cvs_color;
}
