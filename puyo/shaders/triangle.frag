#version 330 core
in vec2 TexCoord;

uniform vec3 colorIN;
uniform sampler2D ourTexture;

out vec4 material;

void main()
{    
    vec4 texColor = texture(ourTexture, TexCoord);
    if(texColor.r == 1.0 && texColor.g == 0.0 && texColor.b >= 0.941 ){
        discard;
    }
    if(texColor.r == 0.0 && texColor.g == 0.0 && texColor.b == 1.0 ){
       texColor =  vec4(colorIN,1);
    } 
    material = texColor;
}  