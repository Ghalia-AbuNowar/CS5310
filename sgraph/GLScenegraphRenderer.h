#ifndef _GLSCENEGRAPHRENDERER_H_
#define _GLSCENEGRAPHRENDERER_H_

#include "SGNodeVisitor.h"
#include "GroupNode.h"
#include "LeafNode.h"
#include "TransformNode.h"
#include "RotateTransform.h"
#include "ScaleTransform.h"
#include "TranslateTransform.h"
#include <ShaderProgram.h>
#include <ShaderLocationsVault.h>
#include "ObjectInstance.h"
#include <stack>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

namespace sgraph {
    /**
     * This visitor implements drawing the scene graph using OpenGL
     * 
     */
    class GLScenegraphRenderer: public SGNodeVisitor {
        public:
        /**
         * @brief Construct a new GLScenegraphRenderer object
         * 
         * @param mv a reference to modelview stack that will be used while rendering
         * @param os the map of ObjectInstance objects
         * @param shaderLocations the shader locations for the program used to render
         */
        GLScenegraphRenderer(stack<glm::mat4>& mv,map<string,util::ObjectInstance *>& os,util::ShaderLocationsVault& shaderLocations) 
            : modelview(mv)
            , objects(os) {
            this->shaderLocations = shaderLocations;
            // LightModelView.push(glm::mat4(1.0f));
            for (map<string,util::ObjectInstance *>::iterator it=objects.begin();it!=objects.end();it++) {
                cout << "Mesh with name: "<< it->first << endl;
            }
        }

        /**
         * @brief Recur to the children for drawing
         * 
         * @param groupNode 
         */
        void visitGroupNode(GroupNode *groupNode) {
            // collectNodeLights(groupNode);
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
            // collectNodeLights(leafNode);
            //send modelview matrix to GPU  
            // glUniformMatrix4fv(shaderLocations.getLocation("modelview"), 1, GL_FALSE, glm::value_ptr(modelview.top()));
            // glUniform4fv(shaderLocations.getLocation("vColor"),1,glm::value_ptr(leafNode->getMaterial().getAmbient()));
            util::Material mat = leafNode->getMaterial();
            glm::mat4 normalmatrix = glm::inverse(glm::transpose((modelview.top())));
            //send modelview matrix to GPU  
            glUniformMatrix4fv(shaderLocations.getLocation("modelview"), 1, GL_FALSE, glm::value_ptr(modelview.top()));
            glUniformMatrix4fv(shaderLocations.getLocation("normalmatrix"), 1, false,glm::value_ptr(normalmatrix));

            glUniform3fv(shaderLocations.getLocation("material.ambient"), 1, glm::value_ptr(mat.getAmbient()));
            glUniform3fv(shaderLocations.getLocation("material.diffuse"), 1, glm::value_ptr(mat.getDiffuse()));
            glUniform3fv(shaderLocations.getLocation("material.specular"), 1,glm::value_ptr(mat.getSpecular()));
            glUniform1f(shaderLocations.getLocation("material.shininess"), mat.getShininess());
            objects[leafNode->getInstanceOf()]->draw();
        }

        /**
         * @brief Multiply the transform to the modelview and recur to child
         * 
         * @param transformNode 
         */
        void visitTransformNode(TransformNode * transformNode) {
            // LightModelView.push(LightModelView.top());
            modelview.push(modelview.top());
            // LightModelView.top() = LightModelView.top() * transformNode->getTransform();
            modelview.top() = modelview.top() * transformNode->getTransform();
            // collectNodeLights(transformNode);
            if (transformNode->getChildren().size()>0) {
                transformNode->getChildren()[0]->accept(this);
            }
            modelview.pop();
            // LightModelView.pop();
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
        // void collectNodeLights(SGNode* node) {
        //     auto nodeLights = node->getLights();
    
        //     if (!nodeLights.empty()) {  
        //         glm::mat4 transformation = LightModelView.top(); 

        //         for (auto& light : nodeLights) {
        //             collectedLights.push_back(light);
        //             collectedTransformations.push_back(transformation);
        //         }
        //     }
        // }
        
        // vector<util::Light>& getCollectedLights() {
        //     return collectedLights;
        // }

        // vector<glm::mat4>& getCollectedTransformations() {
        //     return collectedTransformations;
        // }

        private:
        // stack<glm::mat4> LightModelView;
        stack<glm::mat4>& modelview;    
        util::ShaderLocationsVault shaderLocations;
        map<string,util::ObjectInstance *> objects;
        // vector<util::Light> collectedLights;
        // vector<glm::mat4> collectedTransformations;

   };
}

#endif