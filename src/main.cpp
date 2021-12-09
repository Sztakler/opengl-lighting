#include <iostream>
#include <math.h>
#include <time.h>

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
#include "drawable_light.h"

#include <GLFW/glfw3.h>

// Vertices coordinates
GLfloat vertices[] =
	{
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.0f, 0.0f, 0.0f};

void windowResizeHandler(GLFWwindow *window, int width, int height)
{
	const float aspectRatio = (float)width / height;
	float sx = aspectRatio > 1.0f ? aspectRatio : 1.0f;
	float sy = aspectRatio > 1.0f ? 1.0f : 1.0f / aspectRatio;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-sx, sx, -sy, sy, 1, -1);

	glViewport(0, 0, width, height);
}

bool first_mouse_movement = true;
float delta_time;
float last_change;

int SCR_WIDTH = 1920;
int SCR_HEIGHT = 1080;

float last_x = SCR_WIDTH / 2;
float last_y = SCR_HEIGHT / 2;

float fov = 45.0f;

float pitch = 0.0f;
float yaw = -90.0f;

DRAWING_MODE drawing_mode = TRIANGLES;

Camera player_camera(glm::vec3(5.0f, 5.0f, 5.0f));
Camera static_camera(glm::vec3(-20.0f, 10.0f, 20.0f));
Camera *current_camera = &player_camera;
CAMERA camera_index = PLAYER_CAMERA;

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
	current_camera->processMouseScroll(y_offset);
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

	if ((glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) && (last_change > 0.3))
	{
		if (drawing_mode == TRIANGLES)
			drawing_mode = WIREFRAME;
		else
			drawing_mode = TRIANGLES;

		last_change = 0.0;
	}

	if ((glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) && (last_change > 0.3))
	{
		switch (camera_index)
		{
		case PLAYER_CAMERA:
			current_camera = &static_camera;
			camera_index = STATIC_CAMERA;
			break;
		case STATIC_CAMERA:
			current_camera = &player_camera;
			camera_index = PLAYER_CAMERA;
			break;
		default:
			current_camera = &static_camera;
			camera_index = STATIC_CAMERA;
			break;
		}

		last_change = 0.0;
		std::cout << "CURRENT CAMERA = " << camera_index << std::endl;
	}
}

void loadPointLightsUniforms(std::vector<DrawableLight *> lights, Drawable entity);
void loadPointLightsUniforms(std::vector<DrawableLight *> lights, Enemies entity);
void loadDirectionLightsUniforms(DirLight light, Drawable entity);
void loadDirectionLightsUniforms(DirLight light, Enemies entity);
void loadSpotLightsUniforms(SpotLight light, Drawable entity);
void loadSpotLightsUniforms(SpotLight light, Enemies entity);

int totalScore;
bool GAME_OVER = false;
int level = 1;

