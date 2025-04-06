#ifndef RAY_SCENEGRAPH_RENDERER_H
#define RAY_SCENEGRAPH_RENDERER_H

#include "SGNodeVisitor.h"
#include "GroupNode.h"
#include "LeafNode.h"
#include "TransformNode.h"
#include "RotateTransform.h"
#include "ScaleTransform.h"
#include "TranslateTransform.h"

#include "../Ray.h"
#include "../HitRecord.h"

#include <stack>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <iostream>
#include <limits>

namespace sgraph {
/**
 * A visitor that traverses the scene graph and checks ray intersections
 */
class RayScenegraphRenderer : public sgraph::SGNodeVisitor
{
public:
    /**
     * takes model view stack as a parameter
     */
    RayScenegraphRenderer(std::stack<glm::mat4>& mv)
        : modelview(mv)
    {
        activeRay = nullptr;     
        activeHit = nullptr;     
    }

    /**
     * Set a Ray and HitRecord
     */
    void setRayAndHit(const Ray* ray, HitRecord* hitRecord)
    {
        activeRay = ray;
        activeHit = hitRecord;

        // set the hit back to infinity
        if (activeHit) {
            activeHit->t = std::numeric_limits<float>::infinity();
        }
    }

    void visitGroupNode(sgraph::GroupNode *groupNode) override
    {
        for (int i = 0; i < groupNode->getChildren().size(); i++) {
            groupNode->getChildren()[i]->accept(this);
        }
    }

    void visitTransformNode(sgraph::TransformNode * transformNode) override
    {
        modelview.push(modelview.top());
        modelview.top() = modelview.top() * transformNode->getTransform();

        if (!transformNode->getChildren().empty()) {
            transformNode->getChildren()[0]->accept(this);
        }

        modelview.pop();
    }

    void visitScaleTransform(sgraph::ScaleTransform *scaleNode) override
    {
        visitTransformNode(scaleNode);
    }

    void visitRotateTransform(sgraph::RotateTransform *rotateNode) override
    {
        visitTransformNode(rotateNode);
    }

    void visitTranslateTransform(sgraph::TranslateTransform *translateNode) override
    {
        visitTransformNode(translateNode);
    }

    /**
     * ray-object intersection and transform the ray into local space using inverse(modelview.top()).
     */
    void visitLeafNode(sgraph::LeafNode *leafNode) override
    {

        // No idea How correct this is but it seems to be working?
        // If no ray is set
        if (!activeRay || !activeHit) {
            return;
        }

        // Invert the current modelview to go from world ro local
        glm::mat4 invMV = glm::inverse(modelview.top());

        // Transform the activeRay into local coords
        glm::vec4 locO = invMV * glm::vec4(activeRay->origin, 1.0f);
        glm::vec4 locD = invMV * glm::vec4(activeRay->direction, 0.0f);
        Ray localRay;
        localRay.origin    = glm::vec3(locO);
        localRay.direction = glm::normalize(glm::vec3(locD));

        // intersection check inside LeafNode 
        HitRecord localHit;
        localHit.t = std::numeric_limits<float>::infinity();

        bool didHit = leafNode->intersect(localRay, localHit);
        if (didHit) {
            // Convert localHit's intersection point & normal back to world space
            glm::vec4 worldP = modelview.top() * glm::vec4(localHit.point, 1.0f);
            glm::vec4 worldN = glm::transpose(glm::inverse(modelview.top()))
                               * glm::vec4(localHit.normal, 0.0f);

            // The distance from activeRay's start to the intersection
            float dist = glm::length(glm::vec3(worldP) - activeRay->origin);

            // update activeHit if it is closer
            if (dist < activeHit->t) {
                activeHit->t = dist;
                activeHit->point  = glm::vec3(worldP);
                activeHit->normal = glm::normalize(glm::vec3(worldN));
            }
        }
    }

private:
    std::stack<glm::mat4>& modelview;
    const Ray* activeRay; 
    HitRecord* activeHit;
};

}
#endif 
