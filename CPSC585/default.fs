#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec4 FragPos;
in vec4 Normal;
in mat4 Transform;

#define NR_LIGHTS 16

uniform sampler2D texture_diffuse1;
uniform vec3 light_positions[NR_LIGHTS];

float ambient = 0.05f;

void main()
{    
   vec4 textureColor =  texture(texture_diffuse1, TexCoords);
 
  
   float illum = 0;
   for(int i = 0; i < NR_LIGHTS; i++){
		vec4 lPos = vec4(light_positions[i], 1.0f);
		vec4 lDir = FragPos - lPos;
		float d = length(lPos - FragPos);
		illum +=  5.0f * abs(dot(lDir, Normal)) / (d*d);
   }
   FragColor = min((ambient + illum), 1.0f) * textureColor;
}