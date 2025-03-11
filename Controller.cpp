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

Controller::Controller(Model& m,View& v,string filename)
        {
    model = m;
    view = v;


    initScenegraph(filename);
}

void Controller::initScenegraph(string filename) {

     
    
    //read in the file of commands
    ifstream inFile(filename);
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
        view.display(scenegraph);
    }
    view.closeWindow();
    exit(EXIT_SUCCESS);
}

void Controller::onkey(int key, int scancode, int action, int mods)
{

    cout << (char)key << " pressed" << endl;

    if (action == GLFW_PRESS || action == GLFW_REPEAT) {  // Detect key press or hold

        // NOTE THE + and - are the ones on the NUMBERPAD !!!
        switch (key) {
            case GLFW_KEY_1:
                view.setCameraMode(1);
                break;
            case GLFW_KEY_2:
                view.setCameraMode(2);
                break;
            case GLFW_KEY_3:
                view.setCameraMode(3);
                break;

            case GLFW_KEY_F:
                view.setSpeed(1);
                break;

            case GLFW_KEY_S:
                if (mods & GLFW_MOD_SHIFT) {
                    // uppercase "S"
                    view.rollDrone();
                   
                } else {
                    // Lowercase "s" 
                    view.setSpeed(-1);
                }
             
                break;

            case GLFW_KEY_KP_ADD:  // '+' Move forward
                view.translateDrone(model.getScenegraph(), 1.0f);
                break;

            case GLFW_KEY_KP_SUBTRACT:  // '-' Move backward
                view.translateDrone(model.getScenegraph(), -1.0f);
                break;

            case GLFW_KEY_R:  
                view.resetDronePosition(model.getScenegraph());
                break;


            // case GLFW_KEY_R:  // Reset camera
            //     cout << "Resetting trackball" << endl;
            //     cameraPos = initialCameraPos;
            //     theta = initialTheta;
            //     phi = initialPhi;
            //     up = glm::vec3(0.0f, 1.0f, 0.0f);
            //     break;

            case GLFW_KEY_LEFT:  // Rotate left
                view.rotateDrone(model.getScenegraph(),-5.0f, 0.0f);
                break;

            case GLFW_KEY_RIGHT: // Rotate right 
                view.rotateDrone(model.getScenegraph(), 5.0f, 0.0f);
                break;

            case GLFW_KEY_UP:    // Rotate up 
                view.rotateDrone(model.getScenegraph(), 0.0f, -5.0f);
                break;

            case GLFW_KEY_DOWN:  // Rotate down 
                view.rotateDrone(model.getScenegraph(), 0.0f, 5.0f);
                break;
        }
    }

    

    // cout << (char)key << " pressed" << endl;

    // if (action == GLFW_PRESS) { 
    //     if (key == GLFW_KEY_R || key == 'r') {
    //         cout << "Resetting trackball" << endl;

    //         // Reset camera position and angles
    //         cameraPos = initialCameraPos;
    //         theta = initialTheta;
    //         phi = initialPhi;
    //         up = glm::vec3(0.0f, 1.0f, 0.0f);
    
    //     }
    // }
}

void Controller::reshape(int width, int height) 
{
    cout <<"Window reshaped to width=" << width << " and height=" << height << endl;
    glViewport(0, 0, width, height);
}

// void Controller::onScroll(double xpos, double ypos) 
// {
//     if (!rotating) return;

//         float sensitivity = 0.005f;
//         float dx = (xpos - lastX) * sensitivity;
//         float dy = (ypos - lastY) * sensitivity;

//         theta -= dx;
//         phi -= dy;
//         updateCameraPosition();
//         lastX = xpos;
//         lastY = ypos;
// }


// void Controller::mouseCallBack(int button,int action,int mods){
//     if (button == GLFW_MOUSE_BUTTON_LEFT) {
//         if (action == GLFW_PRESS) {
//             rotating = true;
//             glfwGetCursorPos(glfwGetCurrentContext(), &lastX, &lastY);
//         } else if (action == GLFW_RELEASE) {
//             rotating = false;
//         }
//     }
// }

// void Controller::updateCameraPosition() {
    
//     // manually apply rotations to avoid gimbal lock
//     glm::mat4 rotation = glm::mat4(1.0f);

//     // Rotate around the Y
//     rotation = glm::rotate(rotation, theta, glm::vec3(0.0f, 1.0f, 0.0f));

//     // Rotate around the X
//     rotation = glm::rotate(rotation, phi, glm::vec3(1.0f, 0.0f, 0.0f));

//     // Initial z
//     glm::vec4 initialPos(0.0f, 0.0f, radius, 1.0f);

//     // Apply rotation and update camera
//     glm::vec4 transformedPos = rotation * initialPos;
//     cameraPos = target + glm::vec3(transformedPos);

//     // Compute up vector
//     glm::vec3 newUp = glm::vec3(rotation * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
//     up = newUp;
// }



void Controller::dispose()
{
    view.closeWindow();
}

void Controller::error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}