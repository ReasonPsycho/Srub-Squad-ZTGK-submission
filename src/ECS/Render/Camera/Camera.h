//
// Created by redkc on 21/11/2023.
//

#ifndef OPENGLGP_CAMERA_H
#define OPENGLGP_CAMERA_H

#include "glm/vec3.hpp"
#include "glm/trigonometric.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "ECS/Render/ModelLoading/Shader.h"
#include "GLFW/glfw3.h"

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UPWARD,
    DOWNWARD
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 50.0f;
const float SENSITIVITY = 5.0f;
const float ZOOM = 45.0f;
const float NEARCLIP = 0.1f;
const float FARCLIP = 400.0f;

const float MAX_X_POS = 220;
const float MIN_X_POS = -20;
const float MAX_Y_POS = 30;
const float MIN_Y_POS = 5;
const float MAX_Z_POS = 220;
const float MIN_Z_POS = -20;

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera {
public:
    // camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    float nearClip;
    float farClip;
    // euler Angles
    float Yaw;
    float Pitch;
    // camera options
    float MovementSpeed;
    int PanZoneSize = 80;
    float MouseSensitivity;
    float Zoom;

    bool debugMovement = false;
    
    //Constructors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
           float yaw = YAW, float pitch = PITCH, float nearClip = NEARCLIP, float farClip = FARCLIP);

    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

    // Returns view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix();

    glm::mat4 GetProjectionMatrix();
    float GetAspectRatio();

    //InputProcessing added update_delta time with predefintion of one (for cases where it takes to long to implement it)
    void MoveCamera(GLFWwindow *window);
    void MoveCamera(float xPos, float yPos);
    void MoveCamera(float scroll);

    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true, double deltaTime = 1);

    void ProcessMouseScroll(float yoffset, float deltaTime = 1);

    //updates shader values
    void UpdateShader(Shader *shader, int display_w, int display_h);

    void UpdateShader(Shader *shader);
    void UpdateCamera(int display_w, int display_h);

    glm::vec3 getDirFromCameraToCursor(float mouseX, float mouseY, int screenWidth, int screenHeight);

    int saved_display_w = 1920;
    int saved_display_h = 1080;

    void Rotate(bool clockwise, float angle = 3);
private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors();

};


#endif //OPENGLGP_CAMERA_H
