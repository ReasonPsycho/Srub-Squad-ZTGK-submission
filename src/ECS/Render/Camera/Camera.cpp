//
// Created by redkc on 21/11/2023.
//

#include "Camera.h"
#include "GLFW/glfw3.h"
#include "ECS/Utils/Time.h"
#include <glm/gtx/rotate_vector.hpp>

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch, float nearClip, float farClip) : Front(
        glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM), nearClip(
        nearClip), farClip(farClip) {
    Position = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(
        glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
    Position = glm::vec3(posX, posY, posZ);
    WorldUp = glm::vec3(upX, upY, upZ);
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix() {
    return glm::lookAt(Position, Position + Front, Up);
}

void Camera::MoveCamera(GLFWwindow *window) {
    float velocity = MovementSpeed * (float) Time::Instance().DeltaTime();

    if(debugMovement){
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            Position += Front * velocity;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            Position -= Front * velocity;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            Position -= Right * velocity;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            Position += Right * velocity;
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            Position += Up * velocity;
        if (glfwGetKey(window, GLFW_KEY_C) ==GLFW_PRESS)
            Position -= Up * velocity;  
    }else{
        bool no_mods = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) != GLFW_PRESS
                && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) != GLFW_PRESS
                && glfwGetKey(window, GLFW_KEY_LEFT_ALT) != GLFW_PRESS
                && glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) != GLFW_PRESS
                && glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) != GLFW_PRESS
                && glfwGetKey(window, GLFW_KEY_RIGHT_ALT) != GLFW_PRESS;

        if (!no_mods) return;

        glm::vec3 front;
        front.x = cos(glm::radians(Yaw));
        front.y = 0;   // ignore pitch
        front.z = sin(glm::radians(Yaw));
        front = glm::normalize(front);

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            Position += front * velocity;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            Position -= front * velocity;

        glm::vec3 right = glm::normalize(glm::cross(front, Up));
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            Position += right * velocity;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            Position -= right * velocity;

        if (Position.x > MAX_X_POS)
            Position = glm::vec3(MAX_X_POS,Position.y,Position.z) ;
        if (Position.z > MAX_Z_POS)
            Position = glm::vec3(Position.x,Position.y,MAX_Z_POS);
        if (Position.y > MAX_Y_POS)
            Position = glm::vec3(Position.x,MAX_Y_POS,Position.z) ;
        if (Position.y < MIN_Y_POS)
            Position = glm::vec3(Position.x,MIN_Y_POS,Position.z) ;
        if (Position.x < MIN_X_POS)
            Position = glm::vec3(MIN_X_POS,Position.y,Position.z) ;
        if (Position.z < MIN_Z_POS)
            Position = glm::vec3(Position.x,Position.y,MIN_Z_POS);
    }
}

void Camera::MoveCamera(float xPos, float yPos) {
    if(!debugMovement){
        float velocity = MovementSpeed * (float) Time::Instance().DeltaTime();
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw));
        front.y = 0;   // ignore pitch
        front.z = sin(glm::radians(Yaw));
        front = glm::normalize(front);
        glm::vec3 right = glm::normalize(glm::cross(front, Up));
        
        if (xPos > saved_display_w - PanZoneSize)
            Position += right * velocity;
        if (xPos < PanZoneSize)
            Position -= right * velocity;
        if ( yPos > saved_display_h - PanZoneSize)
            Position -= front * velocity;
        if (yPos < PanZoneSize)
            Position += front * velocity;

        if (Position.x > MAX_X_POS)
            Position = glm::vec3(MAX_X_POS,Position.y,Position.z) ;
        if (Position.z > MAX_Z_POS)
            Position = glm::vec3(Position.x,Position.y,MAX_Z_POS);
        if (Position.y > MAX_Y_POS)
            Position = glm::vec3(Position.x,MAX_Y_POS,Position.z) ;
        if (Position.y < MIN_Y_POS)
            Position = glm::vec3(Position.x,MIN_Y_POS,Position.z) ;
        if (Position.x < MIN_X_POS)
            Position = glm::vec3(MIN_X_POS,Position.y,Position.z) ;
        if (Position.z < MIN_Z_POS)
            Position = glm::vec3(Position.x,Position.y,MIN_Z_POS);
        
    }
}


void Camera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch, double deltaTime) {

    xoffset *= MouseSensitivity * deltaTime;
    yoffset *= MouseSensitivity * deltaTime;
    if(debugMovement) {
        Yaw += xoffset;
        Pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch) {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }
    }else{
        
    }
    // UpdateImpl Front, Right and Up Vectors using the updated Euler angles
    updateCameraVectors();
}

void Camera::ProcessMouseScroll(float yoffset, float deltaTime) {
    Zoom -= (float) yoffset * deltaTime;
    if (Zoom < 1.0f)
        Zoom = 1.0f;
    if (Zoom > 45.0f)
        Zoom = 45.0f;
}

