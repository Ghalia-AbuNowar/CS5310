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
        // if (objInstanceName == "box") {
        //     return intersectBox(ray, hit);
        // }
        if (objInstanceName == "sphere") {
            return intersectSphere(ray, hit);
            
        }
        return false;
    }

    bool intersectSphere(const Ray& ray, HitRecord& hit){

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
        if (t0 > t1) std::swap(t0, t1);

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

            hit.updateHit(tNear, pHit, nHit);
            return true;
        }

        return false;
 }


   

};
}
#endif
