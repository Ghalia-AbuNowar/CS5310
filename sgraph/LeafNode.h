#ifndef _LEAFNODE_H_
#define _LEAFNODE_H_

#include "AbstractSGNode.h"
#include "SGNodeVisitor.h"
#include "Material.h"
#include "Light.h"
#include "glm/glm.hpp"
#include <map>
#include <stack>
#include <string>
#include <vector>
#include "../Ray.h"
#include "../HitRecord.h"
using namespace std;

namespace sgraph
{

/**
 * This node represents the leaf of a scene graph. It is the only type of node that has
 * actual geometry to render.
 * \author Amit Shesh
 */
class LeafNode: public AbstractSGNode
{
    /**
     * The name of the object instance that this leaf contains. All object instances are stored
     * in the scene graph itself, so that an instance can be reused in several leaves
     */
protected:
    string objInstanceName;
    /**
     * The material associated with the object instance at this leaf
     */
    util::Material material;
    string textureName;

public:

    LeafNode(const string& instanceOf,util::Material& material, const string& name,sgraph::IScenegraph *graph)
        :AbstractSGNode(name,graph) {
        this->objInstanceName = instanceOf;
        this->material = material;
        this->textureName = "";
    }

    LeafNode(const string& instanceOf,const string& name,sgraph::IScenegraph *graph)
        :AbstractSGNode(name,graph) {
        this->objInstanceName = instanceOf;
    }
	
	~LeafNode(){}



    /*
	 *Set the material of each vertex in this object
	 */
    void setMaterial(const util::Material& mat) {
        material = mat;
    }

    /*
     * gets the material
     */
    util::Material getMaterial()
    {
        return material;
    }

    /*
    assign the texture name
    */
    void setTexture(string& texName) {
        textureName = texName;
    }

    /*
    return the texture name
    */
    string getTexture() {
        return textureName;
    }
   

    /**
     * Get the name of the instance this leaf contains
     * 
     * @return string 
     */
    string getInstanceOf() {
        return this->objInstanceName;
    }

    /**
     * Get a copy of this node.
     * 
     * @return SGNode* 
     */

    SGNode *clone() {
        LeafNode *newclone = new LeafNode(this->objInstanceName,material,name,scenegraph);
        return newclone;
    }

    /**
     * Visit this node.
     * 
     */
    void accept(SGNodeVisitor* visitor) {
      visitor->visitLeafNode(this);
    }

    bool intersect(const Ray& ray, HitRecord& hit)
    {


        if(objInstanceName == "cone"){
            return intersectCone(ray,hit);
        }
        if(objInstanceName == "cylinder"){
            return intersectCylinder(ray,hit);
        }
        if (objInstanceName == "box") {
            return intersectBox(ray, hit);
        }
        if (objInstanceName == "sphere") {
            return intersectSphere(ray, hit);
            
        }
        return false;
    }

