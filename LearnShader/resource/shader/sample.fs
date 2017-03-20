varying vec2 V_Texcoord;
varying vec4 V_Color;

uniform sampler2D U_MainTexture;

void main()
{
	gl_FragColor= V_Color * texture2D(U_MainTexture,V_Texcoord);
}