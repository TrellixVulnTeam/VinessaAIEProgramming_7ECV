#version 330 core
		in vec4 vColor;
		in vec2 UV;
		out vec4 outColour;
		uniform sampler2D diffuseTexture;
		void main () {
			outColour = texture(diffuseTexture, UV).rgba * vColor;
		}