    bool intersectCylinder(const Ray& ray, HitRecord& hit){
        util::Material objMaterial = this->material;
        // Cylinder center is (0,0,0) and radius = 1 and height is 1
        glm::vec3 oc = ray.origin;  
        float a = ray.direction.x*ray.direction.x +  ray.direction.z*ray.direction.z;
        float b = 2.0f * (oc.x*ray.direction.x +  oc.z*ray.direction.z);
        float c = oc.x*oc.x + oc.z*oc.z - 1.0f;  

        float discriminant = b*b - 4.0f * a * c;
        if (discriminant < 0.0f) {
            // imaginary number
            return false;
        }

        // Compute t-values
        float sqrtDisc = glm::sqrt(discriminant);
        float t0 = (-b - sqrtDisc) / (2.0f * a);
        float t1 = (-b + sqrtDisc) / (2.0f * a);

        // smallest positive t
        // if (t0 > t1) std::swap(t0, t1);

        // If t0 is negative
        float tNear = (t0 < 0.0f) ? t1 : t0;
        if (tNear < 0.0f) {
            // both intersections are behind the ray
            return false;
        }

        glm::vec3 pHit = ray.origin + tNear * ray.direction;
        float yHit = pHit.y;

        if (yHit < 0.0f || yHit > 1.0f) {
            // The intersection is outside the cylinder's height
            return false;
        }

        // If tNear is smaller than previous intersection, tnear is new hit record
        if (tNear < hit.t) {
            glm::vec3 nHit = glm::normalize(glm::vec3(pHit.x, 0.0f, pHit.z));  // Normal is in the xz-plane

            // Update the hit record with the new intersection data
            hit.updateHit(tNear, pHit, nHit, objMaterial);
            return true;
        }


        if (ray.direction.y != 0.0f) {
                float tTop = (1 - oc.y) / ray.direction.y;  // y = 0.5 for top cap
                if (tTop > 0.0f) {
                    glm::vec3 pTop = ray.origin + tTop * ray.direction;
                    if (pTop.x * pTop.x + pTop.z * pTop.z <= 1.0f) {  // Check if within radius
                        if (tTop < hit.t) {
                            // Update hit record for the top cap
                            glm::vec3 nTop = glm::vec3(0.0f, 1.0f, 0.0f);  // Normal for top cap
                            hit.updateHit(tTop, pTop, nTop, objMaterial);
                            return true;
                        }
                    }
                }

                // Check if ray intersects the bottom cap (y = -0.5)
                float tBottom = (- oc.y) / ray.direction.y;  // y = -0.5 for bottom cap
                if (tBottom > 0.0f) {
                    glm::vec3 pBottom = ray.origin + tBottom * ray.direction;
                    if (pBottom.x * pBottom.x + pBottom.z * pBottom.z <= 1.0f) {  // Check if within radius
                        if (tBottom < hit.t) {
                            // Update hit record for the bottom cap
                            glm::vec3 nBottom = glm::vec3(0.0f, -1.0f, 0.0f);  // Normal for bottom cap
                            hit.updateHit(tBottom, pBottom, nBottom, objMaterial);
                            return true;
                        }
                    }
                }
            }

        return false;
 }

    bool intersectBox(const Ray& ray, HitRecord& hit){
        util::Material objMaterial = this->material;
        auto safeDivide = [](float numerator, float denominator) -> float {
        return (denominator == 0.0f) ? ((numerator > 0.0f) ? INFINITY : -INFINITY) : numerator / denominator;
        };

        glm::vec3 oc = ray.origin;

        float tminX = safeDivide((-0.5 - oc.x ) , ray.direction.x);
        float tmaxX = safeDivide((0.5 - oc.x) , ray.direction.x);

        float tminY = safeDivide((-0.5 - oc.y ) , ray.direction.y);
        float tmaxY = safeDivide((0.5 - oc.y) , ray.direction.y);

        float tminZ = safeDivide((-0.5 - oc.z ) , ray.direction.z);
        float tmaxZ = safeDivide((0.5 - oc.z) , ray.direction.z);

        if (tminX > tmaxX) std::swap(tminX, tmaxX);
        if (tminY > tmaxY) std::swap(tminY, tmaxY);
        if (tminZ > tmaxZ) std::swap(tminZ, tmaxZ);

        // max of three mins should be less than min of all three max
        float maxOfMins = max(tminX, tminY);
        maxOfMins = max(maxOfMins,tminZ);

        float minOfMaxs = min(tmaxX, tmaxY);
        minOfMaxs = min(minOfMaxs,tmaxZ);

        if(maxOfMins<0){
            return false;
        }
        else if (tminX<tmaxX && tminY<tmaxY && tminZ<tmaxZ && maxOfMins<minOfMaxs){
            glm::vec3 pHit = ray.origin + maxOfMins * ray.direction;  
            glm::vec3 nHit(0.0f);

            // absolute value of x, y, z
            float absX = fabs(pHit.x);
            float absY = fabs(pHit.y);
            float absZ = fabs(pHit.z);

            if (absX > absY && absX > absZ) {
                // If x is largest the left face or right face was hit
                nHit.x = (pHit.x > 0.0f) ? 1.0f : -1.0f;
            }
            else if (absY > absZ) {
                // If y is the largest top or bottom was hit
                nHit.y = (pHit.y > 0.0f) ? 1.0f : -1.0f;
            }
            else {
                //if z is largest front or back was hit
                nHit.z = (pHit.z > 0.0f) ? 1.0f : -1.0f;
            }              

            hit.updateHit(maxOfMins, pHit, nHit,objMaterial);
            return true;
        }

        return false;
 }

