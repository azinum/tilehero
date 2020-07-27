// sprite.frag

#version 330 core

in vec2 TexCoords;
out vec4 color;

uniform sampler2D image;
uniform vec3 sprite_color;

void main() {
	// color = vec4(sprite_color, 1.0);
	color = texture(image, TexCoords);
}