int main(int argc, char *argv[])
{
	int seed;
	int board_size;

	if (argc < 2)
	{
		seed = time(0);
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

	glfwWindowHint(GLFW_SAMPLES, 4);			   // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);		   // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGl

	// Create a GLFWwindow object of 800 by 800 pixels, naming it "YoutubeOpenGL"
	GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "framework", NULL, NULL);
	if (window == NULL)
	{
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	glfwSetWindowSizeCallback(window, framebuffer_size_callback);
	glfwMakeContextCurrent(window);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Hides mouse cursor and captures it
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

	Drawable aquarium("data/aquarium_smooth.obj", "shaders/aquarium.vsh", "shaders/aquarium.fsh",
					  Material{glm::vec3(0.0314 / 4, 0.1686 / 4, 0.4275 / 4), glm::vec3(0.0314, 0.1686, 0.4275),
							   glm::vec3(0.0314, 0.1686, 0.4275), 32.0f});
	Drawable plane("data/plane.obj", "shaders/aquarium.vsh", "shaders/aquarium.fsh",
				   Material{glm::vec3(0.0314 / 4, 0.1686 / 4, 0.4275 / 4), glm::vec3(0.0314, 0.1686, 0.4275),
							glm::vec3(0.5, 0.5, 0.5), 36.0f});
	Drawable player("data/sphere.obj", "shaders/aquarium.vsh", "shaders/aquarium.fsh",
					Material{glm::vec3(1.0, 0.0, 0.1333), glm::vec3(1.0, 0.0, 0.1333),
							 glm::vec3(0.5, 0.5, 0.5), 36.0f});

	int n_bubbles = 100;
	int n_lights = 6;

	Enemies bubbles("data/sphere.obj", "shaders/enemies.vsh", "shaders/enemies.fsh",
					seed, glm::vec3(20, 20, 40), Material{glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.8941, 0.7412, 0.9647), glm::vec3(0.5f, 0.5f, 0.5f), 36.0f}, n_bubbles);

	std::vector<DrawableLight *> glowingBubbles;
	for (int i = 0; i < n_lights; i++)
	{
		glowingBubbles.push_back(new DrawableLight("data/sphere.obj", "shaders/glowing_bubble.vsh", "shaders/glowing_bubble.fsh", seed++));
	}

	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	float rotation = 0.0f;
	double prev_time = glfwGetTime();

	// float deltaTime = 0.0f;	// Time between current frame and last frame
	float lastFrame = 0.0f; // Time of last frame

	player_camera.front = glm::vec3(1.0, 0.0, 1.0);

	glm::vec3 lightPosition(0.0, 10.0, 0.0);
	glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

	glm::vec3 aquarium_center(10.0, 10.0, 20.0);

	aquarium.position.x += 14.362542;
	aquarium.position.y += 8.999555;
	aquarium.position.z += 19.583765;

	plane.position.y += 8.999555;

	player.position = {4.0, 4.0, 4.0};

	DirLight directionLight = {
		.direction = glm::vec3(0.0, 30.0, 0.0),
		.ambient = glm::vec3(0.2f, 0.2f, 0.2f),
		.diffuse = glm::vec3(0.5f, 0.5f, 0.5f),
		.specular = glm::vec3(1.0f, 1.0f, 1.0f)};

	SpotLight spotLight = {
		.position = player_camera.position,
		.direction = player_camera.front,
		.ambient = glm::vec3(0.2f, 0.0f, 0.0f),
		.diffuse = glm::vec3(0.5f, 0.0f, 0.0f),
		.specular = glm::vec3(1.0f, 0.0f, 0.0f),
		.constant = 0.5,
		.linear = 0.09,
		.quadratic = 0.2,
		.cutOff = 12.5,
		.outCutOff = 17.5};

	unsigned int counter = 0;
	// glfwSwapInterval(0); // disables VSync -- unlimited FPS

	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0 && !GAME_OVER)
	{

		float currentFrame = glfwGetTime();
		delta_time = currentFrame - lastFrame;
		last_change += delta_time;
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

		if (camera_index == PLAYER_CAMERA)
			view = current_camera->getViewMatrix();
		if (camera_index == STATIC_CAMERA)
			view = glm::lookAt(static_camera.position, player_camera.position, static_camera.up);

		projection = glm::perspective(glm::radians(current_camera->zoom), (float)(SCR_WIDTH) / (float)SCR_HEIGHT, 0.01f, 100.0f);

		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

		player.shader.Activate();
		player.Bind();

		glUniform3fv(glGetUniformLocation(player.shader.id, "viewerPosition"), 1, glm::value_ptr(player_camera.position));

		player.loadUniforms();

		// direction light
		loadDirectionLightsUniforms(directionLight, player);
		// // point lights
		loadPointLightsUniforms(glowingBubbles, player);
		// spotlight
		loadSpotLightsUniforms(spotLight, player);

		if (camera_index != PLAYER_CAMERA)
		{
			player.Draw(&model, &view, &projection, TRIANGLES, false, player_camera.position);
			player.Unbind();
		}

		aquarium.shader.Activate();
		aquarium.Bind();
		glUniform3fv(glGetUniformLocation(aquarium.shader.id, "viewerPosition"), 1, glm::value_ptr(player_camera.position));
		aquarium.loadUniforms();
		// direction light
		loadDirectionLightsUniforms(directionLight, aquarium);
		// point lights
		loadPointLightsUniforms(glowingBubbles, aquarium);
		// spotlight
		loadSpotLightsUniforms(spotLight, aquarium);

		aquarium.Draw(&model, &view, &projection, TRIANGLES, false, player_camera.position);
		aquarium.Unbind();

		plane.shader.Activate();
		plane.Bind();

		glUniform3fv(glGetUniformLocation(plane.shader.id, "viewerPosition"), 1, glm::value_ptr(player_camera.position));

		plane.loadUniforms();
		// direction light
		loadDirectionLightsUniforms(directionLight, plane);
		// point lights
		loadPointLightsUniforms(glowingBubbles, plane);
		// spotlight
		loadSpotLightsUniforms(spotLight, plane);

		plane.Draw(&model, &view, &projection, TRIANGLES, false, player_camera.position);
		plane.Unbind();

		for (auto point_light : glowingBubbles)
		{
			point_light->shader.Activate();
			point_light->Bind();

			glUniform3fv(glGetUniformLocation(point_light->shader.id, "viewerPosition"), 1, glm::value_ptr(player_camera.position));

			point_light->loadUniforms();

			point_light->Draw(&model, &view, &projection, TRIANGLES, false, player_camera.position);
			point_light->Unbind();
		}

		bubbles.shader.Activate();
		bubbles.Bind();
		glUniform3fv(glGetUniformLocation(bubbles.shader.id, "viewerPosition"), 1, glm::value_ptr(player_camera.position));
		bubbles.loadUniforms();
		// direction light
		loadDirectionLightsUniforms(directionLight, bubbles);
		// point lights
		loadPointLightsUniforms(glowingBubbles, bubbles);
		// spotlight
		loadSpotLightsUniforms(spotLight, bubbles);
		// glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);
		glEnable(GL_BLEND);
		bubbles.DrawInstanced(n_bubbles, &model, &view, &projection, drawing_mode, true, current_camera->position);
		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
		// glEnable(GL_DEPTH_TEST);
		bubbles.Unbind();

		for (int i = 0; i < bubbles.positions.size(); i++)
		{
			bubbles.positions[i].y += bubbles.velocity;
			bubbles.translations[3 * i + 1] = bubbles.positions[i].y;

			if (bubbles.positions[i].y > 18.0)
			{
				bubbles.positions[i] = glm::vec3(rand() % 2000 / 100.0,
												 -rand() % 1000 / 100.0,
												 rand() % 4000 / 100.0);

				bubbles.translations[3 * i] = bubbles.positions[i].x;
				bubbles.translations[3 * i + 1] = bubbles.positions[i].y;
				bubbles.translations[3 * i + 2] = bubbles.positions[i].z;
			}
		}

		player.position = player_camera.position;

		// Collision detection

		if (player_camera.position.x < 0.5 || player_camera.position.x > 19.5 ||
			player_camera.position.y < 0.5 || player_camera.position.y > 19.5 ||
			player_camera.position.z < 0.5 || player_camera.position.z > 39.5)
			player_camera.undoMove(delta_time);

		for (glm::vec3 bubble_position : bubbles.positions)
		{
			if (glm::length(bubble_position - player_camera.position) < 2.0)
			{
				player_camera.undoMove(delta_time);
				// GAME_OVER = true;
			}
		}
		for (auto glowingBubble : glowingBubbles)
		{
			if (glm::length(glowingBubble->point_light.position - player_camera.position) < 2.0)
			{
				totalScore += 10;
				glowingBubble->point_light.position = glm::vec3(100.0, 100.0, 100.0);
				glowingBubble->position = glowingBubble->point_light.position;
			}
		}

		if (player.position.z > 39.0)
		{
			player_camera.position = glm::vec3(10.0, 10.0, 1.0);

			level++;
			totalScore += 100;

			for (int i = 0; i < n_bubbles; i++)
			{
				bubbles.positions[i] = glm::vec3(rand() % 2000 / 100.0,
												 rand() % 1000 / 100.0,
												 rand() % 3500 / 100.0 + 5.0);

				bubbles.translations[3 * i] = bubbles.positions[i].x;
				bubbles.translations[3 * i + 1] = bubbles.positions[i].y;
				bubbles.translations[3 * i + 2] = bubbles.positions[i].z;
			}
			for (int i = 0; i < n_lights; i++)
			{
				glowingBubbles[i]->position = glm::vec3(rand() % 2000 / 100.0,
														rand() % 1000 / 100.0,
														rand() % 3500 / 100.0 + 5.0);
				glowingBubbles[i]->point_light.position = glowingBubbles[i]->position;
			}

			bubbles.velocity += 0.005;
		}

		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();
	}

	std::cout << "You've survived " << level << " levels and scored " << totalScore << "points!\n";

	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();
	return 0;
}

