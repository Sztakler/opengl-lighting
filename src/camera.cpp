#include "camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) :
    front(glm::vec3(0.0f, 0.0f, -1.0f)),
    movement_speed(SPEED),
    mouse_sensitivity(SENSITIVITY),
    zoom(ZOOM)
{
    this->position = position;
    this->world_up = up;
    this->yaw = yaw;
    this->pitch = pitch;
    
    updateCameraVectors();
}


Camera::Camera(float pos_x, float pos_y, float pos_z, float up_x, float up_y, float up_z, float yaw, float pitch) :
    front(glm::vec3(0.0f, 0.0f, -1.0f)),
    movement_speed(SPEED),
    mouse_sensitivity(SENSITIVITY),
    zoom(ZOOM)
{
    this->position = glm::vec3(pos_x, pos_y, pos_z);
    this->world_up = glm::vec3(up_x, up_y, up_z);
    this->yaw = yaw;
    this->pitch = pitch;

    updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix()
{
    return glm::lookAt(position, position + front, up);
}

void Camera::processKeyboard(Camera_Movement direction, float delta_time)
{
    float velocity = movement_speed * delta_time;

    previous_moves.clear();

    if (direction == FORWARD)
    {
        position += front * velocity; 
        previous_moves.push_back(FORWARD);
    }
    if (direction == BACKWARD)
    {
        position -= front * velocity;
        previous_moves.push_back(BACKWARD);
    }
    if (direction == LEFT)
    {
        position -= right * velocity;
        previous_moves.push_back(LEFT);
    }
    if (direction == RIGHT)
    {
        position += right * velocity;
        previous_moves.push_back(RIGHT);
    }
    if (direction == UP)
    {
        position += up * velocity;
        previous_moves.push_back(UP);
    }
    if (direction == DOWN)
    {
        position -= up * velocity;
        previous_moves.push_back(DOWN);
    }
}

void Camera::processMouseMovement(float x_offset, float y_offset, GLboolean constrain_pitch)
{
    // std::cout << x_offset << " " << y_offset << "\n";
    x_offset *= mouse_sensitivity;
    y_offset *= mouse_sensitivity;

    yaw += x_offset;
    pitch += y_offset;

    if (constrain_pitch)
    {
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;
    }

    updateCameraVectors();
}

void Camera::processMouseScroll(float y_offset)
{
    zoom -= (float)y_offset;
    if (zoom < 1.0f)
        zoom = 1.0f;
    if (zoom > 45.0f)
        zoom = 45.0f; 
}

void Camera::updateCameraVectors()
{
    glm::vec3 front_vector;
    front_vector.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front_vector.y = sin(glm::radians(pitch));
    front_vector.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(front_vector);



    // also re-calculate the Right and Up vector
    right = glm::normalize(glm::cross(front, world_up));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    up    = glm::normalize(glm::cross(right, front));
}

void Camera::undoMove(float delta_time)
{
    // printf("MOVEMENTS: ");
    for (Camera_Movement direction : previous_moves)
    {   
        float velocity = movement_speed * delta_time;
        switch (direction)
        {
            case FORWARD:
                position -= front * velocity;
                // printf("FORWARD ");
                break;
            case BACKWARD:
                position += front * velocity;
                // printf("BACKWARD ");
                break;
            case LEFT:
                position += right * velocity;
                // printf("LEFT ");
                break;
            case RIGHT:
                 position -= right * velocity;
                //  printf("RIGHT ");
                break;
            default:
                break;
        }
    }
    // printf("\n");
}