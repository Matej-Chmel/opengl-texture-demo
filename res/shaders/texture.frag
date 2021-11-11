#version 460 core

in vec2 fTexCoord;
out vec4 outColor;
uniform sampler2D texUnit;

void main() {
	outColor = texture(texUnit, fTexCoord);
}
