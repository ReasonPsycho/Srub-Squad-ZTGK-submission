//
// Created by redkc on 16/01/2024.
//

#include "ECS/Entity.h"
#include "SpotLight.h"

void SpotLight::Innit(int width, int height, int index) {
    glm::mat4 lightProjection, lightView;
    lightProjection = glm::perspective(glm::radians(45.0f), (GLfloat) width / (GLfloat) height,
                                       near_plane,
                                       far_plane); // note that if you use a perspective projection matrix you'll have to change the light position as the current light position isn't enough to reflect the whole scene
    //lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);


    glm::vec3 lightPos = glm::vec3(data.position.x, data.position.y, data.position.z);


    glm::vec3 eulerAngles = data.direction;
    glm::vec3 direction = glm::vec3(0, 0, 0);
    direction = glm::normalize(direction); // Normalize vector

    lightView = glm::lookAt(lightPos, lightPos + direction, glm::vec3(0.0, 1.0,
                                                                      0.0)); //TODO when shadows gonna be here change that to appropriate things
// Take in mind that glm::lookAt requires a position where the camera is located, a target where it should look at
// and an up vector to decide where is your top. Most likely, that it should be glm::vec3(0.0f, 1.0f, 0.0f)
    data.lightSpaceMatrix = lightProjection * lightView;
    // render scene from light's point of view
}

void SpotLight::SetUpShadowBuffer(Shader *shadowMapShader,
                                  GLuint ShadowMapArrayId, int index, int layer) {
    ZoneScopedN("SetUpShadowBuffer");

    shadowMapShader->use();
    shadowMapShader->setMatrix4("lightSpaceMatrix", false, glm::value_ptr(data.lightSpaceMatrix));
    shadowMapShader->setFloat("far_plane", far_plane);
    shadowMapShader->setFloat("near_plane", near_plane);
    
    glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, ShadowMapArrayId, 0, index);
    glClear(GL_DEPTH_BUFFER_BIT);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
}

SpotLight::SpotLight(SpotLightData data) : data(data) {
    name = "Spot light";

    lightType = Spot;
}

void SpotLight::showImGuiDetailsImpl(Camera *camera) {
    ImGui::ColorEdit4("Diffuse", glm::value_ptr(data.diffuse));
    ImGui::ColorEdit4("Specular", glm::value_ptr(data.specular));
    ImGui::InputFloat("Constant", &data.constant);
    ImGui::InputFloat("Linear", &data.linear);
    ImGui::InputFloat("Quadratic", &data.quadratic);
    ImGui::InputFloat("Cut off", &data.cutOff);
    ImGui::InputFloat("Outer cut Off", &data.outerCutOff);
    if(ImGui::Button("Update")){
        this->setIsDirty(true);
    }
}

void SpotLight::UpdateData(int height, int width) {
    glm::mat4 lightProjection, lightView;
    far_plane = ComputeFarPlane(data.constant, data.linear, data.quadratic);
    data.position = glm::vec4(getEntity()->transform.getGlobalPosition(),far_plane);

    lightProjection = glm::perspective(glm::radians(45.0f), (GLfloat) width / (GLfloat) height, near_plane, far_plane);
    data.direction = glm::vec4(-getEntity()->transform.getForward(), 0.0f);

    glm::vec3 lightPos = glm::vec3(data.position);
    glm::vec3 target = lightPos + glm::vec3(data.direction) * far_plane;

    // To define the 'up' direction for lookAt function 
    // you will generally use glm::vec3(0.0f, 1.0f, 0.0f) if your world's 'up' vector is along y-axis
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    lightView = glm::lookAt(lightPos, target, up);

    data.lightSpaceMatrix = lightProjection * lightView;


    this->setIsDirty(false);
}

SpotLight::SpotLight() : ILight(), data(SpotLightData()) {
    name = "Spot light";
    lightType = Spot;
}
