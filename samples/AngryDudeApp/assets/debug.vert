#version 100

uniform mat4 mvp;
uniform mat4 boneMatrices[60];

attribute vec4 positionBone;

void main()
{
    vec4 animatedPosition = boneMatrices[int(positionBone.w)] * vec4(positionBone.xyz, 1.0);
    gl_Position = mvp * animatedPosition;
}
