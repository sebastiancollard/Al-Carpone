#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec4 FragPos;
in vec4 Normal;
in mat4 Transform;
in vec3 crntPos;

#define NR_LIGHTS 16

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform vec3 light_positions[NR_LIGHTS];
uniform vec3 camPos;

float ambient = 0.05f;

void main()
{    
   vec4 textureColor =  texture(texture_diffuse1, TexCoords);
 
  
   float illum = 0;
   float specular = 0;
   for(int i = 0; i < NR_LIGHTS; i++){
		vec4 lPos = vec4(light_positions[i], 1.0f);
		vec4 lDir = FragPos - lPos;
		float d = length(lPos - FragPos);
		illum +=  5.0f * abs(dot(lDir, Normal)) / (d*d);

		// specular lighting
		vec3 normal = normalize(vec3(Normal.x, Normal.y, Normal.z));
		float specularLight = 0.90f;
		vec3 lightDirection = normalize(light_positions[i] - crntPos);
		vec3 viewDirection = normalize(camPos - crntPos);
		vec3 reflectionDirection = reflect(-lightDirection, normal);
		float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 64);
		specular += specAmount * specularLight;
   }

   

   FragColor = min((ambient + illum), 1.0f) * textureColor + texture(texture_specular1, TexCoords).r  * specular * min((ambient + illum), 1.0f);
}