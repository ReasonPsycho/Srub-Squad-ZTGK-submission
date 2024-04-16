//
// Created by igork on 16.04.2024.
//

#ifndef ZTGK_COLLISIONSYSTEM_H
#define ZTGK_COLLISIONSYSTEM_H


#include "ECS/System.h"
#include "ECS/Component.h"
#include "ECS/Raycasting/Colliders/BoxCollider.h"
#include "ECS/Raycasting/Colliders/SphereCollider.h"
class Collider;
class BoxCollider;
class SphereCollider;

class CollisionSystem : public System {
public:


    CollisionSystem();
    ~CollisionSystem() override = default;

    void addComponent(void *component) override;
    void removeComponent(void *component) override;
    const std::type_index* getComponentTypes() override;
    int getNumComponentTypes() override;

    void showImGuiDetails(Camera *camera) override;


    std::vector<Collider*> getColliders();

    std::unordered_map<unsigned, std::vector<BoxCollider*>> BoxColliders;
    std::unordered_map<unsigned, std::vector<SphereCollider*>> SphereColliders;

private:
    std::array<std::type_index, 2> componentTypes = {
        std::type_index(typeid(BoxCollider)),
        std::type_index(typeid(SphereCollider))
    };
};


#endif //ZTGK_COLLISIONSYSTEM_H