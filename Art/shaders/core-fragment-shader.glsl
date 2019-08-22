#version 410

in vec4 ya_cfs_color;

out vec4 ynk_FragColor;

void main()
{
    ynk_FragColor = vec4(ya_cfs_color);
}