void loadPointLightsUniforms(std::vector<DrawableLight *> lights, Enemies entity)
{
	glUniform3fv(glGetUniformLocation(entity.shader.id, "pointLights[0].position"), 1, glm::value_ptr(lights[0]->point_light.position));
	glUniform3fv(glGetUniformLocation(entity.shader.id, "pointLights[0].ambient"), 1, glm::value_ptr(lights[0]->point_light.ambient));
	glUniform3fv(glGetUniformLocation(entity.shader.id, "pointLights[0].diffuse"), 1, glm::value_ptr(lights[0]->point_light.diffuse));
	glUniform3fv(glGetUniformLocation(entity.shader.id, "pointLights[0].specular"), 1, glm::value_ptr(lights[0]->point_light.specular));
	glUniform1f(glGetUniformLocation(entity.shader.id, "pointLights[0].constant"), lights[0]->point_light.constant);
	glUniform1f(glGetUniformLocation(entity.shader.id, "pointLights[0].linear"), lights[0]->point_light.linear);
	glUniform1f(glGetUniformLocation(entity.shader.id, "pointLights[0].quadratic"), lights[0]->point_light.quadratic);

	glUniform3fv(glGetUniformLocation(entity.shader.id, "pointLights[1].position"), 1, glm::value_ptr(lights[1]->point_light.position));
	glUniform3fv(glGetUniformLocation(entity.shader.id, "pointLights[1].ambient"), 1, glm::value_ptr(lights[1]->point_light.ambient));
	glUniform3fv(glGetUniformLocation(entity.shader.id, "pointLights[1].diffuse"), 1, glm::value_ptr(lights[1]->point_light.diffuse));
	glUniform3fv(glGetUniformLocation(entity.shader.id, "pointLights[1].specular"), 1, glm::value_ptr(lights[1]->point_light.specular));
	glUniform1f(glGetUniformLocation(entity.shader.id, "pointLights[1].constant"), lights[1]->point_light.constant);
	glUniform1f(glGetUniformLocation(entity.shader.id, "pointLights[1].linear"), lights[1]->point_light.linear);
	glUniform1f(glGetUniformLocation(entity.shader.id, "pointLights[1].quadratic"), lights[1]->point_light.quadratic);

	glUniform3fv(glGetUniformLocation(entity.shader.id, "pointLights[2].position"), 1, glm::value_ptr(lights[2]->point_light.position));
	glUniform3fv(glGetUniformLocation(entity.shader.id, "pointLights[2].ambient"), 1, glm::value_ptr(lights[2]->point_light.ambient));
	glUniform3fv(glGetUniformLocation(entity.shader.id, "pointLights[2].diffuse"), 1, glm::value_ptr(lights[2]->point_light.diffuse));
	glUniform3fv(glGetUniformLocation(entity.shader.id, "pointLights[2].specular"), 1, glm::value_ptr(lights[2]->point_light.specular));
	glUniform1f(glGetUniformLocation(entity.shader.id, "pointLights[2].constant"), lights[2]->point_light.constant);
	glUniform1f(glGetUniformLocation(entity.shader.id, "pointLights[2].linear"), lights[2]->point_light.linear);
	glUniform1f(glGetUniformLocation(entity.shader.id, "pointLights[2].quadratic"), lights[2]->point_light.quadratic);

	glUniform3fv(glGetUniformLocation(entity.shader.id, "pointLights[3].position"), 1, glm::value_ptr(lights[3]->point_light.position));
	glUniform3fv(glGetUniformLocation(entity.shader.id, "pointLights[3].ambient"), 1, glm::value_ptr(lights[3]->point_light.ambient));
	glUniform3fv(glGetUniformLocation(entity.shader.id, "pointLights[3].diffuse"), 1, glm::value_ptr(lights[3]->point_light.diffuse));
	glUniform3fv(glGetUniformLocation(entity.shader.id, "pointLights[3].specular"), 1, glm::value_ptr(lights[3]->point_light.specular));
	glUniform1f(glGetUniformLocation(entity.shader.id, "pointLights[3].constant"), lights[3]->point_light.constant);
	glUniform1f(glGetUniformLocation(entity.shader.id, "pointLights[3].linear"), lights[3]->point_light.linear);
	glUniform1f(glGetUniformLocation(entity.shader.id, "pointLights[3].quadratic"), lights[3]->point_light.quadratic);

	glUniform3fv(glGetUniformLocation(entity.shader.id, "pointLights[4].position"), 1, glm::value_ptr(lights[4]->point_light.position));
	glUniform3fv(glGetUniformLocation(entity.shader.id, "pointLights[4].ambient"), 1, glm::value_ptr(lights[4]->point_light.ambient));
	glUniform3fv(glGetUniformLocation(entity.shader.id, "pointLights[4].diffuse"), 1, glm::value_ptr(lights[4]->point_light.diffuse));
	glUniform3fv(glGetUniformLocation(entity.shader.id, "pointLights[4].specular"), 1, glm::value_ptr(lights[4]->point_light.specular));
	glUniform1f(glGetUniformLocation(entity.shader.id, "pointLights[4].constant"), lights[4]->point_light.constant);
	glUniform1f(glGetUniformLocation(entity.shader.id, "pointLights[4].linear"), lights[4]->point_light.linear);
	glUniform1f(glGetUniformLocation(entity.shader.id, "pointLights[4].quadratic"), lights[4]->point_light.quadratic);

	glUniform3fv(glGetUniformLocation(entity.shader.id, "pointLights[5].position"), 1, glm::value_ptr(lights[5]->point_light.position));
	glUniform3fv(glGetUniformLocation(entity.shader.id, "pointLights[5].ambient"), 1, glm::value_ptr(lights[5]->point_light.ambient));
	glUniform3fv(glGetUniformLocation(entity.shader.id, "pointLights[5].diffuse"), 1, glm::value_ptr(lights[5]->point_light.diffuse));
	glUniform3fv(glGetUniformLocation(entity.shader.id, "pointLights[5].specular"), 1, glm::value_ptr(lights[5]->point_light.specular));
	glUniform1f(glGetUniformLocation(entity.shader.id, "pointLights[5].constant"), lights[5]->point_light.constant);
	glUniform1f(glGetUniformLocation(entity.shader.id, "pointLights[5].linear"), lights[5]->point_light.linear);
	glUniform1f(glGetUniformLocation(entity.shader.id, "pointLights[5].quadratic"), lights[5]->point_light.quadratic);
}

