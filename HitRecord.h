#ifndef HIT_RECORD_H
#define HIT_RECORD_H

#include <glm/glm.hpp>
#include "Ray.h"

class HitRecord {
public:
    // where the hit occurred on the ray
    float t;                
    // 3D point where the ray hit the object
    glm::vec3 point;        
    // Normal vector at the hit point
    glm::vec3 normal;       
    // Whether the ray hit the front face of the object do i need this???
    bool frontFace;         

    // Default constructor
    HitRecord() : t(0.0f), point(0.0f), normal(0.0f), frontFace(false) {}

    // Set the face normal based on the ray direction and outward normal
    inline void setFaceNormal(const Ray& ray, const glm::vec3& outwardNormal) {
        // If the ray and the outward normal point in opposite directions, 
        // then we're hitting the front face of the object
        frontFace = glm::dot(ray.direction, outwardNormal) < 0;
        
        // Set the normal to always point against the ray
        normal = frontFace ? outwardNormal : -outwardNormal;
    }
};

#endif 