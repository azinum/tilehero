// text.frag

#version 330 core

in vec2 uv;
out vec4 color;

uniform sampler2D image;
uniform vec2 uv_offset;
uniform vec2 uv_range;
uniform vec3 tint;

void main() {
  // if (gl_FragCoord.x >= clip.x &&
  //     gl_FragCoord.x <= clip.z &&
  //     gl_FragCoord.y >= clip.y &&
  //     gl_FragCoord.y <= clip.w) {
  // }
  // else {
  //   discard;
  // }

  color = texture(image, (uv * uv_range) + uv_offset);
  if (color.r == 0 && color.g == 0 && color.b == 0) {
    discard;
    return;
  }
  color *= vec4(tint.xyz, 1);
}
