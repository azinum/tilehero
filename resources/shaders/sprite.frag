// sprite.frag

#version 330 core

in vec2 uv;
out vec4 color;

uniform sampler2D image;
uniform vec2 uv_offset; // x1, y1
uniform vec2 uv_range;  // x2, y2

void main() {
	color = texture(image, (uv * uv_range) + uv_offset);
}
