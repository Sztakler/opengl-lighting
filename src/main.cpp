#include<iostream>
#include<math.h>

#include "includes.h"

#include "shader.h"
#include "vao.h"
#include "vbo.h"
#include "ebo.h"
#include "player.h"
#include "camera.h"
#include "sphere.h"
#include "enemies.h"
#include "box.h"

#include <GLFW/glfw3.h>

// Vertices coordinates
GLfloat vertices[] =
{
	-0.5f, -0.5f, 0.0f,
	 0.5f, -0.5f, 0.0f,
	 0.0f,  0.0f, 0.0f
};

void windowResizeHandler(GLFWwindow* window, int width, int height)
{
	const float aspectRatio = (float)width / height;
	float sx = aspectRatio > 1.0f ? aspectRatio : 1.0f;
	float sy = aspectRatio > 1.0f ? 1.0f : 1.0f/aspectRatio;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-sx, sx, -sy, sy, 1, -1);

	glViewport(0, 0, width, height);
}

bool first_mouse_movement = true;
float delta_time;

int SCR_WIDTH = 1920;
int SCR_HEIGHT = 1080;

float last_x = SCR_WIDTH / 2;
float last_y = SCR_HEIGHT / 2;

float fov = 45.0f;

float pitch = 0.0f;
float yaw = -90.0f;

DRAWING_MODE drawing_mode = TRIANGLES;
 
Camera player_camera(glm::vec3(0.0f, 0.0f, 0.0f));
Camera static_camera(glm::vec3(-2.0f, 2.0f,  0.0f));

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
	SCR_WIDTH = width;
	SCR_HEIGHT = height;
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow *window, double x_pos, double y_pos)
{
	if (first_mouse_movement)
	{
		last_x = x_pos;
		last_y = y_pos;
		first_mouse_movement = false;
	}

	float x_offset = x_pos - last_x;
	float y_offset = last_y - y_pos;

	last_x = x_pos;
	last_y = y_pos;

	player_camera.processMouseMovement(x_offset, y_offset); 
}

void scroll_callback(GLFWwindow *window, double x_offset, double y_offset)
{
	player_camera.processMouseScroll(y_offset);
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		player_camera.processKeyboard(FORWARD, delta_time);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		player_camera.processKeyboard(BACKWARD, delta_time);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		player_camera.processKeyboard(LEFT, delta_time);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		player_camera.processKeyboard(RIGHT, delta_time);


	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		player_camera.processMouseMovement(0.0, 10.0);
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		player_camera.processMouseMovement(0.0, -10.0);
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		player_camera.processMouseMovement(-10.0, 0.0);
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		player_camera.processMouseMovement(10.0, 0.0);
	

	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
	{
		if (drawing_mode == TRIANGLES)
			drawing_mode = WIREFRAME;
		else
			drawing_mode = TRIANGLES;
	}
}



int main(int argc, char* argv[])
{
	int seed;
	int board_size;

	if (argc < 2)
	{
		seed = 1;
		board_size = 10;
	}
	else if (argc == 2)
	{
		seed = atoi(argv[1]);
		board_size = 10;
	}
	else
	{
		seed = atoi(argv[1]);
		board_size = atoi(argv[2]);
	}

    // Initialize GLFW
   glewExperimental = true; // Needed for core profile
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);               // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGl


    // Create a GLFWwindow object of 800 by 800 pixels, naming it "YoutubeOpenGL"
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "framework", NULL, NULL);
	if (window == NULL)
	{
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	glfwSetWindowSizeCallback(window, framebuffer_size_callback);
	glfwMakeContextCurrent(window);


	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  // Hides mouse cursor and captures it
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	if (glewInit() != GLEW_OK)
	{
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}  



	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the player_camera than the former one
	glDepthFunc(GL_LESS);




	Player player("data/vertices_player.txt", "data/colors_player.txt", "shaders/player.vsh", "shaders/player.fsh");
	Sphere sphere(0.2f, 36, 18);

	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	glm::vec3 direction;

	float rotation = 0.0f;
	double prev_time = glfwGetTime();

	// float deltaTime = 0.0f;	// Time between current frame and last frame
	float lastFrame = 0.0f; // Time of last frame
	
	player_camera.front = glm::vec3(0.0, 0.0, 0.0);

	glm::vec3 lightColor(1.0f, 1.0f, 1.0f);	
	glm::vec3 lightPosition(2.0, 0.5, 2.0);

	sphere.position = lightPosition;

	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0)
	{	
		float currentFrame = glfwGetTime();
		delta_time = currentFrame - lastFrame;
		lastFrame = currentFrame;  

		processInput(window);
		// lightPosition = sphere.position;

		lightPosition.y = cos(currentFrame) + 0.5;
		lightPosition.x = cos(currentFrame) + 0.5;
		lightPosition.z = sin(currentFrame) + 0.3;

		sphere.position = lightPosition;




		std::cout << "Light position: (" << lightPosition.x << ", " << lightPosition.y << ", " << lightPosition.z << ")\n";
		std::cout << "Current frame: " << currentFrame << "\n";
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);


		view = player_camera.getViewMatrix();
		projection = glm::perspective(glm::radians(player_camera.zoom), (float)(SCR_WIDTH) / (float)SCR_HEIGHT, 0.01f, 100.0f);
		
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

		
		player.shader.Activate();
		player.Bind();
		glUniform3fv(3, 1, glm::value_ptr(lightColor));
		glUniform3fv(4, 1, glm::value_ptr(sphere.position));
		glUniform3fv(5, 1, glm::value_ptr(player_camera.position));

		player.Draw(&model, &view, &projection, drawing_mode);
		player.Unbind();

		sphere.shader.Activate();
		sphere.Bind();
		sphere.Draw(&model, &view, &projection, drawing_mode);
		sphere.Unbind();

		// sphere.position = player_camera.position;


		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();
	}


	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();
	return 0;
}
