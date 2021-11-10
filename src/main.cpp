#include <cstdlib>
#include <exception>
#include <filesystem>
#include <fstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>
#include <iostream>
#include <sstream>
#include <streambuf>
#include <string>
namespace fs = std::filesystem;

constexpr glm::vec3 BACK_COLOR{.8f, 1.f, .9f};
constexpr auto TITLE = "Triangle";
constexpr auto WIDTH = 800;
constexpr auto HEIGHT = 600;

const GLfloat triangleVertices[]{
	-.8f, -.5f, 0.f, 0.f, 1.f, 1.f,
	.8f, -.5f, 0.f, 0.f, .5f, .8f,
	0.f, .5f, 0.f, 0.f, .3f, .7f
};

class AppError : public std::exception {
public:
	AppError(const char* const msg) : std::exception(msg) {}
	AppError(const std::string& msg) : AppError(msg.c_str()) {}
	AppError(const std::stringstream& s) : AppError(s.str()) {}
};

static void errorCallback(const int code, const char* const desc) {
	std::cerr << "[ERROR " << code << "] " << desc << ".\n";
}

std::stringstream operator"" _f(const char* const s, const size_t _) {
	return std::stringstream() << s;
}

class readFile {
	std::string content;
	const char* data;

public:
	operator const char** () {
		return &this->data;
	}

	readFile(const fs::path& p) {
		std::ifstream s(p);

		if(!s)
			throw AppError("Could not open file "_f << p.string() << '.');

		this->content = std::string(std::istreambuf_iterator(s), std::istreambuf_iterator<char>());
		this->data = this->content.c_str();
	}
};

int main() {
	try {
		glfwSetErrorCallback(errorCallback);

		if(glfwInit() == GLFW_FALSE)
			throw AppError("Could not init GLFW.");

		const auto window = glfwCreateWindow(WIDTH, HEIGHT, TITLE, nullptr, nullptr);

		if(!window)
			throw AppError("Could not create window \""_f << TITLE << "\".");

		glfwMakeContextCurrent(window);

		glewExperimental = GL_TRUE;
		const auto err = glewInit();

		if(err != GLEW_OK)
			throw AppError("Could not init GLEW. "_f << glewGetErrorString(err) << '.');

		glViewport(0, 0, WIDTH, HEIGHT);

		GLuint VAO, VBO;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);

		const auto stride = 6 * sizeof(GLfloat);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (const void*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (const void*)(3 * sizeof(GLfloat)));

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		const auto shadersDir = fs::path(RES) / "shaders";

		const auto frag = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(frag, 1, readFile(shadersDir / "constantGradient.frag"), nullptr);
		glCompileShader(frag);

		const auto vert = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vert, 1, readFile(shadersDir / "constantGradient.vert"), nullptr);
		glCompileShader(vert);

		const auto prog = glCreateProgram();
		glAttachShader(prog, frag);
		glAttachShader(prog, vert);
		glLinkProgram(prog);

		GLint status;
		glGetProgramiv(prog, GL_LINK_STATUS, &status);

		if(status == GL_FALSE) {
			GLint infoLogLength;
			glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &infoLogLength);
			auto infoLog = new GLchar[infoLogLength + 1];
			glGetProgramInfoLog(prog, infoLogLength, nullptr, infoLog);
			const auto s = "Linker failure\n\n"_f << infoLog;
			delete[] infoLog;
			throw AppError(s);
		}

		glDetachShader(prog, frag);
		glDetachShader(prog, vert);
		glDeleteShader(frag);
		glDeleteShader(vert);

		while(glfwWindowShouldClose(window) == GLFW_FALSE) {
			glClear(GL_COLOR_BUFFER_BIT);
			glClearColor(BACK_COLOR.x, BACK_COLOR.y, BACK_COLOR.z, 1.f);

			glUseProgram(prog);
			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, 3);

			glfwPollEvents();
			glfwSwapBuffers(window);
		}

		glfwTerminate();

	} catch(const AppError& e) {
		glfwTerminate();

		std::cerr << "[AppError] " << e.what() << '\n';
		std::cout << "Press ENTER to exit.\n";
		std::cin.get();
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
