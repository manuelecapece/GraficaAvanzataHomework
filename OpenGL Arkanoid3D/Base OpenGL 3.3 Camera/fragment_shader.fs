#version 330 core
out vec4 FragColor;
  
in vec2 TexCoord;

uniform sampler2D myTexture1;
uniform sampler2D myTexture2;

uniform vec3 colorcube;

void main()
{
    //FragColor = mix(texture(myTexture1, TexCoord), texture(myTexture2, TexCoord), 0.6) * vec4(colorcube, 1.0);//due texture mischiate + colore
    FragColor =  texture(myTexture1, TexCoord) * vec4(colorcube, 1.0);//una texture + colore
}