void loadPointLightsUniforms(std::vector<DrawableLight *> lights, Drawable entity)
{
	glUniform3fv(glGetUniformLocation(entity.shader.id, "pointLights[0].position"), 1, glm::value_ptr(lights[0]->point_light.position));
	glUniform3fv(glGetUniformLocation(entity.shader.id, "pointLights[0].ambient"), 1, glm::value_ptr(lights[0]->point_light.ambient));
	glUniform3fv(glGetUniformLocation(entity.shader.id, "pointLights[0].diffuse"), 1, glm::value_ptr(lights[0]->point_light.diffuse));
	glUniform3fv(glGetUniformLocation(entity.shader.id, "pointLights[0].specular"), 1, glm::value_ptr(lights[0]->point_light.specular));
	glUniform1f(glGetUniformLocation(entity.shader.id, "pointLights[0].constant"), lights[0]->point_light.constant);
	glUniform1f(glGetUniformLocation(entity.shader.id, "pointLights[0].linear"), lights[0]->point_light.linear);
	glUniform1f(glGetUniformLocation(entity.shader.id, "pointLights[0].quadratic"), lights[0]->point_light.quadratic);

	glUniform3fv(glGetUniformLocation(entity.shader.id, "pointLights[1].position"), 1, glm::value_ptr(lights[1]->point_light.position));
	glUniform3fv(glGetUniformLocation(entity.shader.id, "pointLights[1].ambient"), 1, glm::value_ptr(lights[1]->point_light.ambient));
	glUniform3fv(glGetUniformLocation(entity.shader.id, "pointLights[1].diffuse"), 1, glm::value_ptr(lights[1]->point_light.diffuse));
	glUniform3fv(glGetUniformLocation(entity.shader.id, "pointLights[1].specular"), 1, glm::value_ptr(lights[1]->point_light.specular));
	glUniform1f(glGetUniformLocation(entity.shader.id, "pointLights[1].constant"), lights[1]->point_light.constant);
	glUniform1f(glGetUniformLocation(entity.shader.id, "pointLights[1].linear"), lights[1]->point_light.linear);
	glUniform1f(glGetUniformLocation(entity.shader.id, "pointLights[1].quadratic"), lights[1]->point_light.quadratic);

	glUniform3fv(glGetUniformLocation(entity.shader.id, "pointLights[2].position"), 1, glm::value_ptr(lights[2]->point_light.position));
	glUniform3fv(glGetUniformLocation(entity.shader.id, "pointLights[2].ambient"), 1, glm::value_ptr(lights[2]->point_light.ambient));
	glUniform3fv(glGetUniformLocation(entity.shader.id, "pointLights[2].diffuse"), 1, glm::value_ptr(lights[2]->point_light.diffuse));
	glUniform3fv(glGetUniformLocation(entity.shader.id, "pointLights[2].specular"), 1, glm::value_ptr(lights[2]->point_light.specular));
	glUniform1f(glGetUniformLocation(entity.shader.id, "pointLights[2].constant"), lights[2]->point_light.constant);
	glUniform1f(glGetUniformLocation(entity.shader.id, "pointLights[2].linear"), lights[2]->point_light.linear);
	glUniform1f(glGetUniformLocation(entity.shader.id, "pointLights[2].quadratic"), lights[2]->point_light.quadratic);

	glUniform3fv(glGetUniformLocation(entity.shader.id, "pointLights[3].position"), 1, glm::value_ptr(lights[3]->point_light.position));
	glUniform3fv(glGetUniformLocation(entity.shader.id, "pointLights[3].ambient"), 1, glm::value_ptr(lights[3]->point_light.ambient));
	glUniform3fv(glGetUniformLocation(entity.shader.id, "pointLights[3].diffuse"), 1, glm::value_ptr(lights[3]->point_light.diffuse));
	glUniform3fv(glGetUniformLocation(entity.shader.id, "pointLights[3].specular"), 1, glm::value_ptr(lights[3]->point_light.specular));
	glUniform1f(glGetUniformLocation(entity.shader.id, "pointLights[3].constant"), lights[3]->point_light.constant);
	glUniform1f(glGetUniformLocation(entity.shader.id, "pointLights[3].linear"), lights[3]->point_light.linear);
	glUniform1f(glGetUniformLocation(entity.shader.id, "pointLights[3].quadratic"), lights[3]->point_light.quadratic);

	glUniform3fv(glGetUniformLocation(entity.shader.id, "pointLights[4].position"), 1, glm::value_ptr(lights[4]->point_light.position));
	glUniform3fv(glGetUniformLocation(entity.shader.id, "pointLights[4].ambient"), 1, glm::value_ptr(lights[4]->point_light.ambient));
	glUniform3fv(glGetUniformLocation(entity.shader.id, "pointLights[4].diffuse"), 1, glm::value_ptr(lights[4]->point_light.diffuse));
	glUniform3fv(glGetUniformLocation(entity.shader.id, "pointLights[4].specular"), 1, glm::value_ptr(lights[4]->point_light.specular));
	glUniform1f(glGetUniformLocation(entity.shader.id, "pointLights[4].constant"), lights[4]->point_light.constant);
	glUniform1f(glGetUniformLocation(entity.shader.id, "pointLights[4].linear"), lights[4]->point_light.linear);
	glUniform1f(glGetUniformLocation(entity.shader.id, "pointLights[4].quadratic"), lights[4]->point_light.quadratic);

	glUniform3fv(glGetUniformLocation(entity.shader.id, "pointLights[5].position"), 1, glm::value_ptr(lights[5]->point_light.position));
	glUniform3fv(glGetUniformLocation(entity.shader.id, "pointLights[5].ambient"), 1, glm::value_ptr(lights[5]->point_light.ambient));
	glUniform3fv(glGetUniformLocation(entity.shader.id, "pointLights[5].diffuse"), 1, glm::value_ptr(lights[5]->point_light.diffuse));
	glUniform3fv(glGetUniformLocation(entity.shader.id, "pointLights[5].specular"), 1, glm::value_ptr(lights[5]->point_light.specular));
	glUniform1f(glGetUniformLocation(entity.shader.id, "pointLights[5].constant"), lights[5]->point_light.constant);
	glUniform1f(glGetUniformLocation(entity.shader.id, "pointLights[5].linear"), lights[5]->point_light.linear);
	glUniform1f(glGetUniformLocation(entity.shader.id, "pointLights[5].quadratic"), lights[5]->point_light.quadratic);
}

