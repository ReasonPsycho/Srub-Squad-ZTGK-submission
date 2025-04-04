//
// Created by redkc on 07/06/2024.
//

#include "ColorMask.h"

void ColorMask::AddMask(std::string name, glm::vec4 color, float timer) {
    if (maskDataMap.find(name) == maskDataMap.end()) {  //If value doesn't already exist
        maskDataMap[name] = ColorMaskData(color, timer);
    } else {
        spdlog::info("This already has this color mask!");
        maskDataMap[name] = ColorMaskData(color, timer);
    }
}

void ColorMask::AddMask(std::string name, glm::vec4 color) {
    if (maskDataMap.find(name) == maskDataMap.end()) {  //If value doesn't already exist
        maskDataMap[name] = ColorMaskData(color);
    } else {
        spdlog::info("This already has this color mask!: " + name);
        maskDataMap[name] = ColorMaskData(color);
    }
}

void ColorMask::RemoveMask(std::string name) {
    if (maskDataMap.find(name) != maskDataMap.end()) {  //If value doesn't already exist
        maskDataMap.erase(name);
    } else {
        spdlog::info("This doesn't have this color mask!: " + name);
    }
}

void ColorMask::UpdateImpl() {
    glm::vec4 currentColorMask = glm::vec4(0);
    if (!maskDataMap.empty()) {
        auto it = maskDataMap.begin();
        currentColorMask = it->second.color;
        it++;
        
        while (it != maskDataMap.end()) {
            for (int i = 0; i < 3; ++i) {
                currentColorMask[i] = currentColorMask[i] * currentColorMask[3] + it->second.color[i] * it->second.color[3];
            }
            currentColorMask[3] = (currentColorMask[3] + it->second.color[3])/2;
            it++;
        }
        it = maskDataMap.begin();
        
        while (it != maskDataMap.end()) {
            if (!it->second.constant) {
                it->second.timer = it->second.timer - (float) Time::Instance().DeltaTime();
                if (it->second.timer <= 0) {
                    maskDataMap.erase(it++);
                } else {
                    ++it;
                }
            } else {
                ++it;
            }
        }
    }
    Render *render = getEntity()->getComponent<Render>();
    if (render != nullptr) {
        render->colorMask = currentColorMask;
    }
}


void ColorMask::showImGuiDetailsImpl(Camera *camera) {
    ImGui::InputText("##Name", input_buf, sizeof(input_buf));
    ImGui::Checkbox("Is constant", &isConstant);
    ImGui::ColorPicker4("Pick color:", color);
    ImGui::InputFloat("Set timer:", &timer);

    if (ImGui::Button("Add timer")) {
        if (isConstant) {
            AddMask(input_buf, glm::vec4(color[0], color[1], color[2], color[3]));
        } else {
            AddMask(input_buf, glm::vec4(color[0], color[1], color[2], color[3]), timer);
        }
    }
}

ColorMask::ColorMask() {
    name = "Color mask";
}

bool ColorMask::HasMask(std::string name) {
    return maskDataMap.find(name) != maskDataMap.end();
}

void ColorMask::ChangeMaskColor(std::string name, glm::vec4 color) {
    if (maskDataMap.find(name) != maskDataMap.end()) {  //If value doesn't already exist
        maskDataMap[name].color = color;
    } else {
        spdlog::info("This doesn't have this color mask!: " + name);
    }
}

glm::vec4 ColorMask::GetMaskColor(std::string name) {
    if (maskDataMap.find(name) != maskDataMap.end()) {  //If value doesn't already exist
        return maskDataMap[name].color;
    } else {
        spdlog::info("This doesn't have this color mask!: " + name);
        return glm::vec4(0);
    }
}
