//
// Created by igork on 03.04.2024.
//

#include "Ray.h"
#include "Collider.h"
#include "ECS/Raycasting/Colliders/BoxCollider.h"
#include "ECS/Raycasting/Colliders/SphereCollider.h"
#include "ECS/Entity.h"
#include "ECS/Grid/Chunk.h"

/**
 * @brief Ray constructor
 * @param origin
 * @param direction
 * @param scene
 */
Ray::Ray(const glm::vec3& origin, const glm::vec3& direction, CollisionSystem *collisionSystem) {
    this->origin = origin;
    this->direction = direction;

    std::vector<Collider*> colliders = collisionSystem->getColliders();
    if (colliders.empty()) {
        spdlog::error("IN RAY CONSTRUCTOR: No colliders found in the scene");
        return;
    }

    std::sort(colliders.begin(), colliders.end(), [origin](Collider* a, Collider* b) {
        return glm::distance(a->parentEntity->transform.getGlobalPosition(), origin) < glm::distance(b->parentEntity->transform.getGlobalPosition(), origin);
    });

    
    rayPoints[0]  = origin.x;
    rayPoints[1]  = origin.y;
    rayPoints[2]  = origin.z;
    rayPoints[3]  = origin.x + direction.x * 10000;
    rayPoints[4]  = origin.y + direction.y * 10000;
    rayPoints[5]  = origin.z + direction.z * 10000;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(rayPoints), &rayPoints, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);
    

    for (auto collider : colliders) {
        if (collider == nullptr) {
            spdlog::error("IN RAY CONSTRUCTOR: Collider is null");
            continue;
        }

        if (doesCollide(collider)) {
            if (collider->type == ColliderType::BOX) {

                if(((BoxCollider*)collider)->collisionType == CollisionType::CHUNK){
                    Chunk * chunk = collider->getEntity()->getComponent<Chunk>();
                    for(int x = 0;x < chunk->width;x++){
                        for(int z = 0;z < chunk->width;z++){
                            Entity* tileEntity = chunk->getTileAt(x,z)->getEntity();
                            if (doesCollide(tileEntity->getComponent<BoxCollider>())){
                                RayHit = GetRayHit(ColliderType::BOX, collider);
                                hitEntity = tileEntity;
                                return;
                            }
                        }
                    }
                }else{
                    RayHit = GetRayHit(ColliderType::BOX, collider);
                    hitEntity = dynamic_cast<Entity*>(collider->parentEntity);
                    break;
                }
            } else if (collider->type == ColliderType::SPHERE) {
                RayHit = GetRayHit(ColliderType::SPHERE, collider);
                hitEntity = dynamic_cast<Entity*>(collider->parentEntity);
                break;
            }
        }
    }
}

Chunk* Ray::checkCollisionWithNeighChunks(Chunk* excluding) {
    auto checkDirection = [&](int offsetX, int offsetZ) -> Chunk* {
        Chunk* chunk = excluding->grid->getChunkAt(excluding->index.x + offsetX, excluding->index.z + offsetZ);
        if (chunk != nullptr && doesCollide(chunk->getEntity()->getComponent<BoxCollider>())) {
            return chunk;
        }
        return nullptr;
    };

    Chunk* result;
    if ((result = checkDirection(1, 0)) != nullptr) return result;
    if ((result = checkDirection(-1, 0)) != nullptr) return result;
    if ((result = checkDirection(0, 1)) != nullptr) return result;
    if ((result = checkDirection(0, -1)) != nullptr) return result;

    return nullptr;
}






/**
 * @brief Returns the point where the ray hits the collider
 * @param type
 * @param collider
 * @return glm::vec3
 */
glm::vec3 Ray::GetRayHit(ColliderType type,Collider* collider) const
{
    if (type == ColliderType::BOX){
        auto* box = dynamic_cast<BoxCollider*>(collider);
        if (!box) {
            spdlog::error("Failed to cast collider to BoxCollider");
            return defaultHit;
        }
        glm::vec3 min = box->getCenter() - box->size;
        glm::vec3 max = box->getCenter() + box->size;
        glm::vec3 tmin = (min - origin) / direction;
        glm::vec3 tmax = (max - origin) / direction;
        glm::vec3 real_min = glm::min(tmin, tmax);
        glm::vec3 real_max = glm::max(tmin, tmax);
        float min_max = glm::max(glm::max(real_min.x, real_min.y), real_min.z);
        float max_min = glm::min(glm::min(real_max.x, real_max.y), real_max.z);
        if (min_max <= max_min)
        {
            return origin + direction * min_max;
        }
    }
    else if(type == ColliderType::SPHERE)
    {
        auto *sphere = dynamic_cast<SphereCollider*>(collider);
        if (!sphere) {
            spdlog::error("Failed to cast collider to SphereCollider");
            return defaultHit;
        }
        glm::vec3 oc = origin - sphere->center;
        float a = glm::dot(direction, direction);
        float b = 2.0f * glm::dot(oc, direction);
        float c = glm::dot(oc, oc) - sphere->radius * sphere->radius;
        float discriminant = b * b - 4 * a * c;
        if (discriminant > 0)
        {
            float t = (-b - sqrt(discriminant)) / (2.0f * a);
            return origin + direction * t;
        }
    
    }
}

/**
 * @brief Checks if the ray collides with the collider
 * @param collider
 * @return bool
 */
bool Ray::doesCollide(Collider* collider) const
{
    if(collider->type == ColliderType::BOX)
    {
        auto* box = dynamic_cast<BoxCollider*>(collider);
        glm::vec3 min = box->getCenter() - box->size;
        glm::vec3 max = box->getCenter() + box->size;
        glm::vec3 tmin = (min - origin) / direction;
        glm::vec3 tmax = (max - origin) / direction;
        glm::vec3 real_min = glm::min(tmin, tmax);
        glm::vec3 real_max = glm::max(tmin, tmax);
        float min_max = glm::max(glm::max(real_min.x, real_min.y), real_min.z);
        float max_min = glm::min(glm::min(real_max.x, real_max.y), real_max.z);
        if (min_max <= max_min)
        {
            return true;
        }
    }
    else if(collider->type == ColliderType::SPHERE)
    {
        auto *sphere = dynamic_cast<SphereCollider*>(collider);
        glm::vec3 oc = origin - sphere->center;
        float a = glm::dot(direction, direction);
        float b = 2.0f * glm::dot(oc, direction);
        float c = glm::dot(oc, oc) - sphere->radius * sphere->radius;
        float discriminant = b * b - 4 * a * c;
        if (discriminant > 0)
        {
            return true;
        }
    }
    return false;
}

/**
 * @brief Returns the point where the ray hits the collider
 * @return glm::vec3
 */
glm::vec3 Ray::RayHitPoint() {
    return RayHit;
}

/**
 * @brief Returns the entity that the ray hit
 * @return Entity*
 */
Entity* Ray::getHitEntity() {
    return hitEntity;
}

void Ray::drawWire(Shader *shader) {
    shader->setMatrix4("model", false, glm::value_ptr(glm::mat4x4(1)));
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 0, 2);
}

Entity* Ray::iterateThruTilesInChunk(Chunk* chunk){
    for(int x = 0;x < chunk->width;x++){
        for(int z = 0;z < chunk->width;z++){
            Entity* tileEntity = chunk->getTileAt(x,z)->getEntity();
            if (doesCollide(tileEntity->getComponent<BoxCollider>())){
                return tileEntity;
            }
        }
    }
    return nullptr;
}





