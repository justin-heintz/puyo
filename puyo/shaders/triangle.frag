#version 330 core
in vec2 TexCoord;

uniform vec3 colorIN;
uniform sampler2D ourTexture;

out vec4 material;

void main()
{    
    material = texture(ourTexture, TexCoord)  ;
}  