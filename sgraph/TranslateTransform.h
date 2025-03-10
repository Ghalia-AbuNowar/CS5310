#ifndef _TRANSLATETRANSFORM_H_
#define _TRANSLATETRANSFORM_H_


#include "TransformNode.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

namespace sgraph {
    /**
     * This node represents a specific kind of transform: translation
     * 
     */
    class TranslateTransform: public TransformNode {
        protected:
            float tx,ty,tz;

            //  Store initial translation values
            float initialTx, initialTy, initialTz;  


        ParentSGNode *copyNode() {
            return new TranslateTransform(tx,ty,tz,name,scenegraph);
        }

        public:
            TranslateTransform(float tx,float ty,float tz,const string& name,sgraph::IScenegraph *graph) 
                :TransformNode(name,graph) {
                    this->tx = tx;
                    this->ty = ty;
                    this->tz = tz;

                    this->initialTx = tx;
                    this->initialTy = ty;
                    this->initialTz = tz;
                    glm::mat4 transform = glm::translate(glm::mat4(1.0),glm::vec3(tx,ty,tz));
                    setTransform(transform);
            }

            
            /**
             * Visit this node.
             * 
             */
            void accept(SGNodeVisitor* visitor) {
                visitor->visitTranslateTransform(this);
            }

            glm::vec3 getTranslate() {
            return glm::vec3(tx,ty,tz);
        }

        /*
        updates the translation matrix by delta
        */
        void updateTranslation(float deltaX, float deltaY, float deltaZ) {

            this->tx += deltaX;
            this->ty += deltaY;
            this->tz += deltaZ;

            // update transform
            this->transform = glm::translate(glm::mat4(1.0f), glm::vec3(tx, ty, tz));
        }

        /*reset to original values NOT WORKING YET*/

        void resetTranslation() {
            //  Reset the translation values to the original ones
            this->tx = initialTx;
            this->ty = initialTy;
            this->tz = initialTz;
        
            glm::mat4 transform = glm::translate(glm::mat4(1.0),glm::vec3(tx,ty,tz));
            setTransform(transform);       

        }


    };
}

#endif