#ifndef _SCALETRANSFORM_H_
#define _SCALETRANSFORM_H_

#include "SGNode.h"
#include "TransformNode.h"
#include "IScenegraph.h"
#include <glm/gtc/matrix_transform.hpp>

namespace sgraph {
    /**
     * This node represents a specific type of transform: scale
     * 
     */
    class ScaleTransform: public TransformNode {
    protected:
        float sx,sy,sz;
        float initialSX, initialSY, initialSZ;

        ParentSGNode *copyNode() {
            return new ScaleTransform(sx,sy,sz,name,scenegraph);
        }    

    public:
        ScaleTransform(float sx,float sy,float sz,const string& name,sgraph::IScenegraph *graph) 
            :TransformNode(name,graph) {
                this->sx = sx;
                this->sy = sy;
                this->sz = sz;

                // save initial values 
                this->initialSX = sx;
                this->initialSY = sy;
                this->initialSZ = sz;
                
                glm::mat4 transform = glm::scale(glm::mat4(1.0),glm::vec3(sx,sy,sz));
                setTransform(transform);
        }

        

        /**
         * Visit this node.
         * 
         */
        void accept(SGNodeVisitor* visitor) {
        return visitor->visitScaleTransform(this);
        }

        glm::vec3 getScale() {
            return glm::vec3(sx,sy,sz);
        }


        /*
        updates the scale node
        */
       void updateScale(float newX, float newY, float newZ) {

        this->sx = newX;
        this->sy = newY;
        this->sz = newZ;

        // update transform
        transform = glm::scale(glm::mat4(1.0f), glm::vec3(sx, sy, sz));
        setTransform(transform);   
    }

   
        /*reset to original values*/

        void resetScale() {
            //  Reset the scale values to the original ones
            this->sx = initialSX;
            this->sy = initialSY;
            this->sz = initialSZ;
        
            transform = glm::scale(glm::mat4(1.0),glm::vec3(sx,sy,sz));
            setTransform(transform);       

        }

    };
}

#endif