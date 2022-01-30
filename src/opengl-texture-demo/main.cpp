#include <filesystem>
#include <fstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL/SOIL.h>
#include <streambuf>
#include <string>
namespace fs = std::filesystem;

const auto RES_PATH = fs::path(RES);

constexpr GLfloat TRIANGLE[]{
	-.8f, -.5f, 0.f, .1f, 0.f,
	.8f, -.5f, 0.f, .9f, 0.f,
	0.f, .5f, 0.f, .5f, .5f
};

constexpr auto WIDTH = 800;
constexpr auto HEIGHT = 600;

static GLuint compileShader(const fs::path& p, const GLuint prog, const GLenum type) {
	std::ifstream s(RES_PATH / "shaders" / p);
	const auto content = std::string(
		std::istreambuf_iterator(s),
		std::istreambuf_iterator<char>()
	);
	const auto data = content.c_str();

	const auto ID = glCreateShader(type);
	glShaderSource(ID, 1, &data, nullptr);
	glCompileShader(ID);
	glAttachShader(prog, ID);
	return ID;
}

int main() {
	(void)glfwInit();

	auto window = glfwCreateWindow(WIDTH, HEIGHT, "Texture demo", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	(void)glewInit();
	glViewport(0, 0, WIDTH, HEIGHT);

	const auto prog = glCreateProgram();
	const auto frag = compileShader("texture.frag", prog, GL_FRAGMENT_SHADER);
	const auto vert = compileShader("texture.vert", prog, GL_VERTEX_SHADER);
	glLinkProgram(prog);
	glDetachShader(prog, frag);
	glDetachShader(prog, vert);
	glDeleteShader(frag);
	glDeleteShader(vert);

	glUseProgram(prog);
	glUniform1i(glGetUniformLocation(prog, "texUnit"), 0);

	GLuint VAO, VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TRIANGLE), TRIANGLE, GL_STATIC_DRAW);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	const auto stride = 5 * sizeof(GLfloat);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (const void*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (const void*)(3 * sizeof(GLfloat)));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(
		GL_TEXTURE_2D, SOIL_load_OGL_texture(
			(RES_PATH / "textures" / "wood.png").string().c_str(),
			SOIL_LOAD_RGBA, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y
	));

	while(!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteProgram(prog);
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
