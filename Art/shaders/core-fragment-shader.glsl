#version 410

in vec4 ya_cfs_color;
in vec2 ya_cfs_tex_position;

out vec4 ynk_FragColor;

uniform sampler2D yau_cfs_texture;

void main()
{
    // ynk_FragColor = vec4(ya_cfs_color);
    ynk_FragColor = texture(yau_cfs_texture, ya_cfs_tex_position);
}
