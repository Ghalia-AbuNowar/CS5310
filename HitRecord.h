#ifndef HIT_RECORD_H
#define HIT_RECORD_H

#include <glm/glm.hpp>
#include "Ray.h"

class HitRecord {
public:


    float t;                 // Ray parameter at hit
    glm::vec3 point;         // intersection point
    glm::vec3 normal;        // Surface normal at the hit point
    util::Material objMaterial;
    
    HitRecord()
        : t(INFINITY), point(0.0f), normal(0.0f) {}

     // Updates the hit record if the intersection is closer
     void updateHit(float t, const glm::vec3& point, const glm::vec3& normal, util::Material objMaterial) {
        if (t < this->t) {
            this->t = t;
            this->point = point;
            this->normal = normal;
            this->objMaterial = objMaterial;
        }
    }
};
#endif 