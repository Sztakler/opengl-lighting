#ifndef INCLUDES_H
#define INCLUDES_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cstdlib>
#include <math.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <sstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

typedef struct point
{
    float x;
    float y;
    float z;
} point;

typedef struct triangle
{
    point p1;
    point p2;
    point p3;
} triangle;

typedef struct vec3d
{
    float x;
    float y;
    float z;
} vec3d;

typedef struct AABB
{
    float Bminx;
    float Bmaxx;
    float Bminy;
    float Bmaxy;
    float Bminz;
    float Bmaxz;
} AABB;

typedef struct Material
{
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
} Material;

struct DirLight {
    glm::vec3 direction;
	
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

struct PointLight {
    glm::vec3 position;
    
    float constant;
    float linear;
    float quadratic;
	
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

struct SpotLight {
    glm::vec3 position;
    glm::vec3 direction;
  
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float constant;
    float linear;
    float quadratic;

    float cutOff;
    float outCutOff;

};
enum DRAWING_MODE
{
	TRIANGLES,
	WIREFRAME
};

enum CAMERA
{
    PLAYER_CAMERA,
    STATIC_CAMERA
};

#endif