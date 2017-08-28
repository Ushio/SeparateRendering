#version 120


varying vec3 normalVarying;

void main(void)
{
	gl_Position = ftransform();
	normalVarying = gl_Normal;
}