void Camera::updateCameraVectors() {
    // calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);
    // also re-calculate the Right and Up vector
    Right = glm::normalize(glm::cross(Front,
                                      WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    Up = glm::normalize(glm::cross(Right, Front));
}

void Camera::UpdateShader(Shader *shader) {
    shader->use();
    // pass projection matrix to shader 
    glm::mat4 projection = GetProjectionMatrix();
    shader->setMatrix4("projection", false, glm::value_ptr(projection));

    // camera/view transformation
    glm::mat4 view = GetViewMatrix();
    shader->setMatrix4("view", false, glm::value_ptr(view));
}

void Camera::UpdateShader(Shader *shader, int display_w, int display_h) {
    saved_display_w = display_w;
    saved_display_h = display_h;
    shader->use();
    // pass projection matrix to shader 
    glm::mat4 projection = GetProjectionMatrix();
    shader->setMatrix4("projection", false, glm::value_ptr(projection));

    // camera/view transformation
    glm::mat4 view = GetViewMatrix();
    shader->setMatrix4("view", false, glm::value_ptr(view));
}

glm::mat4 Camera::GetProjectionMatrix() {
    return glm::perspective(glm::radians(Zoom), (float) saved_display_w / (float) saved_display_h, nearClip, farClip);
}

void Camera::UpdateCamera(int display_w, int display_h) {
    saved_display_w = display_w;
    saved_display_h = display_h;
}


glm::vec3 Camera::getDirFromCameraToCursor(float mouseX, float mouseY, int screenWidth, int screenHeight) {
    // Calculate normalized device coordinates (NDC)
    float x = (2.0f * mouseX) / screenWidth - 1.0f;
    float y = 1.0f - (2.0f * mouseY) / screenHeight;
    float z = 1.0f;

    // Convert NDC to clip space
    glm::vec3 ray_nds = glm::vec3(x, y, z);
    glm::vec4 ray_clip = glm::vec4(ray_nds.x, ray_nds.y, -1.0, 1.0);

    // Convert clip space to eye space
    glm::vec4 ray_eye = glm::inverse(GetProjectionMatrix()) * ray_clip;
    ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);

    // Convert eye space to world space (use updated view matrix)
    glm::mat4 viewMatrix = GetViewMatrix();
    glm::vec3 ray_wor = glm::vec3(glm::inverse(viewMatrix) * ray_eye);
    ray_wor = glm::normalize(ray_wor);

    return ray_wor;
}

float Camera::GetAspectRatio() {
    return (float ) saved_display_w/(float )saved_display_h;
}

void Camera::MoveCamera(float scroll) {
    if(!debugMovement){
        float velocity = MovementSpeed * scroll *(float) Time::Instance().DeltaTime();

        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front = glm::normalize(front);
        glm::vec3 right = glm::normalize(glm::cross(front, Up));
        
        auto tmpPosition =    Position + front * velocity;

        if (tmpPosition.y > MAX_Y_POS)
            return;
        if (tmpPosition.y < MIN_Y_POS)
            return;

        Position = tmpPosition;

        if (Position.x > MAX_X_POS)
            Position = glm::vec3(MAX_X_POS,Position.y,Position.z) ;
        if (Position.z > MAX_Z_POS)
            Position = glm::vec3(Position.x,Position.y,MAX_Z_POS);
        if (Position.y > MAX_Y_POS)
            Position = glm::vec3(Position.x,MAX_Y_POS,Position.z) ;
        if (Position.y < MIN_Y_POS)
            Position = glm::vec3(Position.x,MIN_Y_POS,Position.z) ;
        if (Position.x < MIN_X_POS)
            Position = glm::vec3(MIN_X_POS,Position.y,Position.z) ;
        if (Position.z < MIN_Z_POS)
            Position = glm::vec3(Position.x,Position.y,MIN_Z_POS);

    }
}

void Camera::Rotate(bool clockwise, float angle){
    float radians = glm::radians(angle);

    // Determine the direction of rotation
    if (clockwise) {
        radians = -radians;
    }

    // Calculate the point in front of the camera (for rotation pivot)
    glm::vec3 pivot = Position + Front * 50.0f; // 10 units in front of the camera

    // Translate the camera to the origin (relative to the pivot)
    glm::vec3 translatedPosition = Position - pivot;

    // Perform the rotation around the Y axis
    float cosAngle = cos(radians);
    float sinAngle = sin(radians);
    float newX = translatedPosition.x * cosAngle - translatedPosition.z * sinAngle;
    float newZ = translatedPosition.x * sinAngle + translatedPosition.z * cosAngle;
    translatedPosition.x = newX;
    translatedPosition.z = newZ;

    // Translate the camera back to its original position (relative to the pivot)
    Position = translatedPosition + pivot;

    // Update the camera's Yaw angle
    Yaw += (clockwise ? -angle : angle);

    // Ensure Yaw stays within the bounds of 0 to 360 degrees
    if (Yaw > 360.0f) {
        Yaw -= 360.0f;
    } else if (Yaw < 0.0f) {
        Yaw += 360.0f;
    }

    // Update camera vectors using the updated Yaw
    updateCameraVectors();
}





















