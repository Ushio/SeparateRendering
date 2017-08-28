#version 120

uniform vec3 u_color;
varying vec3 normalVarying;

void main()
{
    gl_FragColor = vec4(normalVarying * u_color, 1.0);
}
