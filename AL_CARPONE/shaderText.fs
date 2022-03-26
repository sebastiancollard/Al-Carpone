#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D text;
uniform vec3 textColor;

void main()
{    
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r); //Sample the colour value of the bitmap texture (stored in red component) This gives us alpha value
    color = vec4(textColor, 1.0) * sampled;                         //Resulting pixel is transparent for all glyph background pixels and non-transparent for actual character. GL_BLEND must be enabled.
}       