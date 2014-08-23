#version 100

precision mediump float;

uniform sampler2D sampler0;

varying vec2 vuv;
varying vec3 vnormal;

vec3 l1 = normalize(vec3(0., 1., 2.));
vec4 c1 = vec4(0.2, 0.2, 0.6, 0.);

vec3 l2 = normalize(vec3(-1., 0., 0.));
vec4 c2 = vec4(0.8, 0.9, 0.8, 0.);

vec3 l3 = normalize(vec3(0., 0., -1.));
vec4 c3 = vec4(0.8, 0.6, 0.6, 0.);

void main()
{
    vec3 n = normalize(vnormal);
    vec4 albedo = texture2D(sampler0, vuv);
    float diffuse1 = clamp(dot(n, l1), 0.2, 1.);
    float diffuse2 = clamp(dot(n, l2), 0.2, 1.);
    float diffuse3 = clamp(dot(n, l3), 0.2, 1.);
    gl_FragColor = albedo*(diffuse1*c1 + diffuse2*c2 + diffuse3*c3);
}
