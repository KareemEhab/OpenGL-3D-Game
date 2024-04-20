#version 330 core

out vec4 fragColor;

in vec4 color;

void main()
{
	fragColor = vec4(1.0f, 0.2f, 0.6f, 1.0f);
	//fragColor = color;
}