#include<iostream>
#include<math.h>

#include "includes.h"

#include "shader.h"
#include "vao.h"
#include "vbo.h"
#include "ebo.h"
#include "drawable.h"
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
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		player_camera.processKeyboard(UP, delta_time);
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		player_camera.processKeyboard(DOWN, delta_time);


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
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glFrontFace(GL_CCW);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA);



	Drawable aquarium("data/aquarium_smooth.obj", "shaders/player.vsh", "shaders/player.fsh");
	Drawable cube("data/cube.obj", "shaders/player.vsh", "shaders/player.fsh");
	Drawable plane("data/plane.obj", "shaders/player.vsh", "shaders/player.fsh");
	Drawable bubble("data/sphere.obj", "shaders/player.vsh", "shaders/player.fsh");
	Drawable suzanne("data/suzanne.obj", "shaders/player.vsh", "shaders/player.fsh");
	Drawable wibblywoobly("data/wibbly-woobly.obj", "shaders/player.vsh", "shaders/player.fsh");
	
	std::vector<Drawable*> bubbles;
 
	int n_bubbles = 100;
	for (int i = 0; i < n_bubbles; i++)
	{
		bubbles.push_back(new Drawable("data/sphere.obj", "shaders/player.vsh", "shaders/player.fsh"));
		bubbles[i]->position = glm::vec3(rand() % 2000 / 100.0, rand() % 2000 / 100.0 - 20.0, rand() % 4000 / 100.0);
		bubbles[i]->speed = 0.01 + rand() % 100 / 10000.0;
	}

	Sphere sphere(0.05f, 36, 18);

	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	float rotation = 0.0f;
	double prev_time = glfwGetTime();

	// float deltaTime = 0.0f;	// Time between current frame and last frame
	float lastFrame = 0.0f; // Time of last frame
	
	player_camera.front = glm::vec3(0.0, 0.0, 0.0);

	glm::vec3 lightPosition(0.0, 10.0, 0.0);
	glm::vec3 lightColor(1.0f, 1.0f, 1.0f);	
	

	glm::vec3 aquarium_center(10.0, 10.0, 20.0);

	sphere.position = lightPosition;

	aquarium.position.x += 14.362542;
	aquarium.position.y += 8.999555;
	aquarium.position.z += 19.583765;


	plane.position.y += 8.999555;

	cube.position.x = 0.0f;
	cube.position.y = 1.001f;
	cube.position.z = 0.0f;

	bubble.position.x += 1.0f;
	bubble.position.y += 1.0f;
	bubble.position.z += 4.0f;

	// suzanne.position.x += -2.0f;
	// suzanne.position.y += 1.5f;
	// suzanne.position.z += 3.0f;

	suzanne.position = aquarium_center;

	wibblywoobly.position.x += 0.0;
	wibblywoobly.position.y += 3.0;
	wibblywoobly.position.z += 2.0;

	float light_constant = 0.5;
	float light_linear = 0.09;
	float light_quadratic = 0.2;
	float light_cutOff = 12.5;
	float light_outCutOff = 17.5;

	unsigned int counter = 0; // disables VSync -- unlimited FPS
	
	// glfwSwapInterval(0);

	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0)
	{	
		float currentFrame = glfwGetTime();
		delta_time = currentFrame - lastFrame;
		counter++;

		if (delta_time >= 1.0 / 30.0)
		{
			std::string FPS = std::to_string((1.0 / delta_time) * counter);
			std::string ms = std::to_string((delta_time / counter) * 1000);
			std::string newTitle = "Aquarium - " + FPS + "FPS / " + ms + "ms";
			glfwSetWindowTitle(window, newTitle.c_str());
			counter = 0;
			lastFrame = currentFrame;  
		}

		processInput(window);
		// lightPosition = sphere.position;

		// lightPosition.y = cos(currentFrame) * 10.0 + 10.0;
		// lightPosition.x = cos(currentFrame) - 5.0;
		// lightPosition.z = -sin(currentFrame) * 18.0;
		lightPosition = player_camera.position;
		// sphere.position = lightPosition;


		for (auto bubble : bubbles)
		{
			bubble->position.y += bubble->speed;

			if (bubble->position.y >= 20.0)
			{
				bubble->position = glm::vec3(rand() % 2000 / 100.0, rand() % 2000 / 100.0 - 20.0, rand() % 4000 / 100.0);
				bubble->speed = 0.01 + rand() % 100 / 10000.0;
			}

		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.07f, 0.07f, 0.07f, 1.0f);


		view = player_camera.getViewMatrix();
		projection = glm::perspective(glm::radians(player_camera.zoom), (float)(SCR_WIDTH) / (float)SCR_HEIGHT, 0.01f, 100.0f);
		
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

		aquarium.shader.Activate();
		aquarium.Bind();
		// glUniform3fv(glGetUniformLocation(aquarium.shader.id, "lightColor"), 1, glm::value_ptr(lightColor));
		// glUniform3fv(glGetUniformLocation(aquarium.shader.id, "lightPosition"), 1, glm::value_ptr(lightPosition));
		glUniform3fv(glGetUniformLocation(aquarium.shader.id, "viewerPosition"), 1, glm::value_ptr(player_camera.position));	
		glUniform3fv(glGetUniformLocation(aquarium.shader.id, "centerPosition"), 1, glm::value_ptr(aquarium.position));

		glUniform3f(glGetUniformLocation(aquarium.shader.id, "material.ambient"), 0.0314 / 4, 0.1686 / 4, 0.4275 / 4);
		glUniform3f(glGetUniformLocation(aquarium.shader.id, "material.diffuse"), 0.0314, 0.1686, 0.4275);
		glUniform3f(glGetUniformLocation(aquarium.shader.id, "material.specular"), 0.0314, 0.1686, 0.4275);
		glUniform1f(glGetUniformLocation(aquarium.shader.id, "material.shininess"), 36.0f);

		// glUniform3fv(glGetUniformLocation(aquarium.shader.id, "light.position"), 1, glm::value_ptr(lightPosition));
		// glUniform3fv(glGetUniformLocation(aquarium.shader.id, "light.direction"), 1, glm::value_ptr(player_camera.front));
		// glUniform3f(glGetUniformLocation(aquarium.shader.id, "light.ambient"), 0.2f, 0.2f, 0.2f);
		// glUniform3f(glGetUniformLocation(aquarium.shader.id, "light.diffuse"), 0.5f, 0.5f, 0.5f);
		// glUniform3f(glGetUniformLocation(aquarium.shader.id, "light.specular"), 1.0f, 1.0f, 1.0f);
		// glUniform1f(glGetUniformLocation(aquarium.shader.id, "light.constant"), light_constant);
		// glUniform1f(glGetUniformLocation(aquarium.shader.id, "light.linear"), light_linear);
		// glUniform1f(glGetUniformLocation(aquarium.shader.id, "light.quadratic"), light_quadratic);
		// glUniform1f(glGetUniformLocation(aquarium.shader.id, "light.cutOff"), glm::cos(glm::radians(light_cutOff)));
		// glUniform1f(glGetUniformLocation(aquarium.shader.id, "light.outCutOff"), glm::cos(glm::radians(light_outCutOff)));

		// direction light
			glUniform3fv(glGetUniformLocation(aquarium.shader.id, "directionlight.direction"), 1, glm::value_ptr(glm::vec3(0.0, 30.0, 0.0)));
			glUniform3f(glGetUniformLocation(aquarium.shader.id, "directionlight.ambient"), 0.2f, 0.2f, 0.2f);
			glUniform3f(glGetUniformLocation(aquarium.shader.id, "directionlight.diffuse"), 0.5f, 0.5f, 0.5f);
			glUniform3f(glGetUniformLocation(aquarium.shader.id, "directionlight.specular"), 1.0f, 1.0f, 1.0f);

			// spotlight
			glUniform3fv(glGetUniformLocation(aquarium.shader.id, "spotlight.position"), 1, glm::value_ptr(lightPosition));
			glUniform3fv(glGetUniformLocation(aquarium.shader.id, "spotlight.direction"), 1, glm::value_ptr(player_camera.front));
			glUniform3f(glGetUniformLocation(aquarium.shader.id, "spotlight.ambient"), 0.2f, 0.2f, 0.2f);
			glUniform3f(glGetUniformLocation(aquarium.shader.id, "spotlight.diffuse"), 0.5f, 0.5f, 0.5f);
			glUniform3f(glGetUniformLocation(aquarium.shader.id, "spotlight.specular"), 1.0f, 1.0f, 1.0f);
			glUniform1f(glGetUniformLocation(aquarium.shader.id, "spotlight.constant"), light_constant);
			glUniform1f(glGetUniformLocation(aquarium.shader.id, "spotlight.linear"), light_linear);
			glUniform1f(glGetUniformLocation(aquarium.shader.id, "spotlight.quadratic"), light_quadratic);
			glUniform1f(glGetUniformLocation(aquarium.shader.id, "spotlight.cutOff"), glm::cos(glm::radians(light_cutOff)));
			glUniform1f(glGetUniformLocation(aquarium.shader.id, "spotlight.outCutOff"), glm::cos(glm::radians(light_outCutOff)));

		aquarium.Draw(&model, &view, &projection, drawing_mode, false, player_camera.position);
		aquarium.Unbind();

		plane.shader.Activate();
		plane.Bind();
		// glUniform3fv(glGetUniformLocation(plane.shader.id, "lightColor"), 1, glm::value_ptr(lightColor));
		// glUniform3fv(glGetUniformLocation(plane.shader.id, "lightPosition"), 1, glm::value_ptr(lightPosition));
		glUniform3fv(glGetUniformLocation(plane.shader.id, "viewerPosition"), 1, glm::value_ptr(player_camera.position));	
		glUniform3fv(glGetUniformLocation(plane.shader.id, "centerPosition"), 1, glm::value_ptr(plane.position));

		glUniform3f(glGetUniformLocation(plane.shader.id, "material.ambient"), 0.0, 0.4039, 0.8667);
		glUniform3f(glGetUniformLocation(plane.shader.id, "material.diffuse"), 0.0, 0.4039, 0.8667);
		glUniform3f(glGetUniformLocation(plane.shader.id, "material.specular"), 0.0, 0.4039, 0.8667);
		glUniform1f(glGetUniformLocation(plane.shader.id, "material.shininess"), 36.0f);

		glUniform3fv(glGetUniformLocation(plane.shader.id, "light.position"), 1, glm::value_ptr(lightPosition));
		glUniform3fv(glGetUniformLocation(plane.shader.id, "light.direction"), 1, glm::value_ptr(player_camera.front));
		glUniform3f(glGetUniformLocation(plane.shader.id, "light.ambient"), 0.2f, 0.2f, 0.2f);
		glUniform3f(glGetUniformLocation(plane.shader.id, "light.diffuse"), 0.5f, 0.5f, 0.5f);
		glUniform3f(glGetUniformLocation(plane.shader.id, "light.specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(plane.shader.id, "light.constant"), light_constant);
		glUniform1f(glGetUniformLocation(plane.shader.id, "light.linear"), light_linear);
		glUniform1f(glGetUniformLocation(plane.shader.id, "light.quadratic"), light_quadratic);
		glUniform1f(glGetUniformLocation(plane.shader.id, "light.cutOff"), glm::cos(glm::radians(light_cutOff)));
		glUniform1f(glGetUniformLocation(plane.shader.id, "light.outCutOff"), glm::cos(glm::radians(light_outCutOff)));
		
		glDisable(GL_CULL_FACE);
		plane.Draw(&model, &view, &projection, drawing_mode, false, player_camera.position);
		glDisable(GL_CULL_FACE);
		plane.Unbind();

		for (auto bubble : bubbles)
		{
			bubble->shader.Activate();
			bubble->Bind();
			
			glUniform3fv(glGetUniformLocation(bubble->shader.id, "viewerPosition"), 1, glm::value_ptr(player_camera.position));
			glUniform3fv(glGetUniformLocation(bubble->shader.id, "centerPosition"), 1, glm::value_ptr(bubble->position));

			glUniform3f(glGetUniformLocation(bubble->shader.id, "material.ambient"), 0.2f, 0.2f, 0.2f);
			glUniform3f(glGetUniformLocation(bubble->shader.id, "material.diffuse"), 0.362f, 0.686f, 0.368f);
			glUniform3f(glGetUniformLocation(bubble->shader.id, "material.specular"), 0.5f, 0.5f, 0.5f);
			glUniform1f(glGetUniformLocation(bubble->shader.id, "material.shininess"), 32.0f);

			// direction light
			glUniform3fv(glGetUniformLocation(bubble->shader.id, "directionlight.direction"), 1, glm::value_ptr(glm::vec3(0.0, 30.0, 0.0)));
			glUniform3f(glGetUniformLocation(bubble->shader.id, "directionlight.ambient"), 0.2f, 0.2f, 0.2f);
			glUniform3f(glGetUniformLocation(bubble->shader.id, "directionlight.diffuse"), 0.5f, 0.5f, 0.5f);
			glUniform3f(glGetUniformLocation(bubble->shader.id, "directionlight.specular"), 1.0f, 1.0f, 1.0f);

			// spotlight
			glUniform3fv(glGetUniformLocation(bubble->shader.id, "spotlight.position"), 1, glm::value_ptr(lightPosition));
			glUniform3fv(glGetUniformLocation(bubble->shader.id, "spotlight.direction"), 1, glm::value_ptr(player_camera.front));
			glUniform3f(glGetUniformLocation(bubble->shader.id, "spotlight.ambient"), 0.2f, 0.2f, 0.2f);
			glUniform3f(glGetUniformLocation(bubble->shader.id, "spotlight.diffuse"), 0.5f, 0.5f, 0.5f);
			glUniform3f(glGetUniformLocation(bubble->shader.id, "spotlight.specular"), 1.0f, 1.0f, 1.0f);
			glUniform1f(glGetUniformLocation(bubble->shader.id, "spotlight.constant"), light_constant);
			glUniform1f(glGetUniformLocation(bubble->shader.id, "spotlight.linear"), light_linear);
			glUniform1f(glGetUniformLocation(bubble->shader.id, "spotlight.quadratic"), light_quadratic);
			glUniform1f(glGetUniformLocation(bubble->shader.id, "spotlight.cutOff"), glm::cos(glm::radians(light_cutOff)));
			glUniform1f(glGetUniformLocation(bubble->shader.id, "spotlight.outCutOff"), glm::cos(glm::radians(light_outCutOff)));
			
			glEnable(GL_BLEND);
			bubble->Draw(&model, &view, &projection, drawing_mode, false, player_camera.position);
			glDisable(GL_BLEND);
			bubble->Unbind();
		}

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


/*
#include<iostream>
#include<math.h>

#include "includes.h"

#include "shader.h"
#include "vao.h"
#include "vbo.h"
#include "ebo.h"
#include "drawable.h"
#include "camera.h"
#include "sphere.h"
#include "enemies.h"
#include "box.h"
#include "light.h"

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
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		player_camera.processKeyboard(UP, delta_time);
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		player_camera.processKeyboard(DOWN, delta_time);


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
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glFrontFace(GL_CCW);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA);



	Drawable aquarium("data/aquarium_smooth.obj", "shaders/aquarium.vsh", "shaders/aquarium.fsh");
	Drawable cube("data/cube.obj", "shaders/player.vsh", "shaders/player.fsh");
	Drawable plane("data/plane.obj", "shaders/player.vsh", "shaders/player.fsh");
	Drawable bubble("data/sphere.obj", "shaders/player.vsh", "shaders/player.fsh");
	Drawable suzanne("data/suzanne.obj", "shaders/player.vsh", "shaders/player.fsh");
	Drawable wibblywoobly("data/wibbly-woobly.obj", "shaders/player.vsh", "shaders/player.fsh");

	Light light("sphere.obj", "shaders/light.vsh", "shaders/light.fsh", 
		glm::vec3(0.2, 0.2, 0.2), glm::vec3(0.5, 0.5, 0.5), glm::vec3(1.0, 1.0, 1.0));

	Drawable sun("data/sphere.obj", "shaders/light.vsh", "shaders/light.fsh");
	
	std::vector<Drawable*> bubbles;

	int n_bubbles = 20;
	for (int i = 0; i < n_bubbles; i++)
	{
		bubbles.push_back(new Drawable("data/sphere.obj", "shaders/player.vsh", "shaders/player.fsh"));
		bubbles[i]->position = glm::vec3(1.0, 10.0, 1.0);
	}

	Sphere sphere(0.05f, 36, 18);

	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	float rotation = 0.0f;
	double prev_time = glfwGetTime();

	// float deltaTime = 0.0f;	// Time between current frame and last frame
	float lastFrame = 0.0f; // Time of last frame
	
	player_camera.front = glm::vec3(0.0, 0.0, 0.0);

	glm::vec3 lightPosition(0.0, 10.0, 0.0);
	glm::vec3 lightColor(1.0f, 1.0f, 1.0f);	
	

	glm::vec3 aquarium_center(10.0, 10.0, 20.0);

	sun.position = aquarium_center + glm::vec3(0.0, 50.0, 0.0);

	sphere.position = lightPosition;

	aquarium.position.x += 14.362542;
	aquarium.position.y += 8.999555;
	aquarium.position.z += 19.583765;


	plane.position.y += 8.999555;

	cube.position = aquarium_center;
	cube.position.x = 0.0f;
	cube.position.y = 1.001f;
	cube.position.z = 0.0f;

	// bubble.position = aquarium_center;
	// bubble.position.x += 1.0f;
	// bubble.position.y += 1.0f;
	// bubble.position.z += 4.0f;

	suzanne.position = aquarium_center;
	suzanne.position.x += -2.0f;
	suzanne.position.y += 1.5f;
	suzanne.position.z += 3.0f;

	wibblywoobly.position = aquarium_center;
	wibblywoobly.position.y += 3.0;
	wibblywoobly.position.z += 2.0;

	float light_constant = 0.5;
	float light_linear = 0.09;
	float light_quadratic = 0.2;
	float light_cutOff = 12.5;
	float light_outCutOff = 17.5;

	unsigned int counter = 0; // disables VSync -- unlimited FPS
	
	// glfwSwapInterval(0);

	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0)
	{	
		float currentFrame = glfwGetTime();
		delta_time = currentFrame - lastFrame;
		counter++;

		if (delta_time >= 1.0 / 30.0)
		{
			std::string FPS = std::to_string((1.0 / delta_time) * counter);
			std::string ms = std::to_string((delta_time / counter) * 1000);
			std::string newTitle = "Aquarium - " + FPS + "FPS / " + ms + "ms";
			glfwSetWindowTitle(window, newTitle.c_str());
			counter = 0;
			lastFrame = currentFrame;  
		}

		processInput(window);

		lightPosition = player_camera.position;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.07f, 0.07f, 0.07f, 1.0f);


		view = player_camera.getViewMatrix();
		projection = glm::perspective(glm::radians(player_camera.zoom), (float)(SCR_WIDTH) / (float)SCR_HEIGHT, 0.01f, 100.0f);
		
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		
		
		// cube.shader.Activate();
		// cube.Bind();

		// glUniform3fv(glGetUniformLocation(cube.shader.id, "viewerPosition"), 1, glm::value_ptr(player_camera.position));
		// glUniform3fv(glGetUniformLocation(cube.shader.id, "centerPosition"), 1, glm::value_ptr(cube.position));

		// glUniform3f(glGetUniformLocation(cube.shader.id, "material.ambient"), 0.2f, 0.2f, 0.2f);
		// glUniform3f(glGetUniformLocation(cube.shader.id, "material.diffuse"), 0.362f, 0.686f, 0.368f);
		// glUniform3f(glGetUniformLocation(cube.shader.id, "material.specular"), 0.5f, 0.5f, 0.5f);
		// glUniform1f(glGetUniformLocation(cube.shader.id, "material.shininess"), 32.0f);

		//direct light
		// glUniform3fv(glGetUniformLocation(wibblywoobly.shader.id, "dirLight.direction"), 1, glm::value_ptr(aquarium_center));
		// glUniform3f(glGetUniformLocation(wibblywoobly.shader.id, "dirLight.ambient"), 0.2f, 0.2f, 0.2f);
		// glUniform3f(glGetUniformLocation(wibblywoobly.shader.id, "dirLight.diffuse"), 0.5, 0.5, 0.5);
		// glUniform3f(glGetUniformLocation(wibblywoobly.shader.id, "dirlight.specular"), 1.0f, 1.0f, 1.0f);
		

// spotlight
		// glUniform3fv(glGetUniformLocation(cube.shader.id, "spotLight.position"), 1, glm::value_ptr(lightPosition));
		// glUniform3fv(glGetUniformLocation(cube.shader.id, "spotLight.direction"), 1, glm::value_ptr(player_camera.front));
		// glUniform3f(glGetUniformLocation(cube.shader.id, "spotLight.ambient"), 0.2f, 0.2f, 0.2f);
		// glUniform3f(glGetUniformLocation(cube.shader.id, "spotLight.diffuse"), 0.5f, 0.5f, 0.5f);
		// glUniform3f(glGetUniformLocation(cube.shader.id, "spotLight.specular"), 1.0f, 1.0f, 1.0f);
		// glUniform1f(glGetUniformLocation(cube.shader.id, "spotLight.constant"), light_constant);
		// glUniform1f(glGetUniformLocation(cube.shader.id, "spotLight.linear"), light_linear);
		// glUniform1f(glGetUniformLocation(cube.shader.id, "spotLight.quadratic"), light_quadratic);
		// glUniform1f(glGetUniformLocation(cube.shader.id, "spotLight.cutOff"), glm::cos(glm::radians(light_cutOff)));
		// glUniform1f(glGetUniformLocation(cube.shader.id, "spotLight.outCutOff"), glm::cos(glm::radians(light_outCutOff)));

		// glEnable(GL_BLEND);
		// cube.Draw(&model, &view, &projection, drawing_mode, true, player_camera.position);
		// glDisable(GL_BLEND);
		// cube.Unbind();

		aquarium.shader.Activate();
		aquarium.Bind();
		glUniform3fv(glGetUniformLocation(aquarium.shader.id, "viewerPosition"), 1, glm::value_ptr(player_camera.position));	
		glUniform3fv(glGetUniformLocation(aquarium.shader.id, "centerPosition"), 1, glm::value_ptr(aquarium.position));

		glUniform3f(glGetUniformLocation(aquarium.shader.id, "material.ambient"), 0.0314 / 4, 0.1686 / 4, 0.4275 / 4);
		glUniform3f(glGetUniformLocation(aquarium.shader.id, "material.diffuse"), 0.0314, 0.1686, 0.4275);
		glUniform3f(glGetUniformLocation(aquarium.shader.id, "material.specular"), 0.0314, 0.1686, 0.4275);
		glUniform1f(glGetUniformLocation(aquarium.shader.id, "material.shininess"), 36.0f);

		glUniform3fv(glGetUniformLocation(aquarium.shader.id, "light.position"), 1, glm::value_ptr(lightPosition));
		glUniform3fv(glGetUniformLocation(aquarium.shader.id, "light.direction"), 1, glm::value_ptr(player_camera.front));
		glUniform3f(glGetUniformLocation(aquarium.shader.id, "light.ambient"), 0.2f, 0.2f, 0.2f);
		glUniform3f(glGetUniformLocation(aquarium.shader.id, "light.diffuse"), 0.5f, 0.5f, 0.5f);
		glUniform3f(glGetUniformLocation(aquarium.shader.id, "light.specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(aquarium.shader.id, "light.constant"), light_constant);
		glUniform1f(glGetUniformLocation(aquarium.shader.id, "light.linear"), light_linear);
		glUniform1f(glGetUniformLocation(aquarium.shader.id, "light.quadratic"), light_quadratic);
		glUniform1f(glGetUniformLocation(aquarium.shader.id, "light.cutOff"), glm::cos(glm::radians(light_cutOff)));
		glUniform1f(glGetUniformLocation(aquarium.shader.id, "light.outCutOff"), glm::cos(glm::radians(light_outCutOff)));

		aquarium.Draw(&model, &view, &projection, drawing_mode, false, player_camera.position);
		aquarium.Unbind();

		plane.shader.Activate();
		plane.Bind();
		glUniform3fv(glGetUniformLocation(plane.shader.id, "viewerPosition"), 1, glm::value_ptr(player_camera.position));	
		glUniform3fv(glGetUniformLocation(plane.shader.id, "centerPosition"), 1, glm::value_ptr(plane.position));

		glUniform3f(glGetUniformLocation(plane.shader.id, "material.ambient"), 0.0, 0.4039, 0.8667);
		glUniform3f(glGetUniformLocation(plane.shader.id, "material.diffuse"), 0.0, 0.4039, 0.8667);
		glUniform3f(glGetUniformLocation(plane.shader.id, "material.specular"), 0.0, 0.4039, 0.8667);
		glUniform1f(glGetUniformLocation(plane.shader.id, "material.shininess"), 36.0f);
		
		//direct light
		glUniform3fv(glGetUniformLocation(plane.shader.id, "dirLight.direction"), 1, glm::value_ptr(aquarium_center));
		glUniform3f(glGetUniformLocation(plane.shader.id, "dirLight.ambient"), 0.2f, 0.2f, 0.2f);
		glUniform3f(glGetUniformLocation(plane.shader.id, "dirLight.diffuse"), 0.5, 0.5, 0.5);
		glUniform3f(glGetUniformLocation(plane.shader.id, "dirlight.specular"), 1.0f, 1.0f, 1.0f);
		

		// spotlight
		glUniform3fv(glGetUniformLocation(plane.shader.id, "spotLight.position"), 1, glm::value_ptr(lightPosition));
		glUniform3fv(glGetUniformLocation(plane.shader.id, "spotLight.direction"), 1, glm::value_ptr(player_camera.front));
		glUniform3f(glGetUniformLocation(plane.shader.id, "spotLight.ambient"), 0.2f, 0.2f, 0.2f);
		glUniform3f(glGetUniformLocation(plane.shader.id, "spotLight.diffuse"), 0.5f, 0.5f, 0.5f);
		glUniform3f(glGetUniformLocation(plane.shader.id, "spotLight.specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(plane.shader.id, "spotLight.constant"), light_constant);
		glUniform1f(glGetUniformLocation(plane.shader.id, "spotLight.linear"), light_linear);
		glUniform1f(glGetUniformLocation(plane.shader.id, "spotLight.quadratic"), light_quadratic);
		glUniform1f(glGetUniformLocation(plane.shader.id, "spotLight.cutOff"), glm::cos(glm::radians(light_cutOff)));
		glUniform1f(glGetUniformLocation(plane.shader.id, "spotLight.outCutOff"), glm::cos(glm::radians(light_outCutOff)));
		
		glDisable(GL_CULL_FACE);
		plane.Draw(&model, &view, &projection, drawing_mode, false, player_camera.position);
		glDisable(GL_CULL_FACE);
		plane.Unbind();

		for (int i = 0; i < bubbles.size(); i++)
		{
			bubbles[i]->shader.Activate();
			bubbles[i]->Bind();
			
			glUniform3fv(glGetUniformLocation(bubbles[i]->shader.id, "viewerPosition"), 1, glm::value_ptr(player_camera.position));
			glUniform3fv(glGetUniformLocation(bubbles[i]->shader.id, "centerPosition"), 1, glm::value_ptr(bubbles[i]->position));

			glUniform3f(glGetUniformLocation(bubbles[i]->shader.id, "material.ambient"), 0.2f, 0.2f, 0.2f);
			glUniform3f(glGetUniformLocation(bubbles[i]->shader.id, "material.diffuse"), 0.362f, 0.686f, 0.368f);
			glUniform3f(glGetUniformLocation(bubbles[i]->shader.id, "material.specular"), 0.5f, 0.5f, 0.5f);
			glUniform1f(glGetUniformLocation(bubbles[i]->shader.id, "material.shininess"), 32.0f);

		//direct light
		glUniform3fv(glGetUniformLocation(bubbles[i]->shader.id, "dirLight.direction"), 1, glm::value_ptr(aquarium_center));
		glUniform3f(glGetUniformLocation(bubbles[i]->shader.id, "dirLight.ambient"), 0.2f, 0.2f, 0.2f);
		glUniform3f(glGetUniformLocation(bubbles[i]->shader.id, "dirLight.diffuse"), 0.5, 0.5, 0.5);
		glUniform3f(glGetUniformLocation(bubbles[i]->shader.id, "dirlight.specular"), 1.0f, 1.0f, 1.0f);
		

// spotlight
			glUniform3fv(glGetUniformLocation(bubbles[i]->shader.id, "spotLight.position"), 1, glm::value_ptr(lightPosition));
			glUniform3fv(glGetUniformLocation(bubbles[i]->shader.id, "spotLight.direction"), 1, glm::value_ptr(player_camera.front));
			glUniform3f(glGetUniformLocation(bubbles[i]->shader.id, "spotLight.ambient"), 0.2f, 0.2f, 0.2f);
			glUniform3f(glGetUniformLocation(bubbles[i]->shader.id, "spotLight.diffuse"), 0.5f, 0.5f, 0.5f);
			glUniform3f(glGetUniformLocation(bubbles[i]->shader.id, "spotLight.specular"), 1.0f, 1.0f, 1.0f);
			glUniform1f(glGetUniformLocation(bubbles[i]->shader.id, "spotLight.constant"), light_constant);
			glUniform1f(glGetUniformLocation(bubbles[i]->shader.id, "spotLight.linear"), light_linear);
			glUniform1f(glGetUniformLocation(bubbles[i]->shader.id, "spotLight.quadratic"), light_quadratic);
			glUniform1f(glGetUniformLocation(bubbles[i]->shader.id, "spotLight.cutOff"), glm::cos(glm::radians(light_cutOff)));
			glUniform1f(glGetUniformLocation(bubbles[i]->shader.id, "spotLight.outCutOff"), glm::cos(glm::radians(light_outCutOff)));
			
			glEnable(GL_BLEND);
			bubbles[i]->Draw(&model, &view, &projection, drawing_mode, false, player_camera.position);
			glDisable(GL_BLEND);
			bubbles[i]->Unbind();
		}

// 		bubble.shader.Activate();
// 		bubble.Bind();
		
// 		glUniform3fv(glGetUniformLocation(bubble.shader.id, "viewerPosition"), 1, glm::value_ptr(player_camera.position));
// 		glUniform3fv(glGetUniformLocation(bubble.shader.id, "centerPosition"), 1, glm::value_ptr(bubble.position));

// 		glUniform3f(glGetUniformLocation(bubble.shader.id, "material.ambient"), 0.2f, 0.2f, 0.2f);
// 		glUniform3f(glGetUniformLocation(bubble.shader.id, "material.diffuse"), 0.362f, 0.686f, 0.368f);
// 		glUniform3f(glGetUniformLocation(bubble.shader.id, "material.specular"), 0.5f, 0.5f, 0.5f);
// 		glUniform1f(glGetUniformLocation(bubble.shader.id, "material.shininess"), 32.0f);

// 		//direct light
// 		glUniform3fv(glGetUniformLocation(bubble.shader.id, "dirLight.direction"), 1, glm::value_ptr(aquarium_center));
// 		glUniform3f(glGetUniformLocation(bubble.shader.id, "dirLight.ambient"), 0.2f, 0.2f, 0.2f);
// 		glUniform3f(glGetUniformLocation(bubble.shader.id, "dirLight.diffuse"), 0.5, 0.5, 0.5);
// 		glUniform3f(glGetUniformLocation(bubble.shader.id, "dirlight.specular"), 1.0f, 1.0f, 1.0f);
		

// // spotlight
// 		glUniform3fv(glGetUniformLocation(bubble.shader.id, "spotLight.position"), 1, glm::value_ptr(lightPosition));
// 		glUniform3fv(glGetUniformLocation(bubble.shader.id, "spotLight.direction"), 1, glm::value_ptr(player_camera.front));
// 		glUniform3f(glGetUniformLocation(bubble.shader.id, "spotLight.ambient"), 0.2f, 0.2f, 0.2f);
// 		glUniform3f(glGetUniformLocation(bubble.shader.id, "spotLight.diffuse"), 0.5f, 0.5f, 0.5f);
// 		glUniform3f(glGetUniformLocation(bubble.shader.id, "spotLight.specular"), 1.0f, 1.0f, 1.0f);
// 		glUniform1f(glGetUniformLocation(bubble.shader.id, "spotLight.constant"), light_constant);
// 		glUniform1f(glGetUniformLocation(bubble.shader.id, "spotLight.linear"), light_linear);
// 		glUniform1f(glGetUniformLocation(bubble.shader.id, "spotLight.quadratic"), light_quadratic);
// 		glUniform1f(glGetUniformLocation(bubble.shader.id, "spotLight.cutOff"), glm::cos(glm::radians(light_cutOff)));
// 		glUniform1f(glGetUniformLocation(bubble.shader.id, "spotLight.outCutOff"), glm::cos(glm::radians(light_outCutOff)));
		
// 		glEnable(GL_BLEND);
// 		bubble.Draw(&model, &view, &projection, drawing_mode, false, player_camera.position);
// 		glDisable(GL_BLEND);
// 		bubble.Unbind();

// 		suzanne.shader.Activate();
// 		suzanne.Bind();
		
// 		glUniform3fv(glGetUniformLocation(suzanne.shader.id, "viewerPosition"), 1, glm::value_ptr(player_camera.position));
// 		glUniform3fv(glGetUniformLocation(suzanne.shader.id, "centerPosition"), 1, glm::value_ptr(suzanne.position));

// 		glUniform3f(glGetUniformLocation(suzanne.shader.id, "material.ambient"), 0.2f, 0.2f, 0.2f);
// 		glUniform3f(glGetUniformLocation(suzanne.shader.id, "material.diffuse"), 0.362f, 0.686f, 0.368f);
// 		glUniform3f(glGetUniformLocation(suzanne.shader.id, "material.specular"), 0.5f, 0.5f, 0.5f);
// 		glUniform1f(glGetUniformLocation(suzanne.shader.id, "material.shininess"), 32.0f);

// 		//direct light
// 		glUniform3fv(glGetUniformLocation(suzanne.shader.id, "dirLight.direction"), 1, glm::value_ptr(aquarium_center));
// 		glUniform3f(glGetUniformLocation(suzanne.shader.id, "dirLight.ambient"), 0.2f, 0.2f, 0.2f);
// 		glUniform3f(glGetUniformLocation(suzanne.shader.id, "dirLight.diffuse"), 0.5, 0.5, 0.5);
// 		glUniform3f(glGetUniformLocation(suzanne.shader.id, "dirlight.specular"), 1.0f, 1.0f, 1.0f);
		

// // spotlight
// 		glUniform3fv(glGetUniformLocation(suzanne.shader.id, "spotLight.position"), 1, glm::value_ptr(lightPosition));
// 		glUniform3fv(glGetUniformLocation(suzanne.shader.id, "spotLight.direction"), 1, glm::value_ptr(player_camera.front));
// 		glUniform3f(glGetUniformLocation(suzanne.shader.id, "spotLight.ambient"), 0.2f, 0.2f, 0.2f);
// 		glUniform3f(glGetUniformLocation(suzanne.shader.id, "spotLight.diffuse"), 0.5f, 0.5f, 0.5f);
// 		glUniform3f(glGetUniformLocation(suzanne.shader.id, "spotLight.specular"), 1.0f, 1.0f, 1.0f);
// 		glUniform1f(glGetUniformLocation(suzanne.shader.id, "spotLight.constant"), light_constant);
// 		glUniform1f(glGetUniformLocation(suzanne.shader.id, "spotLight.linear"), light_linear);
// 		glUniform1f(glGetUniformLocation(suzanne.shader.id, "spotLight.quadratic"), light_quadratic);
// 		glUniform1f(glGetUniformLocation(suzanne.shader.id, "spotLight.cutOff"), glm::cos(glm::radians(light_cutOff)));
// 		glUniform1f(glGetUniformLocation(suzanne.shader.id, "spotLight.outCutOff"), glm::cos(glm::radians(light_outCutOff)));

// 		suzanne.Draw(&model, &view, &projection, drawing_mode, false, player_camera.position);
// 		suzanne.Unbind();

// 		wibblywoobly.shader.Activate();
// 		wibblywoobly.Bind();

// 		glUniform3fv(glGetUniformLocation(wibblywoobly.shader.id, "viewerPosition"), 1, glm::value_ptr(player_camera.position));
// 		glUniform3fv(glGetUniformLocation(wibblywoobly.shader.id, "centerPosition"), 1, glm::value_ptr(wibblywoobly.position));

// 		glUniform3f(glGetUniformLocation(wibblywoobly.shader.id, "material.ambient"), 0.2f, 0.2f, 0.2f);
// 		glUniform3f(glGetUniformLocation(wibblywoobly.shader.id, "material.diffuse"), 0.362f, 0.686f, 0.368f);
// 		glUniform3f(glGetUniformLocation(wibblywoobly.shader.id, "material.specular"), 0.5f, 0.5f, 0.5f);
// 		glUniform1f(glGetUniformLocation(wibblywoobly.shader.id, "material.shininess"), 32.0f);

// 		//direct light
// 		glUniform3fv(glGetUniformLocation(wibblywoobly.shader.id, "dirLight.direction"), 1, glm::value_ptr(aquarium_center));
// 		glUniform3f(glGetUniformLocation(wibblywoobly.shader.id, "dirLight.ambient"), 0.2f, 0.2f, 0.2f);
// 		glUniform3f(glGetUniformLocation(wibblywoobly.shader.id, "dirLight.diffuse"), 0.5, 0.5, 0.5);
// 		glUniform3f(glGetUniformLocation(wibblywoobly.shader.id, "dirlight.specular"), 1.0f, 1.0f, 1.0f);
		

// // spotlight
// 		glUniform3fv(glGetUniformLocation(wibblywoobly.shader.id, "spotLight.position"), 1, glm::value_ptr(lightPosition));
// 		glUniform3fv(glGetUniformLocation(wibblywoobly.shader.id, "spotLight.direction"), 1, glm::value_ptr(player_camera.front));
// 		glUniform3f(glGetUniformLocation(wibblywoobly.shader.id, "spotLight.ambient"), 0.2f, 0.2f, 0.2f);
// 		glUniform3f(glGetUniformLocation(wibblywoobly.shader.id, "spotLight.diffuse"), 0.5f, 0.5f, 0.5f);
// 		glUniform3f(glGetUniformLocation(wibblywoobly.shader.id, "spotLight.specular"), 1.0f, 1.0f, 1.0f);
// 		glUniform1f(glGetUniformLocation(wibblywoobly.shader.id, "spotLight.constant"), light_constant);
// 		glUniform1f(glGetUniformLocation(wibblywoobly.shader.id, "spotLight.linear"), light_linear);
// 		glUniform1f(glGetUniformLocation(wibblywoobly.shader.id, "spotLight.quadratic"), light_quadratic);
// 		glUniform1f(glGetUniformLocation(wibblywoobly.shader.id, "spotLight.cutOff"), glm::cos(glm::radians(light_cutOff)));
// 		glUniform1f(glGetUniformLocation(wibblywoobly.shader.id, "spotLight.outCutOff"), glm::cos(glm::radians(light_outCutOff)));

// 		wibblywoobly.Draw(&model, &view, &projection, drawing_mode, false, player_camera.position);
// 		wibblywoobly.Unbind();

		sun.shader.Activate();
		sun.Bind();
		glUniform3fv(glGetUniformLocation(sun.shader.id, "centerPosition"), 1, glm::value_ptr(sun.position));
		sun.Draw(&model, &view, &projection, drawing_mode, false, player_camera.position);
		sun.Unbind();

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

*/
