// rect.frag

#version 330 core

in vec2 TexCoords;
out vec4 color;

uniform vec4 in_color;
uniform float border_width;

void main() {
  float max_x = 1.0 - border_width;
  float min_x = border_width;
  float max_y = max_x;
  float min_y = min_x;

  if (TexCoords.x < max_x && TexCoords.x > min_x
      && TexCoords.y < max_y && TexCoords.y > min_y) {
    discard;
  }
  else {
    color = in_color;
  }
}
