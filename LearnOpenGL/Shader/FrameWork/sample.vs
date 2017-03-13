attribute vec3 pos;
attribute vec4 color;
attribute vec4 texcoord;

uniform mat4 M;
uniform mat4 P;
uniform mat4 V;

varying vec4 V_Color;
varying vec4 V_Texcoord;
void main()
{
	V_Color=color;
	V_Texcoord=texcoord;
	gl_Position=P*V*M*vec4(pos,1.0);
}