// Std. Includes
#include <string>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GL includes
#include "RenderFunctions.h"
#include "Functions.h"
#include "Shader.h"
#include "Camera.h"
#include "Model.h"


// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Properties
GLuint screenWidth = 800, screenHeight = 600;

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void Do_Movement();

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

glm::vec3 lightPos(1.2f, 0.5f, 2.0f);
float elapsedTime = 0.0f;
unsigned int fps = 0;
void onUpdate(float deltaTime);

// The MAIN function, from here we start our application and run our Game loop
int main()
{
	// Init GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "LearnOpenGL", nullptr, nullptr); // Windowed
	glfwMakeContextCurrent(window);

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// Options
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Initialize GLEW to setup the OpenGL Function pointers
	glewExperimental = GL_TRUE;
	glewInit();

	// Define the viewport dimensions
	glViewport(0, 0, screenWidth, screenHeight);

	// Setup some OpenGL options
	glEnable(GL_DEPTH_TEST);

	float skyboxVertices[] = {
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f
	};

	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	std::vector<std::string> faces =
	{
		"skyboxes/skybox/right.jpg",
		"skyboxes/skybox/left.jpg",
		"skyboxes/skybox/top.jpg",
		"skyboxes/skybox/bottom.jpg",
		"skyboxes/skybox/front.jpg",
		"skyboxes/skybox/back.jpg"
	};
	unsigned int skyboxTexture = Functions::loadCubemap(faces);
	Shader skyboxShader("shaders/4n6/skybox.vs", "shaders/4n6/skybox.fs");

	glm::vec3 lightPositions[] = {
		glm::vec3(-10.0f,  10.0f, 10.0f),
		glm::vec3(10.0f,  10.0f, 10.0f),
		glm::vec3(-10.0f, -10.0f, 10.0f),
		glm::vec3(10.0f, -10.0f, 10.0f),
	};
	glm::vec3 lightColors[] = {
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f)
	};
	int nrRows = 7;
	int nrColumns = 7;
	float spacing = 2.5;

	/*
	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	unsigned int depthCubemap = Functions::createDepthCubemap(SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	Shader depthShader("shaders/5n4/shadow.vs", "shaders/5n4/shadow.fs", "shaders/5n4/shadow.gs");
	*/
	//Shader shaderModel("shaders/5n4/model.vs", "shaders/5n4/model.fs");
	//Shader shaderModel2("shaders/5n4/model.vs", "shaders/5n4/model.fs");
	Shader pbrShader("shaders/6n2/modelPBR.vs", "shaders/6n2/modelPBR.fs");
	unsigned int albedo = Functions::loadTexture("materials/rustedIron/basecolor.png");
	unsigned int normal = Functions::loadTexture("materials/rustedIron/normal.png");
	unsigned int metallic = Functions::loadTexture("materials/rustedIron/metallic.png");
	unsigned int roughness = Functions::loadTexture("materials/rustedIron/roughness.png");


	//Model object("models/obiwan/0.obj");
	//Model model2("models/hatka/hatka_local_.obj");

	skyboxShader.Use();
	skyboxShader.setInt("skybox", 0);

	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
	pbrShader.Use();
	pbrShader.setMat4("projection", projection);
	pbrShader.setVec3f("albedo", 0.5f, 0.0f, 0.0f);
	pbrShader.setFloat("ao", 1.0f);
	pbrShader.setInt("albedoMap", 0);
	pbrShader.setInt("normalMap", 1);
	pbrShader.setInt("metallicMap", 2);
	pbrShader.setInt("roughnessMap", 3);
	// Game loop
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	//glFrontFace(GL_CCW);

	//glm::vec3 lightPos(0.0f, 0.0f, 0.0f);

	/*float aspect = static_cast<float>(SHADOW_WIDTH)/SHADOW_HEIGHT;
	float near = 1.0f;
	float far = 25.0f;
	glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, near, far);*/

	while (!glfwWindowShouldClose(window))
	{
		// Set frame time
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		onUpdate(deltaTime);

		// Check and call events
		glfwPollEvents();
		Do_Movement();

		//lightPos.z = sin(glfwGetTime() * 0.5) * 3.0;
		// Clear the colorbuffer
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		/*
		std::vector<glm::mat4> shadowTransforms;
		shadowTransforms.reserve(6);
		shadowTransforms.push_back(shadowProj*
			glm::lookAt(lightPos, lightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(shadowProj*
			glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(shadowProj*
			glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
		shadowTransforms.push_back(shadowProj*
			glm::lookAt(lightPos, lightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
		shadowTransforms.push_back(shadowProj*
			glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(shadowProj*
			glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

		{
			glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
			glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
			glClear(GL_DEPTH_BUFFER_BIT);
			depthShader.Use();
			for (unsigned int i = 0; i < 6; ++i)
			{
				std::string shadowMatr = "shadowMatrices[" + std::to_string(i) + "]";
				depthShader.setMat4(shadowMatr.c_str(), shadowTransforms[i]);
			}
			depthShader.setFloat("farPlane", far);
			depthShader.setVec3f("lightPos", lightPos);
			glm::mat4 modelMat = glm::mat4(1.0f);
			modelMat = glm::scale(modelMat, glm::vec3(1.f, 1.f, 1.f));
			shaderModel2.setMat4("model", modelMat);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		glViewport(0, 0, screenWidth, screenHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		*/
		
		{
			glDepthMask(GL_FALSE);
			skyboxShader.Use();

			glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
			skyboxShader.setMat4("view", view);
			skyboxShader.setMat4("projection", projection);
			glBindVertexArray(skyboxVAO);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);
			glDepthMask(GL_TRUE);
		}

		{
			pbrShader.Use();
			glm::mat4 view = camera.GetViewMatrix();
			pbrShader.setMat4("view", view);
			pbrShader.setVec3f("camPos", camera.Position);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, albedo);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, normal);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, metallic);
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, roughness);

			glm::mat4 model = glm::mat4(1.0f);
			for (int row = 0; row < nrRows; ++row)
			{
				pbrShader.setFloat("metallic", static_cast<float>(row)/ nrRows);
				for (int col = 0; col < nrColumns; ++col)
				{
					pbrShader.setFloat("roughness", glm::clamp(static_cast<float>(col)/ nrColumns, 0.05f, 1.0f));

					model = glm::mat4(1.0f);
					model = glm::translate(model, glm::vec3(
						(col - (nrColumns / 2)) * spacing,
						(row - (nrRows / 2)) * spacing,
						0.0f
					));
					pbrShader.setMat4("model", model);
					RenderFunctions::RenderSphere();
				}
			}

			for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
			{
				glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
				newPos = lightPositions[i];
				std::string lightPosStr = "lightPositions[" + std::to_string(i) + "]";
				pbrShader.setVec3f(lightPosStr.c_str(), newPos);
				std::string lightColStr = "lightColors[" + std::to_string(i) + "]";
				pbrShader.setVec3f(lightColStr.c_str(), lightColors[i]);

				model = glm::mat4(1.0f);
				model = glm::translate(model, newPos);
				model = glm::scale(model, glm::vec3(0.5f));
				pbrShader.setMat4("model", model);
				RenderFunctions::RenderSphere();
			}
		}


		/*
		{
			shaderModel.Use();
			glm::mat4 view = camera.GetViewMatrix();
			shaderModel.setMat4("projection", projection);
			shaderModel.setMat4("view", view);

		
			glm::mat4 modelMat = glm::mat4(1.0f);
			modelMat = glm::translate(modelMat, glm::vec3(2.5f, 0.0f, -1.0f));
			modelMat = glm::scale(modelMat, glm::vec3(0.5f, 0.5f, 0.5f));
			shaderModel.setMat4("model", modelMat);
			shaderModel.setVec3f("viewPos", camera.Position);
			shaderModel.setVec3f("dirLight.position", lightPos);
			shaderModel.setVec3f("dirLight.direction", -1.0f, -1.0f, -1.0f);
			shaderModel.setVec3f("dirLight.ambient", 0.2f, 0.2f, 0.2f);
			shaderModel.setVec3f("dirLight.diffuse", 0.5f, 0.5f, 0.5f);
			shaderModel.setVec3f("dirLight.specular", 1.0f, 1.0f, 1.0f);
			shaderModel.setFloat("material.shininess", 4.0f);
			shaderModel.setVec3f("spotLight.position", camera.Position);
			shaderModel.setVec3f("spotLight.direction", camera.Front);
			shaderModel.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
			shaderModel.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));
			shaderModel.setVec3f("spotLight.ambient", 0.1f, 0.1f, 0.1f);
			shaderModel.setVec3f("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
			shaderModel.setVec3f("spotLight.specular", 1.0f, 1.0f, 1.0f);
			shaderModel.setInt("depthMap", depthCubemap);
			shaderModel.setFloat("farPlane", far);
			model.Draw(&shaderModel);
		}*/
		/*
		{
			shaderModel2.Use();
			glm::mat4 view = camera.GetViewMatrix();
			shaderModel2.setMat4("projection", projection);
			shaderModel2.setMat4("view", view);


			glm::mat4 modelMat = glm::mat4(1.0f);
			modelMat = glm::scale(modelMat, glm::vec3(1.f, 1.f, 1.f));
			shaderModel2.setMat4("model", modelMat);
			shaderModel2.setVec3f("viewPos", camera.Position);
			shaderModel2.setVec3f("dirLight.position", lightPos);
			shaderModel2.setVec3f("dirLight.direction", -1.0f, -1.0f, -1.0f);
			shaderModel2.setVec3f("dirLight.ambient", 0.2f, 0.2f, 0.2f);
			shaderModel2.setVec3f("dirLight.diffuse", 0.5f, 0.5f, 0.5f);
			shaderModel2.setVec3f("dirLight.specular", 1.0f, 1.0f, 1.0f);
			shaderModel2.setFloat("material.shininess", 4.0f);
			shaderModel2.setVec3f("spotLight.position", camera.Position);
			shaderModel2.setVec3f("spotLight.direction", camera.Front);
			shaderModel2.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
			shaderModel2.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));
			shaderModel2.setVec3f("spotLight.ambient", 0.1f, 0.1f, 0.1f);
			shaderModel2.setVec3f("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
			shaderModel2.setVec3f("spotLight.specular", 1.0f, 1.0f, 1.0f);
			shaderModel2.setInt("depthMap", depthCubemap);
			shaderModel2.setFloat("farPlane", far);
			model2.Draw(&shaderModel2);
		}*/


		// Swap the buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	// Properly de-allocate all resources once they've outlived their purpose
	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteBuffers(1, &skyboxVAO);
	glfwTerminate();
	return 0;
}

// Moves/alters the camera positions based on user input
void Do_Movement()
{
	// Camera controls
	if (keys[GLFW_KEY_W])
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S])
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A])
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D])
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	//cout << key << endl;
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

void onUpdate(float deltaTime)
{
	elapsedTime += deltaTime;

	if (elapsedTime >= 1.0f)
	{
		fps = 1.f / deltaTime;
		std::cout << "FPS: "  << fps << std::endl;
		fps = 0;
		elapsedTime = 0.0f;
	}
}


