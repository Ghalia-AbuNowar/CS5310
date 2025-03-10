#ifndef __VIEW_H__
#define __VIEW_H__

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cstdio>
#include <ShaderProgram.h>
#include "sgraph/SGNodeVisitor.h"
#include "ObjectInstance.h"
#include "PolygonMesh.h"
#include "VertexAttrib.h"
#include "Callbacks.h"
#include "sgraph/IScenegraph.h"

#include <stack>
using namespace std;


class View
{
public:
    View();
    ~View();
    void init(Callbacks* callbacks,map<string,util::PolygonMesh<VertexAttrib>>& meshes);
    void display(sgraph::IScenegraph *scenegraph);
    void callTextRenderer(sgraph::IScenegraph *scenegraph);
    bool shouldWindowClose();
    void closeWindow();
    void setLookAt(glm::mat4 lookAt);

    // method to move drone and reset drone
    void translateDrone(sgraph::IScenegraph *scenegraph, float deltaX, float deltaY, float deltaZ);
    void resetDronePosition(sgraph::IScenegraph *scenegraph);
    void rotateDrone(sgraph::IScenegraph *scenegraph, float yawOffset, float pitchOffset);
    void setCameraMode(int c);



private: 
    glm::mat4 lookAtMatrix;
    GLFWwindow* window;
    util::ShaderProgram program;
    util::ShaderLocationsVault shaderLocations;
    map<string,util::ObjectInstance *> objects;
    glm::mat4 projection;
    stack<glm::mat4> modelview;
    sgraph::SGNodeVisitor *renderer;
    sgraph::SGNodeVisitor *textRenderer;
    int frames;
    double time;
    int cameraMode;
    // copy of the scengraph 
    sgraph::IScenegraph *originalScenegraph = nullptr;



    // speed
    float speed = 1.0f;
    
    // update rotation
    void updateSceneRotation(sgraph::IScenegraph *scenegraph, float currentTime);

    float droneYaw = 0.0f;   
    float dronePitch = 0.0f; 
};

#endif