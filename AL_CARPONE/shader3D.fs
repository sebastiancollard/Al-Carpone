#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec4 FragPos;
in vec3 Normal;
in mat4 Transform;
in vec3 crntPos;

#define MAX_NR_LIGHTS 128

uniform sampler2D texture_diffuse1;
uniform int specLoaded;
uniform sampler2D texture_specular1;
uniform samplerCube skybox;

uniform int shaderMode;



#define SHADER_MODE_FLAT 0
#define SHADER_MODE_DIFFUSE 1
#define SHADER_MODE_FULL 2

uniform int numLights;

uniform vec3 light_positions[MAX_NR_LIGHTS];
uniform vec3 camPos;


float ambient = 0.15f;


void main()
{    
   vec4 textureColor =  texture(texture_diffuse1, TexCoords);
 
  

   if(shaderMode == SHADER_MODE_FLAT){
		  FragColor = textureColor;
		  return;
   }
  
   float illum = 0;
   float specular = 0;

   vec3 I = normalize(FragPos.xyz - camPos);
   vec3 R = reflect(I, normalize(Normal));


   //Get light contributions
    for(int i = 0; i < numLights; i++){
		// diffuse
		vec4 lPos = vec4(light_positions[i], 1.0f);
		vec4 lDir = FragPos - lPos;
		float d = length(lPos - FragPos);
		illum +=  2.5f * abs(dot(lDir, vec4(Normal, 1.0))) / (d*d);

		// specular lighting
		vec3 normal = normalize(Normal);
		float specularLight = 0.9f;
		vec3 lightDirection = normalize(light_positions[i] - crntPos);
		vec3 viewDirection = normalize(camPos - crntPos);
		vec3 reflectionDirection = reflect(-lightDirection, normal);
		float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 64);
		specular += specAmount * specularLight / (d*d);
   }

   if (shaderMode == SHADER_MODE_DIFFUSE) {
		FragColor = min((ambient + illum), 1.0f) * textureColor;
   }

   if (shaderMode == SHADER_MODE_FULL) {
		vec4 textureSpec = texture(texture_specular1, TexCoords);
		FragColor = min((ambient + illum), 1.0f) * textureColor + vec4(texture(skybox, R).rgb, 1.0) * textureSpec * 0.5f + textureSpec * specular * min(illum + ambient, 1.0f);
   }
  
}