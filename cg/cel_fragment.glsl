varying vec3 normal;
uniform vec4 light_position;
uniform sampler1D sample;

void main()
{
   float intensity = dot(light_position.xyz, normal);
   gl_FragColor = gl_Color * texture1D(sample, intensity);
}
