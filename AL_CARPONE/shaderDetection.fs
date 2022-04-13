#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec4 FragPos;
in vec3 Normal;
in mat4 Transform;
in vec3 crntPos;


uniform sampler2D texture_diffuse1;
uniform float distance;
uniform float detectionRadius;

void main()
{    
   vec4 textureColor =  texture(texture_diffuse1, TexCoords);
 
   //float alpha = (distance < detectionRadius + 10.f ? (distance - detectionRadius) / (detectionRadius) : 1.);
   float alpha;
   if (distance < detectionRadius + 60) alpha = (detectionRadius + 60.f - distance) / 300.f;
   else discard;

   FragColor = vec4(textureColor.xyz, alpha);

}