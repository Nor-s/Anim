#include "camera.h"

namespace glcpp
{
    // constructor with vectors
    Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
        : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }
    // constructor with scalar values
    Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
        : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 Camera::GetViewMatrix() const
    {

        glm::mat4 mat = glm::lookAt(Position, Position + Front, Up);
        mat = glm::rotate(mat, xAngle, glm::vec3(1.0f, 0.0f, 0.0f));
        return glm::rotate(mat, yAngle, glm::vec3(0.0f, 1.0f, 0.0f));
    }

    void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
            Position += Up * velocity;
        if (direction == BACKWARD)
            Position -= Up * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;
    }

    void Camera::ProcessMouseMovement(float x_angle, float y_angle)
    {
        xAngle += x_angle * SENSITIVITY;
        yAngle += y_angle * SENSITIVITY;
    }

    void Camera::ProcessMouseScroll(float yoffset)
    {
        if (yoffset > 0)
            Position += Front * MovementSpeed;
        if (yoffset < 0)
            Position -= Front * MovementSpeed;
    }
    void Camera::ProcessMouseScrollPress(float yoffset, float xoffset, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime * 0.5f;
        Position += Up * velocity * yoffset;
        Position -= Right * velocity * xoffset;
    }

    void Camera::updateCameraVectors()
    {
        // calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp)); // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up = glm::normalize(glm::cross(Right, Front));
    }
}