    bool intersectSphere(const Ray& ray, HitRecord& hit){
        util::Material objMaterial = this->material;
        // Sphere center is (0,0,0) and radius = 1
        glm::vec3 oc = ray.origin;  
        float a = glm::dot(ray.direction, ray.direction); 
        float b = 2.0f * glm::dot(oc, ray.direction);
        float c = glm::dot(oc, oc) - 1.0f;  

        float discriminant = b*b - 4.0f * a * c;
        if (discriminant < 0.0f) {
            // imaginary number
            return false;
        }

        // Compute t-values
        float sqrtDisc = glm::sqrt(discriminant);
        float t0 = (-b - sqrtDisc) / (2.0f * a);
        float t1 = (-b + sqrtDisc) / (2.0f * a);

        // smallest positive t
        // if (t0 > t1) std::swap(t0, t1);

        // If t0 is negative
        float tNear = (t0 < 0.0f) ? t1 : t0;
        if (tNear < 0.0f) {
            // both intersections are behind the ray
            return false;
        }

        // If tNear is smaller than previous intersection, tnear is new hit record
        if (tNear < hit.t) {
            glm::vec3 pHit = ray.origin + tNear * ray.direction;  // intersection point
            glm::vec3 nHit = glm::normalize(pHit);                // normal for a unit sphere

            hit.updateHit(tNear, pHit, nHit,objMaterial);
            return true;
        }

        return false;
 }

 bool intersectCone(const Ray& ray, HitRecord& hit) {
    util::Material objMaterial = this->material;
    glm::vec3 oc = ray.origin;

    // Cone equation: x^2 + z^2 = (1 - y)^2
    // Therefore the implicit function is: x^2 + z^2 - (1 - y)^2 = 0

    float dx = ray.direction.x;
    float dy = ray.direction.y;
    float dz = ray.direction.z;
    float ox = oc.x;
    float oy = oc.y;
    float oz = oc.z;

    float k = 1.0f;  // Since radius = height = 1

    // Coefficients of the quadratic equation: At^2 + Bt + C = 0
    float A = dx * dx + dz * dz - k * k * dy * dy;
    float B = 2.0f * (ox * dx + oz * dz - k * k * (1 - oy) * dy);
    float C = ox * ox + oz * oz - k * k * (1 - oy) * (1 - oy);

    float discriminant = B * B - 4.0f * A * C;
    if (discriminant < 0.0f) {
        return false;
    }

    float sqrtDisc = glm::sqrt(discriminant);
    float t0 = (-B - sqrtDisc) / (2.0f * A);
    float t1 = (-B + sqrtDisc) / (2.0f * A);

    float tNear = (t0 < 0.0f) ? t1 : t0;
    if (tNear < 0.0f) return false;

    glm::vec3 pHit = ray.origin + tNear * ray.direction;
    float yHit = pHit.y;

    // Check if y is within the bounds of the cone (0 to 1)
    if (yHit >= 0.0f && yHit <= 1.0f && tNear < hit.t) {
        // Surface normal for a cone (not a trivial computation)
        glm::vec3 nHit = glm::normalize(glm::vec3(pHit.x, 1.0f - pHit.y, pHit.z));
        hit.updateHit(tNear, pHit, nHit, objMaterial);
        return true;
    }

    // Check intersection with the base cap (circle at y = 0)
    if (ray.direction.y != 0.0f) {
        float tCap = -oc.y / ray.direction.y;
        if (tCap > 0.0f) {
            glm::vec3 pCap = ray.origin + tCap * ray.direction;
            if (pCap.x * pCap.x + pCap.z * pCap.z <= 1.0f) {
                if (tCap < hit.t) {
                    glm::vec3 nCap(0.0f, -1.0f, 0.0f);  // Downward normal
                    hit.updateHit(tCap, pCap, nCap, objMaterial);
                    return true;
                }
            }
        }
    }

    return false;
}

};
}
#endif
