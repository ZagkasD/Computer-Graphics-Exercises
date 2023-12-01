﻿//********************************
// 2023
//Αυτό το αρχείο θα το χρησιμοποιήσετε
// για να υλοποιήσετε την άσκηση 1C της OpenGL
//
//ΑΜ:4628                         Όνομα:Άγγελος Ανδρέου
//ΑΜ:4359                         Όνομα:Δημοσθένης Ζάγκας

//*********************************
#define _CRT_SECURE_NO_WARNINGS

#define STB_IMAGE_IMPLEMENTATION
// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include "stb_image.h"


// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

using namespace glm;
using namespace std;

glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;

glm::mat4 getViewMatrix() {
	return ViewMatrix;
}
glm::mat4 getProjectionMatrix() {
	return ProjectionMatrix;
}

//************************************
// Η LoadShaders είναι black box για σας
//************************************

GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path) {

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if (VertexShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if (FragmentShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;


	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const* VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}



	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const* FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}



	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}


	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

///****************************************************************
//  Εδω θα υλοποιήσετε την συνάρτηση της κάμερας
//****************************************************************
// Camera definition
// Where the camera is located
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 40.0f);
// Where the camera is looking
glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 2.5f);
// Which side if "up" for the camera in 3d space
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
// View direction is a vector that starts at the camera target and points towards the camera position
glm::vec3 viewDirection = cameraPos - cameraTarget;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

void camera_function() {
	// Compute time difference between current and last frame
	// camera speed
	// float speed = static_cast <float> (20.0f * deltaTime);
	float cameraSpeed = (3.0f * deltaTime);

	float FoV = 45.0f;

	// Move camera around x (Up)
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), cameraSpeed, glm::vec3(1.0f, 0.0f, 0.0f));
		cameraPos = glm::vec3(rotationMatrix * glm::vec4(cameraPos - cameraTarget, 1.0f)) + cameraTarget;
		cameraUp = glm::vec3(rotationMatrix * glm::vec4(cameraUp, 0.0f));
	}
	// Move camera around x (Down)
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
		glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), cameraSpeed, glm::vec3(-1.0f, 0.0f, 0.0f));
		cameraPos = glm::vec3(rotationMatrix * glm::vec4(cameraPos - cameraTarget, 1.0f)) + cameraTarget;
		cameraUp = glm::vec3(rotationMatrix * glm::vec4(cameraUp, 0.0f));
	}
	// move camera AROUND y (Right)
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), cameraSpeed, glm::vec3(0.0f, 1.0f, 0.0f));
		cameraPos = glm::vec3(rotationMatrix * glm::vec4(cameraPos - cameraTarget, 1.0f)) + cameraTarget;
		cameraUp = glm::vec3(rotationMatrix * glm::vec4(cameraUp, 0.0f));
	}
	// move camera AROUND y (Left)
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
		glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), cameraSpeed, glm::vec3(0.0f, -1.0f, 0.0f));
		cameraPos = glm::vec3(rotationMatrix * glm::vec4(cameraPos - cameraTarget, 1.0f)) + cameraTarget;
		cameraUp = glm::vec3(rotationMatrix * glm::vec4(cameraUp, 0.0f));
	}
	// move camera BACK
	if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS) {
		cameraPos += cameraPos * (cameraSpeed);
	}
	// move camera FRONT
	if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS) {
		cameraPos -= cameraPos * (cameraSpeed);
	}

	ProjectionMatrix = glm::perspective(glm::radians(FoV), 4.0f / 4.0f, 0.1f, 100.0f);

	ViewMatrix = glm::lookAt(cameraPos, cameraTarget, cameraUp);
}