void loadDirectionLightsUniforms(DirLight directionLight, Drawable entity)
{
	glUniform3fv(glGetUniformLocation(entity.shader.id, "directionlight.direction"), 1, glm::value_ptr(directionLight.direction));
	glUniform3fv(glGetUniformLocation(entity.shader.id, "directionlight.ambient"), 1, glm::value_ptr(directionLight.ambient));
	glUniform3fv(glGetUniformLocation(entity.shader.id, "directionlight.diffuse"), 1, glm::value_ptr(directionLight.diffuse));
	glUniform3fv(glGetUniformLocation(entity.shader.id, "directionlight.specular"), 1, glm::value_ptr(directionLight.specular));
}

void loadDirectionLightsUniforms(DirLight directionLight, Enemies entity)
{
	glUniform3fv(glGetUniformLocation(entity.shader.id, "directionlight.direction"), 1, glm::value_ptr(directionLight.direction));
	glUniform3fv(glGetUniformLocation(entity.shader.id, "directionlight.ambient"), 1, glm::value_ptr(directionLight.ambient));
	glUniform3fv(glGetUniformLocation(entity.shader.id, "directionlight.diffuse"), 1, glm::value_ptr(directionLight.diffuse));
	glUniform3fv(glGetUniformLocation(entity.shader.id, "directionlight.specular"), 1, glm::value_ptr(directionLight.specular));
}

