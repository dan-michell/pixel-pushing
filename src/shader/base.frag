#version 330 core
out vec4 FragColor;
  
in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;

uniform float mixPercent;

void main()
{
    // 0.2 = 80% of the first inputs texture
    FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), mixPercent); 
}
