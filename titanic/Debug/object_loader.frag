// Minimal fragment shader

#version 420

in vec4 fcolour;
in vec2 ftexcoord;
out vec4 outputColor;
uniform sampler2D tex1, tex2;
uniform uint terrain, texturemode;
void main()
{
	if(terrain == 0)
	{
		if(texturemode == 0)
		{
			vec4 texture_colour = texture(tex1, ftexcoord);
			outputColor = fcolour * texture_colour;
		}
		else
		{
			vec4 texture_colour = texture(tex1, ftexcoord);
			outputColor = fcolour * texture_colour;
		}
		//outputColor = fcolour * texture_colour;
	}
	else
	{
		outputColor = fcolour;
	}
}