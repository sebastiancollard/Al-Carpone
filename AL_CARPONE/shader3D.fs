#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec4 FragPos;
in vec3 Normal;
in mat4 Transform;
in vec3 crntPos;

#define MAX_NR_LIGHTS 128
#define MAX_NR_HEADLIGHTS 32

uniform sampler2D texture_diffuse1;
uniform int specLoaded;
uniform sampler2D texture_specular1;
uniform samplerCube skybox;

uniform int shaderMode;

uniform int is_police_headlight;


#define SHADER_MODE_FLAT 0
#define SHADER_MODE_DIFFUSE 1
#define SHADER_MODE_FULL 2

uniform int numLights;
uniform int numHeadlights;

uniform vec3 light_positions[MAX_NR_LIGHTS];
uniform vec3 headlight_positions[MAX_NR_HEADLIGHTS];
uniform vec3 headlight_directions[MAX_NR_HEADLIGHTS];




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

		float comp = - dot(vec4(Normal, 1.0),lDir);
		if(comp < 0) comp = 0;

		illum +=  20.f * comp / (pow(d,2.5));

		// specular lighting
		vec3 normal = normalize(Normal);
		float specularLight = 0.9f;
		vec3 lightDirection = normalize(light_positions[i] - crntPos);
		vec3 viewDirection = normalize(camPos - crntPos);
		vec3 reflectionDirection = reflect(-lightDirection, normal);
		float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 64);
		specular += specAmount * specularLight / (d*d);
   }
   
   vec4 headlightColor = vec4(1.0f,1.0f, 150.f / 255.0f, 0.0f);
   
   float headlightIllum = 0;
   float headlightSpec = 0;

   //if(comp < -0.9 && d < 1.0f) comp = -comp;

		//if(comp < 0) continue;

		//headlightIllum += 10 * comp  / pow(d,2);

   for(int i = 0; i < numHeadlights; i++){
// diffuse
		vec4 lPos = vec4(headlight_positions[i], 1.0f);
		vec4 lDir = FragPos - lPos;
		vec4 headlightForward = vec4(headlight_directions[i],1.0f);

		float comp = dot(normalize(lDir), headlightForward);

		if(comp < 0.8) continue;

		comp =  - dot(lDir, vec4(Normal, 1.0));

		float d = length(lPos - FragPos);

		//make the headlight source bright
		if(d < 0.5f && dot(headlightForward.xyz, Normal) >= 0.8 ){
			comp = abs(comp) * 0.08;
		}
		else if(is_police_headlight == 1){
			comp = abs(comp) * 0.75;
		}
		else if(comp < 0) continue;



		//headlightIllum += 10 * comp  / pow(d,2);

		
		
		headlightIllum +=  comp / pow(d,2);


		// specular lighting
		
		vec3 normal = normalize(Normal);
		float specularLight = 0.9f;
		vec3 lightDirection = normalize(light_positions[i] - crntPos);
		vec3 viewDirection = normalize(camPos - crntPos);
		vec3 reflectionDirection = reflect(-lightDirection, normal);
		float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 64);
		headlightSpec += comp * specAmount * specularLight / (d*d);
		 
	}
   

   if (shaderMode == SHADER_MODE_DIFFUSE) {
		FragColor = 
		vec4((ambient + illum) * textureColor.xyz, textureColor.w) + 
		headlightIllum * (headlightColor + textureColor);
   }

   if (shaderMode == SHADER_MODE_FULL) {
		vec4 textureSpec = texture(texture_specular1, TexCoords);
		if (textureSpec.w != 1.f) textureSpec.w = 1.f;
		FragColor =  
		vec4((ambient + illum) * textureColor.xyz, textureColor.w) + 
		
		vec4(texture(skybox, R).rgb, 0) * textureSpec * 0.5 + 
		textureSpec * specular * (illum + ambient) + 
		
		headlightIllum * (headlightColor + textureColor) +  
		headlightSpec * textureSpec * headlightIllum;
	}
  
}