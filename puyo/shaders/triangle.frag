#version 330 core
uniform vec3 colorIN;
out vec4 color;

void main()
{    
    color = vec4(colorIN, 1.0);
}  