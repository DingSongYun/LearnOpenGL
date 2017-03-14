varying vec4 V_Color;
varying vec4 V_Texcoord;

uniform sampler2D U_MainTexture;

void main()
{
	gl_FragColor=V_Color*texture2D(U_MainTexture,V_Texcoord.xy);
}