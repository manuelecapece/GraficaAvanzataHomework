#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;

void main()
{
     
    vec3 color = texture(texture_diffuse1, TexCoords).rgb;

    BrightColor = vec4(0.0, 0.0, 0.0, 1.0);

    FragColor = vec4(color, 1.0);
}