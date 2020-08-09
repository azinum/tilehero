// rect.frag

#version 330 core

in vec2 TexCoords;
out vec4 color;

uniform vec4 in_color;
uniform float thickness;
uniform vec2 rect_size;

void main() {
	if ((TexCoords.x <= thickness || TexCoords.x >= 1.0 - thickness) ||
		(TexCoords.y <= thickness * (rect_size.x / rect_size.y) || TexCoords.y >= 1.0 - thickness * (rect_size.x / rect_size.y))) {
		color = in_color;
	}
	else {
		discard;
	}
}
