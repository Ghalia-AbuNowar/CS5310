#ifndef RAY_H
#define RAY_H

#include <glm/glm.hpp>

class Ray {
public:
    // Origin point of the ray
    glm::vec3 origin;

    // Direction vector of the ray 
    glm::vec3 direction;
    
    // Default constructor
    Ray() : origin(0.0f), direction(0.0f, 0.0f, -1.0f) {}
    
    // Constructor with specific origin and direction
    Ray(const glm::vec3& o, const glm::vec3& d) : origin(o), direction(glm::normalize(d)) {}
    
    // Get the point at t on the ray
    glm::vec3 pointAt(float t) const {
        return origin + t * direction;
    }
};

#endif 