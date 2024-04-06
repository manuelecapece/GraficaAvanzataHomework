#version 330 core
out vec4 FragColor;
  
in vec2 TexCoord;

uniform sampler2D myTexture1;
uniform sampler2D myTexture2;

uniform vec3 colorcube;

void main()
{
    FragColor =  texture(myTexture1, TexCoord);//una texture + colore
}