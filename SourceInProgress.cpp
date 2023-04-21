#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Sphere.h"
#include "shader.h"
#include <iostream>
#include "cylinder.h"
#include "halftube.h"
#include "shader_m.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 900;

// camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 10.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);



bool firstMouse = true;
float yaw = 0.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 45.0f;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

//ortho
glm::mat4 projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

// lighting
glm::vec3 lightPos(-5.0f, 5.0f, 10.0f);

bool gIsLampOrbiting = true;
unsigned int loadTexture(char const* path);



int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);



#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile our shader zprogram
	// ------------------------------------
	//Shader ourShader("shaderfiles/7.3.camera.vs", "shaderfiles/7.3.camera.fs");
	Shader lightingShader("shaderfiles/6.multiple_lights.vs", "shaderfiles/6.multiple_lights.fs");
	Shader lightCubeShader("shaderfiles/6.light_cube.vs", "shaderfiles/6.light_cube.fs");


	//Shader lightingShader("shaderfiles/objectVertexShader.vs", "shaderfiles/objectFragsmentShader.fs");
	//Shader lightCubeShader("shaderfiles/lampVertexShader.vs", "shaderfiles/lampFramentShader.fs");
	// set up vertex data (and buffer(s)) and configure vertex attributes
	//findamp
	// ------------------------------------------------------------------
	float vertices[] = {

		// left side 
	-4.0f, 0.0f, -2.0f,  -1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
	-4.0f, 3.0f, -2.0f,  -1.0f, 0.0f, 0.0f,   0.0f, 1.0f,
	-4.0f, 0.0f,  0.0f,  -1.0f, 0.0f, 0.0f,   0.5f, 0.0f,

	-4.0f, 3.0f, -2.0f,  -1.0f, 0.0f, 0.0f,   0.0f, 1.0f,
	-4.0f, 3.0f, 0.0f,	 -1.0f, 0.0f, 0.0f,   0.5f, 1.0f,
	-4.0f, 0.0f, 0.0f,	 -1.0f, 0.0f, 0.0f,   0.5f, 0.0f,

	// amp bottom
	-4.0f, 0.0f, -2.0f,  0.0f, -1.0f, 0.0f,  0.0f, 1.0f,
	2.0f, 0.0f, -2.0f,   0.0f, -1.0f, 0.0f,  0.5f, 1.0f,
	-4.0f, 0.0f, 0.0f,   0.0f, -1.0f, 0.0f,  1.0f, 0.0f,

	2.0f, 0.0f, -2.0f,   0.0f, -1.0f, 0.0f,  0.5f, 1.0f,
	2.0f, 0.0f, 0.0f,    0.0f, -1.0f, 0.0f,  0.8f, 0.0f,
	-4.0f, 0.0f, 0.0f,   0.0f, -1.0f, 0.0f,  0.0f, 0.0f,

	// amp right side
	2.0f, 0.0f, -2.0f,   1.0f, 0.0f, 0.0f,  0.5f, 0.0f,
	2.0f, 3.0f, -2.0f,   1.0f, 0.0f, 0.0f,    0.5f, 1.0f,
	2.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f,   0.0f, 0.0f,

	2.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
	2.0f, 3.0f, 0.0f,    1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
	2.0f, 3.0f, -2.0f,   1.0f, 0.0f, 0.0f,    0.5f, 1.0f,

	// amp back
	-4.0f, 0.0f, -2.0f,   0.0f, 0.0f, -1.0f,   1.0f, 0.0f,
	-4.0f, 3.0f, -2.0f,   0.0f, 0.0f, -1.0f,   1.0f, 1.0f,
	 2.0f, 3.0f, -2.0f,   0.0f, 0.0f, -1.0f,  0.0f, 1.0f,
	-4.0f, 0.0f, -2.0f,   0.0f, 0.0f, -1.0f,  1.0f, 0.0f,
	 2.0f, 0.0f, -2.0f,   0.0f, 0.0f, -1.0f,   0.0f, 0.0f,
	 2.0f, 3.0f, -2.0f,   0.0f, 0.0f, -1.0f,   0.0f, 1.0f,
	 -4.0f, 0.0f, -2.0f,  0.0f, 0.0f, -1.0f,   1.0f, 0.0f,
	};

	GLfloat ampTop[] = {

		//Vertext coords      //Normal      /tex coords
		-4.0f,  3.0f,  0.0f,   0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
		 2.0f,  3.0f, -2.0f,   0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
		-4.0f,  3.0f, -2.0f,   0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
		-4.0f,  3.0f,  0.0f,   0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
		 2.0f,  3.0f,  0.0f,   0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
		 2.0f,  3.0f, -2.0f,   0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
		-4.0f,  3.0f,  0.0f,   0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
	};


	GLfloat ampFace[] = {
		//verts             norms                 
		-4.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
		-4.0f, 3.0f, 0.0f,	0.0f, 0.0f, 1.0f,   0.0f, 1.0f,
		 2.0f, 3.0f, 0.0f,	0.0f, 0.0f, 1.0f,   1.0f, 1.0f,

		-4.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
		 2.0f, 3.0f, 0.0f,  0.0f, 0.0f, 1.0f,   1.0f, 1.0f,
		 2.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f,    1.0f, 0.0f,

	};

	// fretboard and headstock vertices
	GLfloat fretBoard[] = {
		0.8f, 2.8f, 0.55f,  0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
		1.15f, 2.8f, 0.55f, 0.0f, 0.0f, -1.0f,  1.0f, 0.0f,
		1.15f, 6.2f, 0.55f, 0.0f, 0.0f, -1.0f,  0.9f, 1.0f,
		0.8f, 2.8f, 0.55f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f,

		0.8f, 6.2f, 0.55f,  0.0f, 0.0f, -1.0f,  0.1f, 1.0f,
		1.15f, 6.2f, 0.55f, 0.0f, 0.0f, -1.0f,   1.0f, 1.0f,
		0.8f, 2.8f, 0.55,   0.0f, 0.0f, -1.0f,  0.0f, 0.0f,


	};

	GLfloat headStock[] = {

		//Bottom part
		0.8f, 6.2f, 0.55f,   0.0f, 0.0f, -1.0f,   0.0f, 0.0f,
		1.15f, 6.2f, 0.55f,  0.0f, 0.0f, -1.0f,   0.0f, 0.0f,
		0.95f, 6.5f, 0.55f,  0.0f, 0.0f, -1.0f,   0.5f, 0.1f,

		0.95f, 6.5f, 0.55f,  0.0f, 0.0f, -1.0f,  0.0f, 1.0f,
		0.6f, 6.5f, 0.55f,   0.0f, 0.0f, -1.0f,  0.0f, 1.0f,
		0.8f, 6.2f, 0.55f,   0.0f, 0.0f, -1.0f,  0.5f, 0.0f,

		1.15f, 6.2f, 0.55f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f,
		1.3f, 6.5f, 0.55f,   0.0f, 0.0f, -1.0f,  0.0f, 1.0f,
		0.6f, 6.5f, 0.55f,   0.0f, 0.0f, -1.0f,  0.5f, 1.0f,

		//Bottom part edges

		0.6f, 6.5f, 0.55f, -0.5f, -0.2f, 0.0f,  0.0f, 0.0f,
		0.6f, 6.5f, 0.4f,  -0.5f, -0.2f, 0.0f,  0.0f, 0.0f,
		0.8f, 6.2f, 0.4f,  -0.5f, -0.2f, 0.0f,  0.0f, 0.0f,

		0.6f, 6.5f, 0.55f, -1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
		0.8f, 6.2f, 0.55f, -1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
		0.8f, 6.2f, 0.4f,  -1.0f, 0.0f, 0.0f,  0.0f, 0.0f,

		0.6f, 6.5f, 0.4f,  -1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
		0.6f, 7.4f, 0.4f,  -1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
		0.6f, 6.5f, 0.55f, -1.0f, 0.0f, 0.0f,  0.0f, 0.0f,

		0.6f, 7.4f, 0.4f,   -1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
		0.6f, 7.4f, 0.55,   -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.6f, 6.5f, 0.55f,  -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,

		0.6f, 7.4f, 0.55f,  0.0f, -1.0f, -0.0f,  0.0f, 0.0f,
		1.3f, 7.4f, 0.55f,  0.0f, -1.0f, -0.0f,  0.0f, 0.0f,
		1.3f, 7.4f, 0.4f,   0.0f, -1.0f, -0.0f, 0.0f, 0.0f,

		0.6f, 7.4f, 0.55f, 0.0f, -1.0f, 0.0f,  0.0f, 0.0f,
		0.6f, 7.4f, 0.4f,  0.0f, -1.0f, 0.0f,  0.0f, 0.0f,
		1.3f, 7.4f, 0.4f,  0.0f, -1.0f, 0.0f,  0.0f, 0.0f,

		1.3f, 7.4f, 0.4f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
		1.3f, 6.5f, 0.4f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
		1.3f, 7.4f, 0.55f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,

		1.3f, 6.5f, 0.4f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
		1.3f, 6.5f, 0.55f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
		1.3f, 7.4f, 0.55f,  1.0f, 0.0f, 0.0f,  0.f, 0.0f,

		1.3f, 6.5f, 0.55f,  1.0f, 0.0f, -1.0f,  1.0f, 0.0f,
		0.6f, 6.5f, 0.55f,  1.0f, 0.0f, -1.0f,  0.0f, 0.0f,
		0.6f, 7.4f, 0.55f,  1.0f, 0.0f, -1.0f,  0.0f, 1.0f,
		1.3f, 6.5f, 0.55f,  1.0f, 0.0f, -1.0f,  1.0f, 0.0f,

		0.6f, 7.4f, 0.55f,   1.0f, 0.0f, -1.0f,  0.0f, 1.0f,
		1.3f, 7.4f, 0.55f,   1.0f, 0.0f, -1.0f,  1.0f, 1.0f,
		1.3f, 6.5f, 0.55f,   1.0f, 0.0f, -1.0f,  1.0f, 0.0f,


		// bottom right side edge
		1.15f, 6.2f, 0.55f, 1.0f, -0.2f, 0.0f,  0.0f, 0.0f,
		1.15f, 6.2f, 0.4f,  1.0f, -0.2f, 0.0f,    0.0f, 0.0f,
		1.3f, 6.5f, 0.55f,  1.0f, -0.2f, 0.0f,    0.0f, 0.0f,

		1.15f, 6.2f, 0.4f,  1.0f, -0.2f, 0.0f,     0.0f, 0.0f,
		1.3f, 6.5f, 0.4f,   1.0f, -0.2f, 0.0f,     0.0f, 0.0f,
		1.3f, 6.5f, 0.55f,  1.0f, -0.2f, 0.0f,     0.0f, 0.0f,


		//back

		1.3f, 6.5f, 0.4f, 0.0f, 0.0f, -1.0f,  0.0f, 0.0f,
		0.6f, 6.5f, 0.4f, 0.0f, 0.0f, -1.0f,  0.0f, 0.0f,
		0.6f, 7.4f, 0.4f, 0.0f, 0.0f, -1.0f,  0.0f, 0.0f,

		0.6f, 7.4f, 0.4f, 0.0f, 0.0f, -1.0f,   0.0f, 0.0f,
		1.3f, 7.4f, 0.4f, 0.0f, 0.0f, -1.0f,   0.0f, 0.0f,
		1.3f, 6.5f, 0.4f, 0.0f, 0.0f, -1.0f,   0.0f, 0.0f,
	};

	GLfloat pickTriangles[] = {

		-0.1f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
		0.05f, 0.1f, 1.0f, 0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
		0.05, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
	};

	GLfloat planeVerts[] = {

		-12.0f, -5.5f,  12.0f,  0.0f,  -1.0f, 0.0f,  0.0f, 0.0f,
		-12.0f, -5.5f, -12.0f,  0.0f,  -1.0f, 0.0f,  0.0f, 1.0f,
		 12.0f, -5.5f, -12.0f,  0.0f,  -1.0f, 0.0f,  1.0f, 1.0f,
		-12.0f, -5.5f,  12.0f,  0.0f,  -1.0f, 0.0f,  0.0f, 0.0f,
		 12.0f, -5.5f, -12.0f,  0.0f,  -1.0f, 0.0f,  1.0f, 1.0f,
		 12.0f, -5.5f,  12.0f,  0.0f,  -1.0f, 0.0f,  1.0f, 0.0f,
		-12.0f, -5.5f,  12.0f,  0.0f,  -1.0f, 0.0f,  0.0f, 0.0f,

	};

	glm::vec3 pointLightPositions[] = {
	  /* glm::vec3(2.0f, 0.0f, 5.0f),
	   glm::vec3(-6.0f, 0.0f, 5.0f),
	   glm::vec3(-2.0f, 0.0f, 3.0f),
	   glm::vec3(2.0f, 0.0f, 2.0f),
	   */
		glm::vec3(0.7f,  0.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -1.0f),
		glm::vec3(-4.0f,  2.0f, -6.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
	};

	GLfloat lightCube[] = {
		// left side 

			-4.0f, 0.0f, 0.0f,
			-4.0f, 3.0f, 0.0f,
			 2.0f,  0.0f, 0.0f,

			 -4.0f, 3.0f, 0.0f,
			 2.0f, 3.0f, 0.0f,
			 2.0f, 0.0f, 0.0f,

			-4.0f, 0.0f, -2.0f,
			-4.0f, 3.0f, -2.0f,
			-4.0f, 0.0f, 0.0f,

			-4.0f, 3.0f, -2.0f,
			-4.0f, 3.0f, 0.0f,
			-4.0f, 0.0f, 0.0f,

			// amp bottom
			-4.0f, 0.0f, -2.0f,
			2.0f, 0.0f, -2.0f,
			-4.0f, 0.0f, 0.0f,

			2.0f, 0.0f, -2.0f,
			2.0f, 0.0f, 0.0f,
			-4.0f, 0.0f, 0.0f,

			// amp right side
			2.0f, 0.0f, -2.0f,
			2.0f, 3.0f, -2.0f,
			2.0f, 0.0f, 0.0f,

			2.0f, 0.0f, 0.0f,
			2.0f, 3.0f, 0.0f,
			2.0f, 3.0f, -2.0f,

			// amp back
			-4.0f, 0.0f, -2.0f,
			-4.0f, 3.0f, -2.0f,
			2.0f, 3.0f, -2.0f,
			-4.0f, 0.0f, -2.0f,
			2.0f, 0.0f, -2.0f,
			2.0f, 3.0f, -2.0f,
			-4.0f, 0.0f, -2.0f,
	};

	unsigned int VBO, VAO;
	unsigned int VBO2, VAO2;
	unsigned int VBO3, VAO3;
	unsigned int VBO4, VAO4;
	unsigned int VBO5, VAO5;
	unsigned int VBO6, VAO6;
	unsigned int VBO7, VAO7;
	unsigned int VBO8, VAO8;
	unsigned int VBO9, VAO9;
	unsigned int VBO10, VAO10;
	unsigned int VBO11, VAO11;
	unsigned int VBO12, VAO12;
	unsigned int VBO13, VAO13;
	unsigned int VBO14, VAO14;
	unsigned int VBOLightCube, VAOLightCube;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);


	// Amp top with controls texture 7
	glGenVertexArrays(1, &VAO8);
	glGenBuffers(1, &VBO8);
	glBindVertexArray(VAO8);
	glBindBuffer(GL_ARRAY_BUFFER, VBO8);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ampTop), ampTop, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// Amp top with controls texture amp face
	glGenVertexArrays(1, &VAO9);
	glGenBuffers(1, &VBO9);
	glBindVertexArray(VAO9);
	glBindBuffer(GL_ARRAY_BUFFER, VBO9);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ampFace), ampFace, GL_STATIC_DRAW);


	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glGenVertexArrays(1, &VAO10);
	glGenBuffers(1, &VBO10);
	glBindVertexArray(VAO10);
	glBindBuffer(GL_ARRAY_BUFFER, VBO10);
	glBufferData(GL_ARRAY_BUFFER, sizeof(fretBoard), fretBoard, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glGenVertexArrays(1, &VAO11);
	glGenBuffers(1, &VBO11);
	glBindVertexArray(VAO11);
	glBindBuffer(GL_ARRAY_BUFFER, VBO11);
	glBufferData(GL_ARRAY_BUFFER, sizeof(headStock), headStock, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));


	glEnableVertexAttribArray(2);
	glGenVertexArrays(1, &VAO4);
	glGenBuffers(1, &VBO4);
	glBindVertexArray(VAO4);
	glBindBuffer(GL_ARRAY_BUFFER, VBO4);

	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVerts), planeVerts, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);


	glGenVertexArrays(1, &VAOLightCube);
	glGenBuffers(1, &VBOLightCube);
	glBindVertexArray(VAOLightCube);
	glBindBuffer(GL_ARRAY_BUFFER, VBOLightCube);

	glBufferData(GL_ARRAY_BUFFER, sizeof(lightCube), lightCube, GL_STATIC_DRAW);


	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glGenVertexArrays(1, &VAO2);
	glGenBuffers(1, &VBO2);
	glBindVertexArray(VAO2);
	glBindBuffer(GL_ARRAY_BUFFER, VBO2);

	glGenVertexArrays(1, &VAO3);
	glGenBuffers(1, &VBO3);
	glBindVertexArray(VAO3);
	glBindBuffer(GL_ARRAY_BUFFER, VBO3);

	glGenVertexArrays(1, &VAO5);
	glGenBuffers(1, &VBO5);
	glBindVertexArray(VAO5);
	glBindBuffer(GL_ARRAY_BUFFER, VBO5);

	glGenVertexArrays(1, &VAO6);
	glGenBuffers(1, &VBO6);
	glBindVertexArray(VAO6);
	glBindBuffer(GL_ARRAY_BUFFER, VBO6);

	glGenVertexArrays(1, &VAO7);
	glGenBuffers(1, &VBO7);
	glBindVertexArray(VAO7);
	glBindBuffer(GL_ARRAY_BUFFER, VBO7);

	glGenVertexArrays(1, &VAO12);
	glGenBuffers(1, &VBO12);
	glBindVertexArray(VAO12);
	glBindBuffer(GL_ARRAY_BUFFER, VBO12);

	glGenVertexArrays(1, &VAO13);
	glGenBuffers(1, &VBO13);
	glBindVertexArray(VAO13);
	glBindBuffer(GL_ARRAY_BUFFER, VBO13);

	// load and create a texture 
	// -------------------------
	/*unsigned int texture1, texture2, texture3, texture4, texture5,
		texture6, texture7, texture8, textureFretboard, textureHeadstock,
		textureSoundHole, texturePick, texturePlane;
*/




// tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
// -------------------------------------------------------------------------------------------

	unsigned int ampDiffuseMap = loadTexture("images/ampFaceDark.jpg");
	unsigned int ampSpecularMap = loadTexture("images/ampFaceDark.jpg");
	unsigned int ampSkinTexture = loadTexture("images/ampSkin.jpg");
	unsigned int sodaBodyTexture = loadTexture("images/clubSodaLabel2.jpg");
	unsigned int sodaDomeTexture = loadTexture("images/bottleDome.jpg");
	unsigned int bottleCapTexture = loadTexture("images/bottleCap.png");
	unsigned int guitarTexture = loadTexture("images/GuitarTexture.jpg");
	unsigned int ashPlaneTexture = loadTexture("images/ash.jpg");
	unsigned int ampPanelTexture = loadTexture("images/ampPanel.jpg");
	unsigned int fretBoardTexture = loadTexture("images/fretBoard.png");
	unsigned int headStockTexture = loadTexture("images/headStock.png");
	unsigned int ampFaceTexture = loadTexture("images/ampFace.jpg");
	unsigned int soundHoleTexture = loadTexture("images/screen.jpg");
	unsigned int pickTexture = loadTexture("images/tea_tin.jpg");


	unsigned int guitarTextureDiffuseMap = guitarTexture;
	unsigned int guitarSpecMap = loadTexture("images/GuitarTextureDark.jpg");

	lightingShader.use();
	lightingShader.setInt("material.diffuse", 0);
	lightingShader.setInt("material.specular", 1);

	glm::mat4 model;
	float angle;


	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;


		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		lightingShader.use();
		lightingShader.setVec3("dirLight.direction", -cameraFront);
		lightingShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
		lightingShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
		// point light 1
		lightingShader.setVec3("pointLights[0].position", pointLightPositions[0]);
		lightingShader.setVec3("pointLights[0].ambient", .5f, 0.5f, 0.5f);
		lightingShader.setVec3("pointLights[0].diffuse", 1.0f, 1.0f, 0.8f);
		lightingShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("pointLights[0].constant", 2.0f);
		lightingShader.setFloat("pointLights[0].linear", 0.9f);
		lightingShader.setFloat("pointLights[0].quadratic", 0.032f);
		
		//point light 2
		lightingShader.setVec3("pointLights[1].position", pointLightPositions[1]);
		lightingShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
		lightingShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("pointLights[1].constant", 0.0f);
		lightingShader.setFloat("pointLights[1].linear", 0.09f);
		lightingShader.setFloat("pointLights[1].quadratic", 0.032f);
		// point light 3
		lightingShader.setVec3("pointLights[2].position", pointLightPositions[2]);
		lightingShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
		lightingShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("pointLights[2].constant", 1.0f);
		lightingShader.setFloat("pointLights[2].linear", 0.09f);
		lightingShader.setFloat("pointLights[2].quadratic", 0.032f);
		// point light 4
		lightingShader.setVec3("pointLights[3].position", pointLightPositions[3]);
		lightingShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
		lightingShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("pointLights[3].constant", 1.0f);
		lightingShader.setFloat("pointLights[3].linear", 0.09f);
		lightingShader.setFloat("pointLights[3].quadratic", 0.032f);
		// spotLight
		lightingShader.setVec3("spotLight.position", lightPos);
		lightingShader.setVec3("spotLight.direction", -cameraFront + glm::vec3(0.0f, 0.0f, -8.0f));
		lightingShader.setVec3("spotLight.ambient", 1.0f, 1.0f, 1.0f);
		lightingShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
		lightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("spotLight.constant", 1.0f);
		lightingShader.setFloat("spotLight.linear", 0.09f);
		lightingShader.setFloat("spotLight.quadratic", 0.032f);
		lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
		lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

		lightingShader.setFloat("material.shininess", 32.0);
		// bind textures on corresponding texture units
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, ampSkinTexture);
		//glActiveTexture(GL_TEXTURE1);
		//glBindTexture(GL_TEXTURE_2D, texture2);

		// activate shader


		// pass projection matrix to shader (note that in this case it could change every frame)

		lightingShader.setMat4("projection", projection);


		// camera/view transformationX
		const float radius = 15.0f;
		float camX = sin(glfwGetTime()) * radius;
		float camZ = cos(glfwGetTime()) * radius;

		//glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		//glm::mat4 view = glm::lookAt(glm::vec3(camX, 0.1f, camZ), glm::vec3(0.0f, 5.0f, 0.0f), cameraUp);
		glm::mat4 view = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), cameraUp);
		lightingShader.setMat4("view", view);

		// render boxes
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, ampSkinTexture);


		glBindVertexArray(VAO);
		// calculate the model matrix for each object and pass it to shader before drawing
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
		//model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		angle = 0.0f;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));

		lightingShader.setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, ampPanelTexture);

		// top of amp
		glBindVertexArray(VAO8);
		// calculate the model matrix for each object and pass it to shader before drawing
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		angle = 0.0f;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
		lightingShader.setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		//amp face




		lightingShader.setMat4("projection", projection);
		lightingShader.setMat4("view", view);
		lightingShader.setMat4("model", model);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, ampFaceTexture);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, ampDiffuseMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, ampSpecularMap);
		glBindVertexArray(VAO9);
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
		// calculate the model matrix for each object and pass it to shader before drawing
		//model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
		model = glm::translate(model, glm::vec3(-5.0f, 0.0f, 0.0f));
		angle = 0.0f;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 0.0f, 0.0f));



		glDrawArrays(GL_TRIANGLES, 0, 6);

		//guitar body
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, guitarTexture);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, guitarTextureDiffuseMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, guitarSpecMap);
		glBindVertexArray(VAO2);
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
		model = glm::translate(model, glm::vec3(1.0f, 1.0f, 0.8f));
		angle = 80.0f;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f));
		lightingShader.setMat4("model", model);

		static_meshes_3D::Cylinder LowerBody(1.0, 360, 0.7, true, false, true);
		LowerBody.render();

		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.053f));
		static_meshes_3D::Cylinder UpperBody(0.75, 360, 0.7f, true, false, true);
		lightingShader.setMat4("model", model);
		UpperBody.render();


		//Soda bottle body
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, sodaBodyTexture);
		glBindVertexArray(VAO5);
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
		model = glm::translate(model, glm::vec3(-3.0f, 3.8f, -0.75f));
		angle = 0.0f;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f));
		lightingShader.setMat4("model", model);

		static_meshes_3D::Cylinder C3(0.25, 360, 1.5, true, true, true);
		C3.render();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, sodaDomeTexture);

		glBindVertexArray(VAO3);
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
		model = glm::translate(model, glm::vec3(-3.0f, 4.6f, -0.75f));
		angle = 95.0f;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f));
		lightingShader.setMat4("model", model);

		//static_meshes_3D::Cylinder C2(2, 10, 1, true, true, true);
		//C2.render();

		Sphere S(0.25, 10, 10);
		S.Draw();

		// Soda bottle cap
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, bottleCapTexture);
		glBindVertexArray(VAO6);
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
		model = glm::translate(model, glm::vec3(-3.0f, 4.85f, -0.75f));
		angle = 0.0f;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f));
		lightingShader.setMat4("model", model);

		static_meshes_3D::Cylinder BottleCap(0.1, 360, 0.125, true, true, true);
		BottleCap.render();



		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, ashPlaneTexture);

		glBindVertexArray(VAO4);
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
		model = glm::translate(model, glm::vec3(.0f, 5.41f, 0.0f));
		lightingShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);



		// Guitar Neck is a half tube
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, guitarTexture);
		glBindVertexArray(VAO7);
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
		model = glm::translate(model, glm::vec3(1.0f, 4.3f, 0.57f));
		angle = 180.0f;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
		angle = 10.0f;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f));
		lightingShader.setMat4("model", model);

		static_meshes_3D::HalfTube ht1(0.15, 360, 3.5, true, true, true);
		ht1.render();

		//FRETBOARD
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, fretBoardTexture);


		glBindVertexArray(VAO10);
		// calculate the model matrix for each object and pass it to shader before drawing
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
		model = glm::translate(model, glm::vec3(0.0f, -0.2f, 0.81f));
		angle = -10.0f;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f));
		lightingShader.setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		//headstock
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, guitarTexture);
		glBindVertexArray(VAO11);
		// calculate the model matrix for each object and pass it to shader before drawing
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
		model = glm::translate(model, glm::vec3(0.0f, -0.2f, 0.85f));
		angle = -10.0f;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f));
		lightingShader.setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 72);

		// soundhole
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, soundHoleTexture);
		glBindVertexArray(VAO12);
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
		model = glm::translate(model, glm::vec3(0.98f, 2.2f, 0.95f));
		angle = 80.0f;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f));
		lightingShader.setMat4("model", model);

		static_meshes_3D::Cylinder SoundHole(0.3, 360, 0.0, true, true, true);
		SoundHole.render();

		// soundhole
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, pickTexture);
		glBindVertexArray(VAO13);
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
		//model = glm::translate(model, glm::vec3(1.1f, 5.5f, 0.4f));
		angle = 90.0f;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f));
		lightingShader.setMat4("model", model);

		static_meshes_3D::Cylinder Pick(0.05, 360, 0.0, true, true, true);
		Pick.render();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, guitarTexture);

		lightCubeShader.use();
		lightCubeShader.setMat4("projection", projection);
		lightCubeShader.setMat4("view", view);


		glBindVertexArray(VAOLightCube);
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
		model = glm::translate(model, lightPos);
		angle = 0.0f;

		model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));

		lightCubeShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glDeleteVertexArrays(1, &VAO2);
	glDeleteBuffers(1, &VBO2);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	float cameraSpeed = 3.5 * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		cameraPos += cameraUp * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		cameraPos -= cameraUp * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		if (projection == glm::ortho(-30.0f, 30.0f, -35.0f, 100.0f, -1000.0f, 1000.0f)) {
			projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		}
		else if (projection == glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f)) {
			projection = glm::ortho(-30.0f, 30.0f, -35.0f, 100.0f, -1000.0f, 1000.0f);
		}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		lightPos.x -= 0.1f;
	}

	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		lightPos.x += 0.1f;
	}

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		lightPos.y += 0.1f;
	}

	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		lightPos.y -= 0.1f;
	}

	if (glfwGetKey(window, GLFW_KEY_PERIOD) == GLFW_PRESS) {
		lightPos.z -= 0.1f;
	}

	if (glfwGetKey(window, GLFW_KEY_COMMA) == GLFW_PRESS) {
		lightPos.z += 0.1f;
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f; // change this value to your liking
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	fov -= (float)yoffset;
	if (fov < 1.0f)
		fov = 1.0f;
	if (fov > 45.0f)
		fov = 45.0f;
}


// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}