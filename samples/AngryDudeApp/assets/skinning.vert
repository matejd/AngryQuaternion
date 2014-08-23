#version 100

uniform mat4 mvp;
uniform mat4 boneMatrices[60];
uniform vec4 boneDualQuaternions[120];

uniform bool useDQB;

attribute vec3 position;
attribute vec3 normal;
attribute vec4 bones;
attribute vec2 uv;

varying vec2 vuv;
varying vec3 vnormal;

float bsign(float value)
{
    if (value < 0.0)
        return -1.0;
    return 1.0;
}

void DQB()
{
    vec4 real0 = boneDualQuaternions[int(bones.x)*2    ];
    vec4 dual0 = boneDualQuaternions[int(bones.x)*2 + 1];
    vec4 b0 = real0 * fract(bones.x);
    vec4 be = dual0 * fract(bones.x);

    vec4 real = boneDualQuaternions[int(bones.y)*2    ];
    vec4 dual = boneDualQuaternions[int(bones.y)*2 + 1];
    b0 += real * fract(bones.y) * bsign(dot(real, real0));
    be += dual * fract(bones.y) * bsign(dot(real, real0));

    real = boneDualQuaternions[int(bones.z)*2    ];
    dual = boneDualQuaternions[int(bones.z)*2 + 1];
    b0 += real * fract(bones.z) * bsign(dot(real, real0));
    be += dual * fract(bones.z) * bsign(dot(real, real0));

    real = boneDualQuaternions[int(bones.w)*2    ];
    dual = boneDualQuaternions[int(bones.w)*2 + 1];
    b0 += real * fract(bones.w) * bsign(dot(real, real0));
    be += dual * fract(bones.w) * bsign(dot(real, real0));

    vec4 c0 = b0 / sqrt(dot(b0, b0));
    vec4 ce = be / sqrt(dot(b0, b0));

    // Fast version (from Geometric Skinning with Approximate Dual Quaternion Blending [Kavan et al]).
    // Bypassing dual quaternion-to-matrix conversion.
    vec3 p = position;
    float a = c0.w;
    float b = ce.w;
    vec3 r = c0.xyz;
    vec3 t = ce.xyz;
    vnormal = normal + 2.0 * cross(r, cross(r, normal) + a*normal);
    gl_Position = mvp * vec4(p + 2.0 * cross(r, cross(r, p) + a*p)
                               + 2.0 * (a*t - b*r + cross(r, t)), 1.0);
}

void main()
{
    vuv = uv;
    if (useDQB) {
        DQB();
    }
    else {
        mat4 transform = boneMatrices[int(bones.x)] * fract(bones.x) +
                         boneMatrices[int(bones.y)] * fract(bones.y) +
                         boneMatrices[int(bones.z)] * fract(bones.z) +
                         boneMatrices[int(bones.w)] * fract(bones.w);
        vnormal = (transform * vec4(normal, 0.0)).xyz;
        gl_Position = mvp * transform * vec4(position, 1.0);
    }
}
