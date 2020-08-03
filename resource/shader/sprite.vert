// sprite.vert

#version 330 core

layout (location = 0) in vec4 vertex;

out vec2 uv;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main() {
  uv = vertex.zw;
  gl_Position = projection * view * model * vec4(vertex.xy, 0, 1.0);
}
