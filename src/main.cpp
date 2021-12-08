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


	Drawable aquarium("data/aquarium_smooth.obj", "shaders/player.vsh", "shaders/player.fsh",
					Material{glm::vec3(0.0314 / 4, 0.1686 / 4, 0.4275 / 4), glm::vec3(0.0314, 0.1686, 0.4275),
							 glm::vec3(0.0314, 0.1686, 0.4275), 36.0f});
	Drawable plane("data/plane.obj", "shaders/player.vsh", "shaders/player.fsh",
					Material{glm::vec3(0.0, 0.4039, 0.8667), glm::vec3(0.0, 0.4039, 0.8667),
							 glm::vec3(0.0, 0.4039, 0.8667), 36.0f});

	int n_bubbles = 1;
	float bubble_base_speed = 0.01;
	// std::vector<Drawable*> bubbles(n_bubbles);
	// for (int i = 0; i < n_bubbles; i++)
	// {
	// 	bubbles[i] = new Drawable("data/icosphere.obj", "shaders/bubble.vsh", "shaders/bubble.fsh", 
	// 								   Material{glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.362f, 0.686f, 0.368f),
	// 								            glm::vec3(0.5f, 0.5f, 0.5f), 32.0f});
	// 	bubbles[i]->position = glm::vec3(rand() % 2000 / 100.0, rand() % 2000 / 100.0, rand() % 4000 / 100.0);
	// 	bubbles[i]->speed = bubble_base_speed + rand() % 100 / 10000.0;
	// }

	Enemies bubbles("data/sphere.obj", "shaders/enemies.vsh", "shaders/enemies.fsh",
		seed, glm::vec3(20, 20, 40), Material{glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.362f, 0.686f, 0.368f),
	 	glm::vec3(0.5f, 0.5f, 0.5f), 32.0f}, n_bubbles);

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


	float light_constant = 0.5;
	float light_linear = 0.09;
	float light_quadratic = 0.2;
	float light_cutOff = 12.5;
	float light_outCutOff = 17.5;

	unsigned int counter = 0; 
	// glfwSwapInterval(0); // disables VSync -- unlimited FPS

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


		// for (auto bubble_position : bubbles.positions)
		// {
		// 	if (bubble_position.y >= 20.0)
		// 	{
		// 		bubble_position = glm::vec3(rand() % 2000 / 100.0, rand() % 2000 / 100.0 - 20.0, rand() % 4000 / 100.0);
		// 		bubble->speed = bubble_base_speed + rand() % 100 / 10000.0;
		// 	}

		// }

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.07f, 0.07f, 0.07f, 1.0f);


		view = player_camera.getViewMatrix();
		projection = glm::perspective(glm::radians(player_camera.zoom), (float)(SCR_WIDTH) / (float)SCR_HEIGHT, 0.01f, 100.0f);
		
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

		aquarium.shader.Activate();
		aquarium.Bind();

		glUniform3fv(glGetUniformLocation(aquarium.shader.id, "viewerPosition"), 1, glm::value_ptr(player_camera.position));	

		aquarium.loadUniforms();

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

		aquarium.Draw(&model, &view, &projection, TRIANGLES, false, player_camera.position);
		aquarium.Unbind();

		plane.shader.Activate();
		plane.Bind();
		
		glUniform3fv(glGetUniformLocation(plane.shader.id, "viewerPosition"), 1, glm::value_ptr(player_camera.position));	
		
		plane.loadUniforms();

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
		

		plane.Draw(&model, &view, &projection, TRIANGLES, false, player_camera.position);

		plane.Unbind();


		// for (auto bubble : bubbles)
		// {
			bubbles.shader.Activate();
			bubbles.Bind();
			
			glUniform3fv(glGetUniformLocation(bubbles.shader.id, "viewerPosition"), 1, glm::value_ptr(player_camera.position));

			bubbles.loadUniforms();

			// direction light
			glUniform3fv(glGetUniformLocation(bubbles.shader.id, "directionlight.direction"), 1, glm::value_ptr(glm::vec3(0.0, 30.0, 0.0)));
			glUniform3f(glGetUniformLocation(bubbles.shader.id, "directionlight.ambient"), 0.2f, 0.2f, 0.2f);
			glUniform3f(glGetUniformLocation(bubbles.shader.id, "directionlight.diffuse"), 0.5f, 0.5f, 0.5f);
			glUniform3f(glGetUniformLocation(bubbles.shader.id, "directionlight.specular"), 1.0f, 1.0f, 1.0f);

			// spotlight
			glUniform3fv(glGetUniformLocation(bubbles.shader.id, "spotlight.position"), 1, glm::value_ptr(lightPosition));
			glUniform3fv(glGetUniformLocation(bubbles.shader.id, "spotlight.direction"), 1, glm::value_ptr(player_camera.front));
			glUniform3f(glGetUniformLocation(bubbles.shader.id, "spotlight.ambient"), 0.2f, 0.2f, 0.2f);
			glUniform3f(glGetUniformLocation(bubbles.shader.id, "spotlight.diffuse"), 0.5f, 0.5f, 0.5f);
			glUniform3f(glGetUniformLocation(bubbles.shader.id, "spotlight.specular"), 1.0f, 1.0f, 1.0f);
			glUniform1f(glGetUniformLocation(bubbles.shader.id, "spotlight.constant"), light_constant);
			glUniform1f(glGetUniformLocation(bubbles.shader.id, "spotlight.linear"), light_linear);
			glUniform1f(glGetUniformLocation(bubbles.shader.id, "spotlight.quadratic"), light_quadratic);
			glUniform1f(glGetUniformLocation(bubbles.shader.id, "spotlight.cutOff"), glm::cos(glm::radians(light_cutOff)));
			glUniform1f(glGetUniformLocation(bubbles.shader.id, "spotlight.outCutOff"), glm::cos(glm::radians(light_outCutOff)));
			
			glDisable(GL_CULL_FACE);
			glEnable(GL_BLEND);
			// bubbles.Draw(&model, &view, &projection, drawing_mode, false, player_camera.position);
			bubbles.DrawInstanced(n_bubbles, &model, &view, &projection, drawing_mode);
			glDisable(GL_BLEND);
			glEnable(GL_CULL_FACE);
			bubbles.Unbind();
		// }

		// Collision detection
	
		for (glm::vec3 bubble_position : bubbles.positions)
		{
			printf("distance: %f\n", glm::length(bubble_position - player_camera.position));
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