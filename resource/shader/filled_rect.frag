// filled_rect.frag

#version 330 core

in vec2 TexCoords;
out vec4 color;

uniform vec4 in_color;

void main() {
  color = in_color;
}
