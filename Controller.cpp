#include "Controller.h"
#include "sgraph/IScenegraph.h"
#include "sgraph/Scenegraph.h"
#include "sgraph/GroupNode.h"
#include "sgraph/LeafNode.h"
#include "sgraph/ScaleTransform.h"
#include "ObjImporter.h"
using namespace sgraph;
#include <iostream>
using namespace std;

#include "sgraph/ScenegraphExporter.h"
#include "sgraph/ScenegraphImporter.h"

Controller::Controller(Model& m,View& v):cameraPos(200.0f, 250.0f, 250.0f),
        target(0.0f, 0.0f, 0.0f),
        up(0.0f, 1.0f, 0.0f),
        rotating(false) {
    model = m;
    view = v;
    radius = glm::length(cameraPos - target);
    theta = atan2(cameraPos.z, cameraPos.x);
    phi = asin(cameraPos.y / radius);
    initScenegraph();
}

void Controller::initScenegraph() {

     
    
    //read in the file of commands
    ifstream inFile("scenegraphmodels/assignment3_main.txt");
    //ifstream inFile("tryout.txt");
    sgraph::ScenegraphImporter importer;
    

    IScenegraph *scenegraph = importer.parse(inFile);
    //scenegraph->setMeshes(meshes);
    model.setScenegraph(scenegraph);
    cout <<"Scenegraph made" << endl;   

}

Controller::~Controller()
{
    
}

void Controller::run()
{
    sgraph::IScenegraph * scenegraph = model.getScenegraph();
    map<string,util::PolygonMesh<VertexAttrib> > meshes = scenegraph->getMeshes();
    view.init(this,meshes);
    view.callTextRenderer(scenegraph);
    while (!view.shouldWindowClose()) {
        view.setLookAt(glm::lookAt(cameraPos,target,up));
        view.display(scenegraph);
    }
    view.closeWindow();
    exit(EXIT_SUCCESS);
}

void Controller::onkey(int key, int scancode, int action, int mods)
{
    cout << (char)key << " pressed" << endl;
}

void Controller::reshape(int width, int height) 
{
    cout <<"Window reshaped to width=" << width << " and height=" << height << endl;
    glViewport(0, 0, width, height);
}

void Controller::onScroll(double xpos, double ypos) 
{
    if (!rotating) return;

        float sensitivity = 0.005f;
        float dx = (xpos - lastX) * sensitivity;
        float dy = (ypos - lastY) * sensitivity;

        theta -= dx;
        phi -= dy;
        phi = glm::clamp(phi, glm::radians(-89.0f), glm::radians(89.0f));

        updateCameraPosition();
        lastX = xpos;
        lastY = ypos;
}


void Controller::mouseCallBack(int button,int action,int mods){
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            rotating = true;
            glfwGetCursorPos(glfwGetCurrentContext(), &lastX, &lastY);
        } else if (action == GLFW_RELEASE) {
            rotating = false;
        }
    }
}

void Controller::updateCameraPosition() {
    cameraPos.x = radius * cos(phi) * cos(theta);
    cameraPos.y = radius * sin(phi);
    cameraPos.z = radius * cos(phi) * sin(theta);
}

void Controller::dispose()
{
    view.closeWindow();
}

void Controller::error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}