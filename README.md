# OpenGL template
Template for OpenGL C++17 apps that are developed in Visual Studio. Uses [any-project-setup](https://github.com/Matej-Chmel/any-project-setup) to download libraries GLEW, GLFW and GLM.

## Setup guide

1. Use this repo as a template.
2. Clone your new repo.
3. Run [init.bat](init.bat) to download the submodule and libraries.
4. Create Visual Studio solution in the root directory with Empty C++ project.
5. Add [main.cpp](src/main.cpp) to the project.
6. Add [OpenGL.props](res/props/OpenGL.props) as a property sheet for the project via Property Manager.
7. Set platform **x86** as active in the Configuration Manager.
8. Run the project and check that a triangle is rendered with no errors.

## Notes

After the libraries are downloaded, all unnecessary library files, e.g. code for x64 platform, are automatically deleted. To change this edit [libs.json](res/cfg/libs.json). Its schema is described in [README of any-project-setup](https://github.com/Matej-Chmel/any-project-setup/blob/master/README.md#zip-downloader).
