#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <SOIL/SOIL.h>
#include <stdexcept>
#include <streambuf>
#include <string>
namespace fs = std::filesystem;

constexpr GLfloat plain[]{
	1.f, 0.f, 1.f, 0.f, 1.f, 0.f, 0.f, 0.f,
	1.f, 0.f, -1.f, 0.f, 1.f, 0.f, 1.f, 0.f,
	-1.f, 0.f, -1.f, 0.f, 1.f, 0.f, 1.f, 1.f,
	-1.f, 0.f, 1.f, 0.f, 1.f, 0.f, 0.f, 1.f,
	1.f, 0.f, 1.f, 0.f, 1.f, 0.f, 0.f, 0.f,
	-1.f, 0.f, -1.f, 0.f, 1.f, 0.f, 1.f, 1.f
};

constexpr GLfloat triangle[]{
	-.8f, -.5f, 0.f, 0.f, 1.f, 1.f, .5f, .5f,
	.8f, -.5f, 0.f, 0.f, .5f, .8f, 1.f, 1.f,
	0.f, .5f, 0.f, 0.f, .3f, .7f, .0f, .5f
};

static void checkLinkStatus(const GLuint prog) {
	GLint status;
	glGetProgramiv(prog, GL_LINK_STATUS, &status);

	if(status == GL_FALSE) {
		GLint infoLogLength;
		glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &infoLogLength);
		auto infoLog = new GLchar[infoLogLength + 1];
		glGetProgramInfoLog(prog, infoLogLength, nullptr, infoLog);
		std::cerr << "Linker failure\n\n" << infoLog;
		delete[] infoLog;
		std::cin.get();
	}
}

static fs::path resPath(const fs::path& p) {
	static const auto RES_DIR = fs::path(RES);
	return RES_DIR / p;
}

static GLuint shader(const fs::path& p) {
	const auto ext = p.extension().string();
	const auto fullPath = resPath("shaders" / p);
	GLenum type;

	if(ext == ".frag")
		type = GL_FRAGMENT_SHADER;
	else if(ext == ".vert")
		type = GL_VERTEX_SHADER;
	else
		throw std::runtime_error("Invalid shader extension " + ext);

	std::ifstream s(fullPath);
	const auto content = std::string(
		std::istreambuf_iterator(s),
		std::istreambuf_iterator<char>()
	);
	const auto data = content.c_str();

	const auto ID = glCreateShader(type);
	glShaderSource(ID, 1, &data, nullptr);
	glCompileShader(ID);
	return ID;
}

int main() {
	(void)glfwInit();

	auto window = glfwCreateWindow(800, 600, "Texture demo", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	(void)glewInit();

	const auto frag = shader("texture.frag");
	const auto prog = glCreateProgram();
	const auto vert = shader("texture.vert");

	glAttachShader(prog, frag);
	glAttachShader(prog, vert);
	glLinkProgram(prog);
	checkLinkStatus(prog);
	glDetachShader(prog, frag);
	glDetachShader(prog, vert);
	glDeleteShader(frag);
	glDeleteShader(vert);

	GLuint VAO, VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));

	GLuint texture = SOIL_load_OGL_texture(resPath(fs::path("textures") / "wood.png").string().c_str(), SOIL_LOAD_RGBA, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glUseProgram(prog);
	GLint texUnit = glGetUniformLocation(prog, "texUnit");
	glUniform1i(texUnit, 0);

	glViewport(0, 0, 800, 600);

	while(!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	return EXIT_SUCCESS;
}
