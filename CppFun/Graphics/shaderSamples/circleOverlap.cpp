precision mediump float;
uniform vec2 u_resolution;
uniform float u_time;
void main() {
  // establishes uv-relative coordinate position for xy pixel.
  vec2 uv = gl_FragCoord.xy / u_resolution;
  // centers uv position?
  vec2 p = uv;
  // Signed distance to two circles (negative = inside)
  // position of each circle
  vec2 a = vec2(0.25, 0.5);
  vec2 b = vec2(0.5 + 0.2 * sin(u_time), 0.5);
  // (pixel position - circle center position) - radius offset
  float da = length(p - a) - 0.225;
  float db = length(p - b) - 0.125;
  // if da and db are less than 0, the pixel position is in both circles.
  float d = max(da, db);
  // step function step(a, b). returns 0 until b is greater than a.
  // effectively switches the color value
  vec3 color = mix(vec3(0.95, 0.55, 0.2), vec3(0.08), step(0.0, d));
  // Faint outlines of both original circles
  color += 1.0 * (1.0 - smoothstep(0.0, 0.01, abs(da)));
  color += 0.25 * (1.0 - smoothstep(0.0, 0.01, abs(db)));
  gl_FragColor = vec4(color, 1.0);
  // this is awesome. You can just compare lengths to get an overlap shader! I guess, just for circles
  // probably need a different approach for other shapes? a different general solution?
}

// Original code from https://www.shader-learn.com/playground
// 	https://www.shader-learn.com/glsl/max
