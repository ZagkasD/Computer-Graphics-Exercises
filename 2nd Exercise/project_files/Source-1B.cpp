//********************************
//Αυτό το αρχείο θα το χρησιμοποιήσετε
// για να υλοποιήσετε την άσκηση 1B της OpenGL
//
//ΑΜ:4628                         Όνομα:Άγγελος Ανδρέου
//ΑΜ:4359                         Όνομα:Δημοσθένης Ζάγκας

//*********************************

// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>



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

/// <summary>
/// //////////////////////////////////////////////
/// </summary>


glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;


glm::mat4 getViewMatrix() {
	return ViewMatrix;
}
glm::mat4 getProjectionMatrix() {
	return ProjectionMatrix;
}

////////////////////////////////////////////// <summary>
/// Add camera function here
/// </summary>

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 30.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 2.5f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);


// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;
void camera_function()
{
	//if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		//glfwSetWindowShouldClose(window, true);
	// glfwGetTime is called only once, the first time this function is called
	
	// Compute time difference between current and last frame
	//camera speed
	float speed = static_cast < float> (2.5f * deltaTime); // 3 units / second

	float FoV = 45.0f;
	
	//glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 2.5f);
	// Direction :
	//glm::vec3 cameraDirection = glm::normalize(position - cameraTarget);
	// Up vector
	//glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);


	// Move camera around (UP)y++ (--)
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * speed;
		//cameraFront += cameraPos;
	}

	// Move camera around (DOWN)y-- (++)
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * speed;
		//cameraFront += cameraPos;
	}

	// move camera AROUND x (left)
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		cameraPos += cameraUp  * speed;
		//cameraFront += cameraPos;
	}
	// move camera AROUND x(RIGHT)
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
		cameraPos -= cameraUp  * speed;
		//cameraFront += cameraPos;
	}

	// move camera BACK
	if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS) {
		cameraPos += cameraPos * speed;
		//cameraFront += cameraPos * speed;
	}
	// move camera FRONT
	if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS) {
		cameraPos -= cameraPos * speed;
		//cameraFront += cameraPos;
	}

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	ProjectionMatrix = glm::perspective(glm::radians(FoV), 4.0f / 4.0f, 0.1f, 100.0f);
	 
	ViewMatrix = glm::lookAt(cameraPos, cameraFront, cameraUp);

	// For the next frame, the "last time" will be "now"
	//lastTime = currentTime1;
}

/////////////////////////////////////////////////

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
///////////////////////////////////////////////////



