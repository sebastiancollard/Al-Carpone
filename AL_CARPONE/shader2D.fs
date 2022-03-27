#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

vec4 color;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform vec3 textColor;

void main()
{    
	
   color = texture(texture_diffuse1, TexCoords);
   //if(color.r == 0.0f && color.g == 1.0f && color.b == 0.0f) return;
   if(color.a == 0.0f) discard;
  // color[3] = -1;
   //vec4 sampled = vec4(1.0, 1.0, 1.0, color.r);
   FragColor = color;//vec4(textColor, 1.0) * sampled;
 }