int main(void) {
	// Initialise GLFW
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(950, 950, u8"Εργασία 1Γ – CSG – Boolean Operations", NULL, NULL);

	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark gray background
	glClearColor(0.3f, 0.3f, 0.3f, 0.0f);

	// Makes objects opaque
	glEnable(GL_DEPTH_TEST);

	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders("P1C.vertexshader", "P1C.fragmentshader");
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	//////////////////////////////////////////////////////
	// Load the textures here
	// 
	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);

	// Create second OpenGL texture

	//glGenTextures(2, &textureID);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);
	/////////////////////////////////////////////////////
	//
	// Vertices 
	//for parallelogram A 

	static const GLfloat obj1[] = {
		//front face
		//1st triangle
		-5.0f,-2.5f,5.0f, //1
		-5.0f,2.5f, 5.0f,//2
		5.0f, 2.5f, 5.0f,//3
		//second triangle
		-5.0f,-2.5f,5.0f, //1
		5.0f, 2.5f, 5.0f,//3
		5.0f, -2.5f,5.0f,//4

		//right face
		//1st triangle
		5.0f, 2.5f, 5.0f,//3
		5.0f, -2.5f,5.0f,//4
		5.0f, 2.5f, 0.0f,//6
		//second triangle
		5.0f, -2.5f,5.0f,//4
		5.0f, 2.5f, 0.0f,//6
		5.0f, -2.5f,0.0f,//5

		//left face
		//1st triangle
		-5.0f,-2.5f,5.0f, //1
		-5.0f,2.5f, 5.0f,//2
		-5.0f, -2.5f, 0.0f,//8
		//second triangle
		-5.0f,2.5f, 5.0f,//2
		-5.0f, -2.5f, 0.0f,//8
		-5.0f, 2.5f,0.0f,//7

		//top face
		//1st triangle
		-5.0f,2.5f, 5.0f,//2
		5.0f, 2.5f, 5.0f,//3
		5.0f, 2.5f, 0.0f,//6
		//second triangle
		-5.0f,2.5f, 5.0f,//2
		5.0f, 2.5f, 0.0f,//6
		-5.0f, 2.5f,0.0f,//7

		//bot face
		//1st triangle
		-5.0f,-2.5f,5.0f, //1
		-5.0f, -2.5f, 0.0f,//8
		5.0f, -2.5f,0.0f,//5
		//second triangle
		5.0f, -2.5f,0.0f,//5
		-5.0f,-2.5f,5.0f, //1
		5.0f, -2.5f,5.0f,//4

		//back face
		//1st triangle
		5.0f, 2.5f, 0.0f,//6
		5.0f, -2.5f,0.0f,//5
		-5.0f, 2.5f,0.0f,//7
		//second triangle
		5.0f, -2.5f,0.0f,//5
		-5.0f, 2.5f,0.0f,//7
		-5.0f, -2.5f, 0.0f,//8
	};

	//for parallelogram B (the tall one)
	static const GLfloat obj2[] =
	{
		//front face
		//1st triangle
		-2.5f,14.0f,5.0f, //1b
		-2.5f,0.0f, 5.0f,//2b
		2.5f, 14.0f,5.0f,//4b
		//second triangle
		-2.5f,0.0f, 5.0f,//2b
		2.5f, 0.0f,5.0f,//3b
		2.5f, 14.0f,5.0f,//4b

		//right face
		//1st triangle
		2.5f, 14.0f,5.0f,//4b
		2.5f, 0.0f,5.0f,//3b
		2.5f, 14.0f, 0.0f,//6b
		//second triangle
		2.5f, 0.0f,5.0f,//3b
		2.5f, 14.0f, 0.0f,//6b
		2.5f, 0.0f,0.0f,//5b

		//left face
		//1st triangle
		-2.5f,14.0f,5.0f, //1b
		-2.5f,0.0f, 5.0f,//2b
		-2.5f, 0.0f, 0.0f,//8b
		//second triangle
		-2.5f,14.0f,5.0f, //1b
		-2.5f, 0.0f, 0.0f,//8b
		-2.5f, 14.0f,0.0f,//7b

		//top face
		//1st triangle
		-2.5f,14.0f,5.0f, //1b
		-2.5f, 14.0f,0.0f,//7b
		2.5f, 14.0f, 0.0f,//6b
		//second triangle
		-2.5f,14.0f,5.0f, //1b
		2.5f, 14.0f, 0.0f,//6b
		2.5f, 14.0f,5.0f,//4b

		//bot face
		//1st triangle
		-2.5f,0.0f, 5.0f,//2b
		-2.5f, 0.0f, 0.0f,//8b
		2.5f, 0.0f,0.0f,//5b

		//second triangle
		-2.5f,0.0f, 5.0f,//2b
		2.5f, 0.0f,0.0f,//5b
		2.5f, 0.0f,5.0f,//3b

		//back face
		//1st triangle
		2.5f, 0.0f,0.0f,//5b
		2.5f, 14.0f, 0.0f,//6b
		-2.5f, 14.0f,0.0f,//7b
		//second triangle
		2.5f, 0.0f,0.0f,//5b
		-2.5f, 14.0f,0.0f,//7b
		-2.5f, 0.0f, 0.0f,//8b
	};

	//for boolean operation A U B
	static const GLfloat obj_u[] = {
		//front face
		//1st triangle 1
		-5.0f,-2.5f,5.0f, //1
		-5.0f,2.5f, 5.0f,//2
		5.0f, 2.5f, 5.0f,//3
		//second triangle 2
		-5.0f,-2.5f,5.0f, //1
		5.0f, 2.5f, 5.0f,//3
		5.0f, -2.5f,5.0f,//4

		//3rd triangle 3
		-2.5f,14.0f,5.0f, //1b
		-2.5f,2.5f,5.0f,//9
		2.5f, 14.0f,5.0f,//4b

		//4th triangle 4
		2.5f,2.5f,5.0f,//11
		-2.5f,2.5f,5.0f,//9
		2.5f, 14.0f,5.0f,//4b


		//right face 
		//1st triangle 5
		5.0f, 2.5f, 5.0f,//3
		5.0f, -2.5f,5.0f,//4
		5.0f, 2.5f, 0.0f,//6
		//second triangle 6
		5.0f, -2.5f,5.0f,//4
		5.0f, 2.5f, 0.0f,//6
		5.0f, -2.5f,0.0f,//5

		//3rd triangle 7
		
		2.5f, 14.0f,5.0f,//4b
		2.5f,2.5f,5.0f,//11
		2.5f, 14.0f, 0.0f,//6b
		//4th triangle 8
		2.5f,2.5f,5.0f,//11
		2.5f, 14.0f, 0.0f,//6b
		2.5f,2.5f, 0.0f,//12

		//left face
		//1st triangle 9
		-5.0f,-2.5f,5.0f, //1
		-5.0f,2.5f, 5.0f,//2
		-5.0f, -2.5f, 0.0f,//8
		//second triangle 10
		-5.0f,2.5f, 5.0f,//2
		-5.0f, -2.5f, 0.0f,//8
		-5.0f, 2.5f,0.0f,//7

		//1st triangle B 11
		-2.5f,14.0f,5.0f, //1b
		-2.5f, 2.5f,0.0f,//10
		-2.5f,2.5f,5.0f,//9
		//second triangle B 12
		-2.5f,14.0f,5.0f, //1b
		-2.5f, 2.5f,0.0f,//10
		-2.5f, 14.0f,0.0f,//7b

		

		//bot face same as A's
		//1st triangle 13
		-5.0f,-2.5f,5.0f, //1
		-5.0f, -2.5f, 0.0f,//8
		5.0f, -2.5f,0.0f,//5
		//second triangle 14
		5.0f, -2.5f,0.0f,//5
		-5.0f,-2.5f,5.0f, //1
		5.0f, -2.5f,5.0f,//4

		//back face
		//1st triangle  15
		-5.0f,-2.5f,0.0f, //8
		-5.0f,2.5f, 0.0f,//7
		5.0f, -2.5f, 0.0f,//5
		//second triangle 16
		-5.0f,2.5f, 0.0f,//7
		5.0f, -2.5f, 0.0f,//5
		5.0f, 2.5f, 0.0f,//6

		//3rd triangle 17
		-2.5f,14.0f,0.0f, //7b
		-2.5f,2.5f,0.0f,//10
		2.5f,2.5f, 0.0f,//12

		//4th triangle 18
		-2.5f,14.0f,0.0f, //7b
		2.5f,2.5f, 0.0f,//12
		2.5f, 14.0f, 0.0f,//6b

		//top face A
		//1st triangle 19
		-5.0f,2.5f, 5.0f,//2
		-2.5f,2.5f,0.0f,//10
		-2.5f,2.5f,5.0f,//9
		//second triangle 20
		-5.0f,2.5f, 5.0f,//2
		-2.5f,2.5f,0.0f,//10
		-5.0f, 2.5f,0.0f,//7

		//1st triangle 21
		2.5f, 2.5f, 0.0f,//12
		5.0f, 2.5f, 5.0f,//3
		5.0f, 2.5f, 0.0f,//6
		//second triangle 22
		2.5f, 2.5f, 0.0f,//12
		5.0f, 2.5f, 5.0f,//3
		2.5f, 2.5f, 5.0f,//11


		//top face B
		//1st triangle 23
		-2.5f,14.0f,5.0f, //1b
		-2.5f, 14.0f,0.0f,//7b
		2.5f, 14.0f, 0.0f,//6b
		//second triangle 24
		-2.5f,14.0f,5.0f, //1b
		2.5f, 14.0f, 0.0f,//6b
		2.5f, 14.0f,5.0f,//4b

	
	};

	//for object A incision Β (cube)
	static const GLfloat obj_i[] =
	{
		//front face
			//1st triangle
			-2.5f,2.5f,5.0f,//9
			2.5f, 2.5f, 5.0f,//11
			-2.5f,0.0f, 5.0f,//2b
			//second triangle
			2.5f, 2.5f, 5.0f,//11
			-2.5f,0.0f, 5.0f,//2b
			2.5f, 0.0f,5.0f,//3b

		//right face
			//1st triangle
			2.5f, 2.5f, 5.0f,//11
			2.5f, 0.0f,5.0f,//3b
			2.5f, 2.5f, 0.0f,//12
			//second triangle
			2.5f, 0.0f,5.0f,//3b
			2.5f, 2.5f, 0.0f,//12
			2.5f, 0.0f,0.0f,//5b

		//left face
			//1st triangle
			-2.5f,2.5f,0.0f,//10
			-2.5f,2.5f,5.0f,//9
			-2.5f, 0.0f, 0.0f,//8b
			//second triangle
			-2.5f,2.5f,5.0f,//9
			-2.5f, 0.0f, 0.0f,//8b
			-2.5f,0.0f, 5.0f,//2b

		//top face
			//1st triangle
			-2.5f,2.5f,0.0f,//10
			2.5f, 2.5f, 0.0f,//12
			-2.5f,2.5f,5.0f,//9
			//second triangle
			2.5f, 2.5f, 0.0f,//12
			-2.5f,2.5f,5.0f,//9
			2.5f, 2.5f, 5.0f,//11
		
		//bot face
			//1st triangle
			-2.5f,0.0f, 5.0f,//2b
			2.5f, 0.0f,5.0f,//3b
			-2.5f, 0.0f, 0.0f,//8b
			//second triangle
			2.5f, 0.0f,5.0f,//3b
			-2.5f, 0.0f, 0.0f,//8b
			2.5f, 0.0f,0.0f,//5b

		//back face
			//1st triangle
			-2.5f,2.5f,0.0f,//10
			2.5f, 2.5f, 0.0f,//12
			2.5f, 0.0f,0.0f,//5b
			//second triangle
			-2.5f,2.5f,0.0f,//10
			-2.5f, 0.0f, 0.0f,//8b
			2.5f, 0.0f,0.0f,//5b
			
	};


	//for boolean operation A-B
	static const GLfloat obj_d[] = {
		//front face
		//1st triangle 1
		-5.0f,-2.5f,5.0f, //1
		-5.0f,2.5f, 5.0f,//2
		-2.5f,2.5f,5.0f,//9
		//second triangle 2
		-5.0f,-2.5f,5.0f, //1
		-2.5f,2.5f,5.0f,//9
		-2.5f,-2.5f,5.0f,//1d 
		//3rd triangle 3
		-2.5f,0.0f, 5.0f,//2b
		-2.5f,-2.5f,5.0f,//2d 
		2.5f, 0.0f,5.0f,//3b
		//4th triangle 4
		-2.5f,-2.5f,5.0f,//1d 
		2.5f, 0.0f,5.0f,//3b
		2.5f, -2.5f,5.0f,//2d
		//5th triangle 5
		2.5f,2.5f,5.0f,//11
		2.5f, -2.5f,5.0f,//2d
		5.0f, 2.5f, 5.0f,//3
		//6th triangle 6
		5.0f, 2.5f, 5.0f,//3
		5.0f, -2.5f,5.0f,//4
		2.5f, -2.5f,5.0f,//2d
		


		//right face
		//1st triangle 7
		5.0f, 2.5f, 5.0f,//3
		5.0f, -2.5f,5.0f,//4
		5.0f, 2.5f, 0.0f,//6
		//second triangle 8
		5.0f, -2.5f,5.0f,//4
		5.0f, 2.5f, 0.0f,//6
		5.0f, -2.5f,0.0f,//5
	

		//left face
		//1st triangle 9
		-5.0f,-2.5f,5.0f, //1
		-5.0f,2.5f, 5.0f,//2
		-5.0f, -2.5f, 0.0f,//8
		//second triangle 10
		-5.0f,2.5f, 5.0f,//2
		-5.0f, -2.5f, 0.0f,//8
		-5.0f, 2.5f,0.0f,//7

		
		//top face
		//1st top square
		//1st triangle 11
		-5.0f,2.5f, 5.0f,//2
		-5.0f, 2.5f,0.0f,//7
		-2.5f, 2.5f,0.0f,//10
		//second triangle 12
		-5.0f,2.5f, 5.0f,//2
		-2.5f, 2.5f,0.0f,//10
		-2.5f, 2.5f,5.0f,//9

		//2nd top square
		//1st triangle 13
		2.5f, 2.5f,5.0f,//11
		5.0f, 2.5f, 0.0f,//6
		5.0f, 2.5f, 5.0f,//3
		//second triangle 14
		2.5f,2.5f, 0.0f,//12
		2.5f, 2.5f,5.0f,//11
		5.0f, 2.5f, 0.0f,//6

		//bot face
		//1st triangle 15
		-5.0f,-2.5f,5.0f, //1
		-5.0f, -2.5f, 0.0f,//8
		5.0f, -2.5f,0.0f,//5
		//second triangle 16
		5.0f, -2.5f,0.0f,//5
		-5.0f,-2.5f,5.0f, //1
		5.0f, -2.5f,5.0f,//4

		//back face
		//1st triangle 17
		5.0f, 2.5f, 0.0f,//6
		2.5f,2.5f, 0.0f,//12
		5.0f, -2.5f,0.0f,//5
		//second triangle 18
		2.5f,2.5f, 0.0f,//12
		5.0f, -2.5f,0.0f,//5
		2.5f,-2.5f,0.0f,//3d 
		
		//3rd triangle 19
		2.5f,-2.5f,0.0f,//3d 
		2.5f, 0.0f,0.0f,//5b
		-2.5f, 0.0f, 0.0f,//8b
		//4th triangle 20
		2.5f,-2.5f,0.0f,//3d 
		-2.5f, 0.0f, 0.0f,//8b
		-2.5f, -2.5f,0.0f,//4d
		
		//5th triangle 21
		-2.5f, 2.5f,0.0f,//10
		-2.5f, -2.5f,0.0f,//4d
		-5.0f, 2.5f,0.0f,//7
		//6th triangle 22
		-2.5f, -2.5f,0.0f,//4d
		-5.0f, 2.5f,0.0f,//7
		-5.0f, -2.5f, 0.0f,//8

		//new inside faces (gap in between)
		//left side
		
			//1st triangle 23
			-2.5f, 2.5f, 5.0f,//9
			-2.5f, 2.5f, 0.0f,//10
			-2.5f, 0.0f, 5.0f,//2b
			//second triangle24 
			-2.5f, 2.5f, 0.0f,//10
			-2.5f, 0.0f, 5.0f,//2b
			-2.5f, 0.0f, 0.0f,//8b

			//right side
			//1st triangle 25
			2.5f, 2.5f, 0.0f,//12
			2.5f, 0.0f, 0.0f,//5b
			2.5f, 2.5f, 5.0f,//11
			//second triangle 26
			2.5f, 0.0f, 0.0f,//5b
			2.5f, 2.5f, 5.0f,//11
			2.5f, 0.0f, 5.0f,//3b

			//bot side
			//1st triangle 27
			-2.5f, 0.0f, 0.0f,//8b
			2.5f, 0.0f, 0.0f,//5b
			-2.5f, 0.0f, 5.0f,//2b
			//second triangle 28
			2.5f, 0.0f, 0.0f,//5b
			-2.5f, 0.0f, 5.0f,//2b
			2.5f, 0.0f, 5.0f,//3b
	};

	//for parallelogram B-A 
	static const GLfloat obj_f[] =
	{
		//front face
		//1st triangle
		-2.5f,14.0f,5.0f, //1b
		-2.5f, 2.5f,5.0f,//9
		2.5f, 14.0f,5.0f,//4b
		//second triangle
		-2.5f, 2.5f,5.0f,//9
		2.5f, 14.0f,5.0f,//4b
		2.5f, 2.5f, 5.0f,//11

		//right face
		//1st triangle
		2.5f, 14.0f,5.0f,//4b
		2.5f, 2.5f, 5.0f,//11
		2.5f, 2.5f, 0.0f,//12
		//second triangle
		2.5f, 14.0f,5.0f,//4b
		2.5f, 2.5f, 0.0f,//12
		2.5f, 14.0f, 0.0f,//6b
		

		//left face
		//1st triangle
		-2.5f,14.0f,5.0f, //1b
		-2.5f, 2.5f,5.0f,//9
		-2.5f, 2.5f,0.0f,//10
		//second triangle
		-2.5f,14.0f,5.0f, //1b
		-2.5f, 2.5f,0.0f,//10
		-2.5f, 14.0f,0.0f,//7b

		//top face
		//1st triangle
		-2.5f,14.0f,5.0f, //1b
		-2.5f, 14.0f,0.0f,//7b
		2.5f, 14.0f, 0.0f,//6b
		//second triangle
		-2.5f,14.0f,5.0f, //1b
		2.5f, 14.0f, 0.0f,//6b
		2.5f, 14.0f,5.0f,//4b

		//bot face
		//1st triangle
		-2.5f, 2.5f,5.0f,//9
		-2.5f, 2.5f,0.0f,//10
		2.5f, 2.5f, 0.0f,//12
		//second triangle
		2.5f, 2.5f, 0.0f,//12
		-2.5f, 2.5f,5.0f,//9
		2.5f, 2.5f, 5.0f,//11
		
		//back face
		//1st triangle
		-2.5f, 2.5f,0.0f,//10
		2.5f, 14.0f, 0.0f,//6b
		-2.5f, 14.0f,0.0f,//7b
		//second triangle
		-2.5f, 2.5f,0.0f,//10
		2.5f, 14.0f, 0.0f,//6b
		2.5f, 2.5f, 0.0f,//12
		
	};

	GLfloat a = 1.0f;
	static const GLfloat colorA[] = {
		0.000f,  0.488f,  0.000f,a,
		0.000f,  0.488f,  0.000f,a,
		0.000f,  0.488f,  0.000f,a,
		0.000f,  0.488f,  0.000f,a,
		0.000f,  0.488f,  0.000f,a,
		0.000f,  0.488f,  0.000f,a,

		1.000f,  0.000f,  0.000f,a,
		1.000f,  0.000f,  0.000f,a,
		1.000f,  0.000f,  0.000f,a,
		1.000f,  0.000f,  0.000f,a,
		1.000f,  0.000f,  0.000f,a,
		1.000f,  0.000f,  0.000f,a,

		1.000f,  0.000f,  1.000f,a,
		1.000f,  0.000f,  1.000f,a,
		1.000f,  0.000f,  1.000f,a,
		1.000f,  0.000f,  1.000f,a,
		1.000f,  0.000f,  1.000f,a,
		1.000f,  0.000f,  1.000f,a,

		0.600f,  0.400f,  0.600f,a,
		0.600f,  0.400f,  0.600f,a,
		0.600f,  0.400f,  0.600f,a,
		0.600f,  0.400f,  0.600f,a,
		0.600f,  0.400f,  0.600f,a,
		0.600f,  0.400f,  0.600f,a,

		0.000f,  1.000f,  0.000f,a,
		0.000f,  1.000f,  0.000f,a,
		0.000f,  1.000f,  0.000f,a,
		0.000f,  1.000f,  0.000f,a,
		0.000f,  1.000f,  0.000f,a,
		0.000f,  1.000f,  0.000f,a,

		0.300f,  1.000f,  0.300f,a,
		0.300f,  1.000f,  0.300f,a,
		0.300f,  1.000f,  0.300f,a,
		0.300f,  1.000f,  0.300f,a,
		0.300f,  1.000f,  0.300f,a,
		0.300f,  1.000f,  0.300f,a,
	};

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(obj1), obj1, GL_STATIC_DRAW);

	GLuint vertexbuffer2;
	glGenBuffers(1, &vertexbuffer2);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(obj2), obj2, GL_STATIC_DRAW);

	GLuint vertexbuffer_u;
	glGenBuffers(1, &vertexbuffer_u);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_u);
	glBufferData(GL_ARRAY_BUFFER, sizeof(obj_u), obj_u, GL_STATIC_DRAW);

	GLuint vertexbuffer_i;
	glGenBuffers(1, &vertexbuffer_i);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_i);
	glBufferData(GL_ARRAY_BUFFER, sizeof(obj_i), obj_i, GL_STATIC_DRAW);

	GLuint vertexbuffer_d;
	glGenBuffers(1, &vertexbuffer_d);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_d);
	glBufferData(GL_ARRAY_BUFFER, sizeof(obj_d), obj_d, GL_STATIC_DRAW);

	GLuint vertexbuffer_f;
	glGenBuffers(1, &vertexbuffer_f);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_f);
	glBufferData(GL_ARRAY_BUFFER, sizeof(obj_f), obj_f, GL_STATIC_DRAW);

	GLuint colorbufferA;
	glGenBuffers(1, &colorbufferA);
	glBindBuffer(GL_ARRAY_BUFFER, colorbufferA);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colorA), colorA, GL_STATIC_DRAW);

	//double currentTime = glfwGetTime();

	//float for scaling
	float scalex = 1.0f, scaley = 1.0f, scalez = 1.0f;

	//int to check which boolean operation to show
	int oper = 0;
	do {

		// per-frame time logic
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Compute the MVP matrix from keyboard input
		camera_function();

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Use our shader
		glUseProgram(programID);



		// Camera matrix
		glm::mat4 Model = glm::mat4(1.0f);
		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * Model;

		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);


		// Get keys for scaling
		// Multiply by delta time to limit fps and make sure it changes at normal speed
		// change scale x up with u key down with p key
		//if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
		//	scalex += 2 * 2.0f * deltaTime;
		//}
		 if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
			scalex -= 2 * 2.0f * deltaTime;
		}
		//change scale y up with i key down with o key
		//if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
		//	scaley += 2 * 2.0f * deltaTime;
		//}
		else if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
			scaley -= 2 * 2.0f * deltaTime;
		}

		//change scale z up with j key down with k key
		if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
			scalez += 2 * 2.0f * deltaTime;
		}
		else if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
			scalez -= 2 * 2.0f * deltaTime;
		}

		//boolean operations check

		if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
			oper = 0;

		}

		if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
			oper = 1;

		}

		if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
			oper = 2;

		}
		
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			oper = 3;

		}
		if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
			oper = 4;

		}


		// TRANSFORMATION
		glm::mat4 myScalingMatrix = glm::scale(glm::vec3(scalex, scaley, scalez));

		MVP = ProjectionMatrix * ViewMatrix * myScalingMatrix;

		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		//draw original shapes key==r
		if (oper == 0) {
			// draw A object
			// 1st attribute buffer : vertices
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
			glVertexAttribPointer(
				0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
				3,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				0,                  // stride
				(void*)0            // array buffer offset
			);


			// 2nd attribute buffer : colors
			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, colorbufferA);
			glVertexAttribPointer(
				1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
				4,                                // size
				GL_FLOAT,                         // type
				GL_FALSE,                         // normalized?
				0,                                // stride
				(void*)0                          // array buffer offset
			);

			// Draw the triangle !
			glDrawArrays(GL_TRIANGLES, 0, 36); // 3 indices starting at 0 -> 1 triangle



			// draw B cube
			// 1rst attribute buffer : vertices
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer2);
			glVertexAttribPointer(
				0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
				3,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				0,                  // stride
				(void*)0            // array buffer offset
			);

			// 2nd attribute buffer : colors
			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, colorbufferA);
			glVertexAttribPointer(
				1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
				4,                                // size
				GL_FLOAT,                         // type
				GL_FALSE,                         // normalized?
				0,                                // stride
				(void*)0                          // array buffer offset
			);

			// Draw the triangle !
			glDrawArrays(GL_TRIANGLES, 0, 36); // 3 indices starting at 0 -> 1 triangle
		}
		//draw shape: A U B Key==U
		if (oper == 1) {
			// draw A U B object
			// 1st attribute buffer : vertices
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_u);
			glVertexAttribPointer(
				0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
				3,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				0,                  // stride
				(void*)0            // array buffer offset
			);

			// Draw the triangles !
			glDrawArrays(GL_TRIANGLES, 0, 72); // 3 indices starting at 0 -> 1 triangle 24 tottal triangles
		}

		//draw shape: A ins B Key==i
		if (oper == 2) {
			// draw  A ins B object
			// 1st attribute buffer : vertices
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_i);
			glVertexAttribPointer(
				0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
				3,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				0,                  // stride
				(void*)0            // array buffer offset
			);

			// Draw the triangles !
			glDrawArrays(GL_TRIANGLES, 0, 36); // 3 indices starting at 0 -> 1 triangle, 12 tottal triangles
		}

		//draw shape: A-B Key==d
		if (oper == 3) {
			// draw A object
			// 1st attribute buffer : vertices
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_d);
			glVertexAttribPointer(
				0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
				3,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				0,                  // stride
				(void*)0            // array buffer offset
			);
			
			// Draw the triangles !
			glDrawArrays(GL_TRIANGLES, 0, 84); // 3 indices starting at 0 -> 1 triangle, 28 tottal triangles
		}

		//draw object B-A key==F
		if (oper == 4) {
			// draw B-A object
			// 1st attribute buffer : vertices
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_f);
			glVertexAttribPointer(
				0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
				3,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				0,                  // stride
				(void*)0            // array buffer offset
			);
			// Draw the triangles !
			glDrawArrays(GL_TRIANGLES, 0, 36); // 3 indices starting at 0 -> 1 triangle, 12 tottal triangles
		}
	
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();


	} // Check if the SPACE key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_SPACE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	// Cleanup VBO
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteProgram(programID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

