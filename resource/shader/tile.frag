// tile.frag

#version 330 core

in vec2 uv;
in vec2 uv_offset;
out vec4 color;

uniform sampler2D image;
uniform vec2 texture_size;
uniform vec2 tile_size;

void main() {
  vec2 uv_range = vec2(tile_size.x / texture_size.x, tile_size.y / texture_size.y);
  color = texture(image, (uv * uv_range) + uv_offset);
  // color = texture(image, (uv * uv_range) + vec2(uv_offset.x / texture_size.x, uv_offset.y / texture_size.y));
  if (color.a < 0.1) {
    discard;
  }
}
