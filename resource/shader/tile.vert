// tile.vert

#version 330 core

layout (location = 0) in vec4 vertex;
layout (location = 1) in vec4 tile_data;

out vec2 uv;
out vec2 uv_offset;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main() {
  uv = vertex.zw;
  uv_offset = tile_data.zw;
  gl_Position = projection * view * model * vec4(vertex.xy + tile_data.xy, 0, 1.0);
}
