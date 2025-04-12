#ifndef HIT_RECORD_H
#define HIT_RECORD_H

#include <glm/glm.hpp>
#include "Ray.h"
#include "TextureImage.h"

class HitRecord {
public:


    float t;                 // Ray parameter at hit
    glm::vec3 point;         // intersection point
    glm::vec3 normal;        // Surface normal at the hit point
    util::Material objMaterial;

       // Texture info
       glm::vec2 texCoord; // u,v coords in [0,1]
       util::TextureImage* texture = nullptr; 
    
    HitRecord()
        : t(INFINITY), point(0.0f), normal(0.0f),  texCoord(0.0f, 0.0f), texture(nullptr) {}

     // Updates the hit record if the intersection is closer
     void updateHit(float t, const glm::vec3& point, const glm::vec3& normal, util::Material objMaterial, glm::vec2 texCoord = glm::vec2(0.0f), 
     util::TextureImage* texture = nullptr) {
        if (t < this->t) {
            this->t = t;
            this->point = point;
            this->normal = normal;
            this->objMaterial = objMaterial;
            this->texCoord = texCoord;
            this->texture = texture;
        }
    }
};
#endif 