int main(void)
{
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(950, 950, u8"Εργασία 1Β", NULL, NULL);


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

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark gray background
	glClearColor(0.3f, 0.3f, 0.3f, 0.0f);
	
	//enable depth test
	glEnable(GL_DEPTH_TEST);

	
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders

	GLuint programID = LoadShaders("P1BVertexShader.vertexshader", "P1BFragmentShader.fragmentshader");

	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	//glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 4.0f, 0.1f, 100.0f);
	// Camera matrix
	//glm::mat4 View = glm::lookAt(
		//glm::vec3(0.0f, 0.0f, 30.0f), // Camera is at (0,0,30), in World Space
		//glm::vec3(0.0f, 0.0f, 2.5f), // and looks at the  E
		//glm::vec3(0.0f, 1.0f, 0.0f)  // y is up (set to 0,-1,0 to look upside-down)
	//);
	// Model matrix : an identity matrix (model will be at the origin)
	//glm::mat4 Model = glm::mat4(1.0f);
	// Our ModelViewProjection : multiplication of our 3 matrices
	//glm::mat4 MVP = Projection * View * Model; // Remember, matrix multiplication is the other way around
	
	//half values 
	GLfloat len = 5.0f, wid=2.5f, heig=2.5f;

	//for cube A
	static const GLfloat cube1[] =
	{
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

	//for cube B
	static const GLfloat cube2[] =
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

	//for cube c
	static const GLfloat cube3[] =
	{
		//front face
			//1st triangle
			-1.0f,-4.0f,-3.0f, //1c
			1.0f,-4.0f, -3.0f,//2c
			-1.0f, -6.0f,-3.0f,//3c
			//second triangle
			1.0f,-4.0f, -3.0f,//2c
			-1.0f, -6.0f,-3.0f,//3c
			1.0f, -6.0f,-3.0f,//4c

		//right face
			//1st triangle
			1.0f,-4.0f, -3.0f,//2c
			1.0f, -6.0f,-3.0f,//4c
			1.0f, -4.0f, -5.0f,//6c
			//second triangle
			1.0f, -6.0f,-3.0f,//4c
			1.0f, -4.0f, -5.0f,//6c
			1.0f, -6.0f,-5.0f,//5c

		//left face
			//1st triangle
			-1.0f,-4.0f,-3.0f, //1c
			-1.0f, -6.0f,-3.0f,//3c
			-1.0f, -6.0f, -5.0f,//8c
			//second triangle
			-1.0f,-4.0f,-3.0f, //1c
			-1.0f, -6.0f, -5.0f,//8c
			-1.0f, -4.0f,-5.0f,//7c

		//top face
			//1st triangle
			-1.0f,-4.0f,-3.0f, //1c
			1.0f,-4.0f, -3.0f,//2c
			1.0f, -4.0f, -5.0f,//6c
			//second triangle
			-1.0f,-4.0f,-3.0f, //1c
			1.0f, -4.0f, -5.0f,//6c
			-1.0f, -4.0f,-5.0f,//7c

		//bot face
			//1st triangle
			-1.0f, -6.0f,-3.0f,//3c
			1.0f, -6.0f,-3.0f,//4c
			-1.0f, -6.0f, -5.0f,//8c

			//second triangle
			1.0f, -6.0f,-3.0f,//4c
			-1.0f, -6.0f, -5.0f,//8c
			1.0f, -6.0f,-5.0f,//5c

		//back face
			//1st triangle
			1.0f, -4.0f, -5.0f,//6c
			1.0f, -6.0f,-5.0f,//5c
			-1.0f, -6.0f, -5.0f,//8c
			//second triangle
			1.0f, -4.0f, -5.0f,//6c
			-1.0f, -4.0f,-5.0f,//7c
			-1.0f, -6.0f, -5.0f,//8c
	};

	GLfloat a=0.5f;
	GLfloat b = 1.0f;
	static const GLfloat colorA[] = {
		
	0.00f,  0.488f,  0.000f,a,
	0.00f,  0.488f,  0.000f,a,
	0.00f,  0.488f,  0.000f,a,
	0.00f,  0.488f,  0.000f,a,
	0.00f,  0.488f,  0.000f,a,
	0.00f,  0.488f,  0.000f,a,

	1.00f,  0.000f,  0.000f,a,
	1.00f,  0.000f,  0.000f,a,
	1.00f,  0.000f,  0.000f,a,
	1.00f,  0.000f,  0.000f,a,
	1.00f,  0.000f,  0.000f,a,
	1.00f,  0.000f,  0.000f,a,

	1.00f,  0.00f,  1.000f,a,
	1.00f,  0.00f,  1.000f,a,
	1.00f,  0.00f,  1.000f,a,
	1.00f,  0.00f,  1.000f,a,
	1.00f,  0.00f,  1.000f,a,
	1.00f,  0.00f,  1.000f,a,

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

	
	static const GLfloat colorB[] = {
		0.0f,  0.0f,  0.0f,b,
	0.0f,  0.0f,  0.0f,b,
	0.0f,  0.0f,  0.0f,b,
	0.0f,  0.0f,  0.0f,b,
	0.0f,  0.0f,  0.0f,b,
	0.0f,  0.0f,  0.0f,b,

	0.0f,  0.0f,  0.3f,b,
	0.0f,  0.0f,  0.3f,b,
	0.0f,  0.0f,  0.3f,b,
	0.0f,  0.0f,  0.3f,b,
	0.0f,  0.0f,  0.3f,b,
	0.0f,  0.0f,  0.3f,b,

	0.2f,  0.2f,  0.8f,b,
	0.2f,  0.2f,  0.8f,b,
	0.2f,  0.2f,  0.8f,b,
	0.2f,  0.2f,  0.8f,b,
	0.2f,  0.2f,  0.8f,b,
	0.2f,  0.2f,  0.8f,b,

	0.000f,  0.000f,  0.600f,b,
	0.000f,  0.000f,  0.600f,b,
	0.000f,  0.000f,  0.600f,b,
	0.000f,  0.000f,  0.600f,b,
	0.000f,  0.000f,  0.600f,b,
	0.000f,  0.000f,  0.600f,b,

	0.300f,  0.300f,  0.600f,b,
	0.300f,  0.300f,  0.600f,b,
	0.300f,  0.300f,  0.600f,b,
	0.300f,  0.300f,  0.600f,b,
	0.300f,  0.300f,  0.600f,b,
	0.300f,  0.300f,  0.600f,b,

	0.500f,  0.500f,  0.900f,b,
	0.500f,  0.500f,  0.900f,b,
	0.500f,  0.500f,  0.900f,b,
	0.500f,  0.500f,  0.900f,b,
	0.500f,  0.500f,  0.900f,b,
	0.500f,  0.500f,  0.900f,b,
	};

	
	static const GLfloat colorC[] = {
		1.0f,  1.0f,  1.0f,b,
		1.0f,  1.0f,  1.0f,b,
		1.0f,  1.0f,  1.0f,b,
		1.0f,  1.0f,  1.0f,b,
		1.0f,  1.0f,  1.0f,b,
		1.0f,  1.0f,  1.0f,b,

		1.0f,  1.0f,  1.0f,b,
		1.0f,  1.0f,  1.0f,b,
		1.0f,  1.0f,  1.0f,b,
		1.0f,  1.0f,  1.0f,b,
		1.0f,  1.0f,  1.0f,b,
		1.0f,  1.0f,  1.0f,b,

		1.0f,  1.0f,  1.0f,b,
		1.0f,  1.0f,  1.0f,b,
		1.0f,  1.0f,  1.0f,b,
		1.0f,  1.0f,  1.0f,b,
		1.0f,  1.0f,  1.0f,b,
		1.0f,  1.0f,  1.0f,b,

		1.0f,  1.0f,  1.0f,b,
		1.0f,  1.0f,  1.0f,b,
		1.0f,  1.0f,  1.0f,b,
		1.0f,  1.0f,  1.0f,b,
		1.0f,  1.0f,  1.0f,b,
		1.0f,  1.0f,  1.0f,b,

		1.0f,  1.0f,  1.0f,b,
		1.0f,  1.0f,  1.0f,b,
		1.0f,  1.0f,  1.0f,b,
		1.0f,  1.0f,  1.0f,b,
		1.0f,  1.0f,  1.0f,b,
		1.0f,  1.0f,  1.0f,b,

		1.0f,  1.0f,  1.0f,b,
		1.0f,  1.0f,  1.0f,b,
		1.0f,  1.0f,  1.0f,b,
		1.0f,  1.0f,  1.0f,b,
		1.0f,  1.0f,  1.0f,b,
		1.0f,  1.0f,  1.0f,b,
	};

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube1), cube1, GL_STATIC_DRAW);

	GLuint vertexbuffer2;
	glGenBuffers(1, &vertexbuffer2);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube2), cube2, GL_STATIC_DRAW);

	GLuint vertexbuffer3;
	glGenBuffers(1, &vertexbuffer3);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer3);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube3), cube3, GL_STATIC_DRAW);
	
	GLuint colorbufferA;
	glGenBuffers(1, &colorbufferA);
	glBindBuffer(GL_ARRAY_BUFFER, colorbufferA);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colorA), colorA, GL_STATIC_DRAW);

	GLuint colorbufferB;
	glGenBuffers(1, &colorbufferB);
	glBindBuffer(GL_ARRAY_BUFFER, colorbufferB);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colorB), colorB, GL_STATIC_DRAW);

	GLuint colorbufferC;
	glGenBuffers(1, &colorbufferC);
	glBindBuffer(GL_ARRAY_BUFFER, colorbufferC);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colorC), colorC, GL_STATIC_DRAW);

	double currentTime = glfwGetTime();
	
	//float for scaling
	float scalex = 1.0f, scaley = 1.0f, scalez = 1.0f;
	do {

		// per-frame time logic
		// --------------------
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

		

		//ProjectionMatrix = glm::perspective(glm::radians(45.0f), 4.0f / 4.0f, 0.1f, 100.0f);
		// Camera matrix
		//ViewMatrix = glm::lookAt(cameraPos, cameraFront, cameraUp);
		glm::mat4 Model = glm::mat4(1.0f);
		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * Model;

		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		

	
		
		//get keys for scaling mult by delta time to limit fps and make sure it changes at normal speed we can see
		//change scale x up with u key down with p key
		if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
			//deltaTime = float(glfwGetTime() - currentTime);
			scalex += 2*2.0f*deltaTime;
		}
		else if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
			//deltaTime = float(glfwGetTime() - currentTime);
			scalex -= 2* 2.0f * deltaTime;
		}
		//change scale y up with i key down with o key
		if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
			//deltaTime = float(glfwGetTime() - currentTime);
			scaley += 2 * 2.0f * deltaTime;
		}
		else if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
			//deltaTime = float(glfwGetTime() - currentTime);
			scaley -= 2 * 2.0f * deltaTime;
		}

		//change scale z up with j key down with k key
		if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
			//deltaTime = float(glfwGetTime() - currentTime);
			scalez += 2 * 2.0f * deltaTime;
		}
		else if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
			//deltaTime = float(glfwGetTime() - currentTime);
			scalez -= 2 * 2.0f * deltaTime;
		}



		//draw C cube
		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer3);
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
		glBindBuffer(GL_ARRAY_BUFFER, colorbufferC);
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



		// TRANSFORMATION
		// 			// TRANSFORMATION

		glm::mat4 myScalingMatrix = glm::scale(glm::vec3(scalex, scaley, scalez));
	
		MVP = ProjectionMatrix * ViewMatrix * myScalingMatrix ;

		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		
		
	//draw A cube
		// 1rst attribute buffer : vertices
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
	


	 //draw B cube
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
		glBindBuffer(GL_ARRAY_BUFFER, colorbufferB);
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

	
	

	
		

		//transformation
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

