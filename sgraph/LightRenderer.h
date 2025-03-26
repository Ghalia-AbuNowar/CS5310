#ifndef _LIGHTRENDERER_H_
#define _LIGHTRENDERER_H_

#include "SGNodeVisitor.h"
#include "GroupNode.h"
#include "LeafNode.h"
#include "TransformNode.h"
#include "RotateTransform.h"
#include "ScaleTransform.h"
#include "TranslateTransform.h"
#include <iostream>
#include <vector>

using namespace std;
namespace sgraph {
    /**
     * This visitor implements drawing the scene graph using OpenGL
     * 
     */
    class LightRenderer: public SGNodeVisitor {
        public:
        /**
         * @brief Construct a new GLScenegraphRenderer object
         * 
         * @param mv a reference to modelview stack that will be used while rendering
         * @param os the map of ObjectInstance objects
         * @param shaderLocations the shader locations for the program used to render
         */
        LightRenderer(){
            LightModelView.push(glm::mat4(1.0f));
            check = 0;
        }

        /**
         * @brief Recur to the children for drawing
         * 
         * @param groupNode 
         */
        void visitGroupNode(GroupNode *groupNode) {
            collectNodeLights(groupNode);
            for (int i=0;i<groupNode->getChildren().size();i=i+1) {
                groupNode->getChildren()[i]->accept(this);
            }
        }

        /**
         * @brief Draw the instance for the leaf, after passing the 
         * modelview and color to the shader
         * 
         * @param leafNode 
         */
        void visitLeafNode(LeafNode *leafNode) {
            collectNodeLights(leafNode);
        }

        /**
         * @brief Multiply the transform to the modelview and recur to child
         * 
         * @param transformNode 
         */
        void visitTransformNode(TransformNode * transformNode) {
            LightModelView.push(LightModelView.top());
            LightModelView.top() = LightModelView.top() * transformNode->getTransform();
            collectNodeLights(transformNode);
            
            if (transformNode->getChildren().size()>0) {
                transformNode->getChildren()[0]->accept(this);
            }
            LightModelView.pop();
        }

        /**
         * @brief For this visitor, only the transformation matrix is required.
         * Thus there is nothing special to be done for each type of transformation.
         * We delegate to visitTransformNode above
         * 
         * @param scaleNode 
         */
        void visitScaleTransform(ScaleTransform *scaleNode) {
            visitTransformNode(scaleNode);

        }

        /**
         * @brief For this visitor, only the transformation matrix is required.
         * Thus there is nothing special to be done for each type of transformation.
         * We delegate to visitTransformNode above
         * 
         * @param translateNode 
         */
        void visitTranslateTransform(TranslateTransform *translateNode) {
            visitTransformNode(translateNode);
        }

        void visitRotateTransform(RotateTransform *rotateNode) {
            visitTransformNode(rotateNode);
        }

        /*
        helper function to a
        */
        void collectNodeLights(SGNode* node) {
            auto nodeLights = node->getLights();
    
            if (!nodeLights.empty()) {  
                glm::mat4 transformation = LightModelView.top(); 

                for (auto& light : nodeLights) {
                    collectedLights.push_back(light);
                    collectedTransformations.push_back(transformation);
                    lightNodeNames.push_back(node->getName());
                }
            }
        }
        
        vector<util::Light>& getCollectedLights() {
            return collectedLights;
        }

        vector<glm::mat4>& getCollectedTransformations() {
            return collectedTransformations;
        }

        vector<string>& getLightNodeNames() {
            return lightNodeNames;
        }



        private:
        int check;
        stack<glm::mat4> LightModelView;
        vector<util::Light> collectedLights;
        vector<glm::mat4> collectedTransformations;
        vector<string> lightNodeNames;

   };
}

#endif