void loadSpotLightsUniforms(SpotLight spotLight, Drawable entity)
{
	glUniform3fv(glGetUniformLocation(entity.shader.id, "spotlight.position"), 1, glm::value_ptr(player_camera.position));
	glUniform3fv(glGetUniformLocation(entity.shader.id, "spotlight.direction"), 1, glm::value_ptr(player_camera.front));
	glUniform3fv(glGetUniformLocation(entity.shader.id, "spotlight.ambient"), 1, glm::value_ptr(spotLight.ambient));
	glUniform3fv(glGetUniformLocation(entity.shader.id, "spotlight.diffuse"), 1, glm::value_ptr(spotLight.ambient));
	glUniform3fv(glGetUniformLocation(entity.shader.id, "spotlight.specular"), 1, glm::value_ptr(spotLight.ambient));
	glUniform1f(glGetUniformLocation(entity.shader.id, "spotlight.constant"), spotLight.constant);
	glUniform1f(glGetUniformLocation(entity.shader.id, "spotlight.linear"), spotLight.linear);
	glUniform1f(glGetUniformLocation(entity.shader.id, "spotlight.quadratic"), spotLight.quadratic);
	glUniform1f(glGetUniformLocation(entity.shader.id, "spotlight.cutOff"), glm::cos(glm::radians(spotLight.cutOff)));
	glUniform1f(glGetUniformLocation(entity.shader.id, "spotlight.outCutOff"), glm::cos(glm::radians(spotLight.outCutOff)));
}

