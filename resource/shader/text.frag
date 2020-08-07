// text.frag

#version 330 core

in vec2 uv;
out vec4 color;

uniform sampler2D image;
uniform vec2 uv_offset;
uniform vec2 uv_range;
uniform vec4 tint;

void main() {
	color = texture(image, (uv * uv_range) + uv_offset);
	if (color.r == 0 && color.g == 0 && color.b == 0) {
    discard;
	}
  else {
    color *= tint;
  }
}
