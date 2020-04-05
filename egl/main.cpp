#include "Wayland.h"
#include <iostream>

Wayland::Window window;

const char* vertexShaderCode = R"(
#version 300 es
precision mediump float;

out vec4 color;

void main()
{
	float x = float(((uint(gl_VertexID) + 2u) / 3u) % 2u);
	float y = float(((uint(gl_VertexID) + 1u) / 3u) % 2u);

	gl_Position = vec4(-1.0+x*2.0,1.0-y*2.0,0.0,1.0);
	color       = vec4(     x    ,    y    ,0.0,1.0);
}
)";

const char* pixelShaderCode = R"(
#version 300 es
precision mediump float;

in vec4 color;
out vec4 FragColor;

void main()
{
	FragColor = color;
}
)";

int main()
{
	Wayland::Window::SetupStatics();

	window.Init(320, 200, 0, 0);
	
	glClearColor(0.0, 0.0, 0.0, 1.0);

	auto vertexShader = glCreateShader(GL_VERTEX_SHADER);
	auto pixelShader = glCreateShader(GL_FRAGMENT_SHADER);

	int  success;
	char infoLog[512];

	glShaderSource(vertexShader, 1, &vertexShaderCode, NULL);
	glShaderSource(pixelShader, 1, &pixelShaderCode, NULL);

	glCompileShader(vertexShader);
	glCompileShader(pixelShader);

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if(!success)
	{
	    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
	    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog;
	}

	glGetShaderiv(pixelShader, GL_COMPILE_STATUS, &success);
	if(!success)
	{
	    glGetShaderInfoLog(pixelShader, 512, NULL, infoLog);
	    std::cout << "ERROR::SHADER::PIXEL::COMPILATION_FAILED\n" << infoLog;
	}

	auto program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, pixelShader);
	glLinkProgram(program);
	glUseProgram(program);

	glGetShaderiv(program, GL_LINK_STATUS, &success);
	if(!success)
	{
	    glGetShaderInfoLog(program, 512, NULL, infoLog);
	    std::cout << "ERROR::PROGRAM::LINK::COMPILATION_FAILED\n" << infoLog;
	}

	while(window.IsOpen())
	{
		window.Poll();
		glClear(GL_COLOR_BUFFER_BIT);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
}