void loadSpotLightsUniforms(SpotLight spotLight, Enemies entity)
{
	glUniform3fv(glGetUniformLocation(entity.shader.id, "spotlight.position"), 1, glm::value_ptr(player_camera.position));
	glUniform3fv(glGetUniformLocation(entity.shader.id, "spotlight.direction"), 1, glm::value_ptr(player_camera.front));
	glUniform3fv(glGetUniformLocation(entity.shader.id, "spotlight.ambient"), 1, glm::value_ptr(spotLight.ambient));
	glUniform3fv(glGetUniformLocation(entity.shader.id, "spotlight.diffuse"), 1, glm::value_ptr(spotLight.ambient));
	glUniform3fv(glGetUniformLocation(entity.shader.id, "spotlight.specular"), 1, glm::value_ptr(spotLight.ambient));
	glUniform1f(glGetUniformLocation(entity.shader.id, "spotlight.constant"), spotLight.constant);
	glUniform1f(glGetUniformLocation(entity.shader.id, "spotlight.linear"), spotLight.linear);
	glUniform1f(glGetUniformLocation(entity.shader.id, "spotlight.quadratic"), spotLight.quadratic);
	glUniform1f(glGetUniformLocation(entity.shader.id, "spotlight.cutOff"), glm::cos(glm::radians(spotLight.cutOff)));
	glUniform1f(glGetUniformLocation(entity.shader.id, "spotlight.outCutOff"), glm::cos(glm::radians(spotLight.outCutOff)));
}