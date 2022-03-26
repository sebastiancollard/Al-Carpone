#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
out vec4 color;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform vec3 textColor;

void main()
{    
	
   FragColor = texture(texture_diffuse1, TexCoords);
   FragColor[3] = -1;
   vec4 sampled = vec4(1.0, 1.0, 1.0, FragColor.r);
   color = vec4(textColor, 1.0) * sampled;
 }