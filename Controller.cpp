#include "Controller.h"
#include "sgraph/IScenegraph.h"
#include "sgraph/Scenegraph.h"
#include "sgraph/GroupNode.h"
#include "sgraph/LeafNode.h"
#include "sgraph/ScaleTransform.h"
#include "ObjImporter.h"
#include "Light.h"

#include "ImageLoader.h"
#include "PPMImageLoader.h"

using namespace sgraph;
#include <iostream>
using namespace std;

#include "sgraph/ScenegraphExporter.h"
#include "sgraph/ScenegraphImporter.h"

Controller::Controller(Model& m,View& v,string filename):
        rotating(false) 
        {
    cameraMode = 1;
    if(cameraMode == 1){
        cameraPos = glm::vec3(200,250,250);
        target = glm::vec3(0,0,0);
        up = glm::vec3(0,1,0);
    }
    else if(cameraMode == 2){
        cameraPos = glm::vec3(0,300,0);
        target = glm::vec3(0,0,0);
        up = glm::vec3(1,0,0);
    }
    else if(cameraMode == 3){
        cameraPos = glm::vec3(0,5,15);
        target = glm::vec3(0,0,500);
        up = glm::vec3(0,1,0);
    }
    model = m;
    view = v;
    copyFilename = filename;


    radius = glm::length(cameraPos - target);
    theta = atan2(cameraPos.z, cameraPos.x);
    phi = asin(cameraPos.y / radius);

    // store init values for reset  
    initialCameraPos = cameraPos;
    initialTheta = theta;
    initialPhi = phi;

    initScenegraph(filename);
}

void Controller::initScenegraph(string filename) {

    
    //read in the file of commands
    ifstream inFile(filename);
    //ifstream inFile("tryout.txt");
    sgraph::ScenegraphImporter importer;

    IScenegraph *scenegraph = importer.parse(inFile);
    // map<string,util::Light> lights = importer.getLights();
    // view.setLights(lights);
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
    
    loadTextures();  
    view.init(this,meshes);

   
    view.setTextures(textures);


    view.callTextRenderer(scenegraph);
    // initialize nodes 
    view.initScenegraphNodes(scenegraph);
    view.initLights(scenegraph);
    while (!view.shouldWindowClose()) {
        view.setLookAt(glm::lookAt(cameraPos,target,up));
        view.display(scenegraph);
    }
    view.closeWindow();
    exit(EXIT_SUCCESS);
}

void Controller::resetCameraLookAt(int c){
    if(c == 1){
        cameraPos = glm::vec3(200,250,250);
        target = glm::vec3(0,0,0);
        up = glm::vec3(0,1,0);
    }
    else if(c == 2){
        cameraPos = glm::vec3(0,500,0);
        target = glm::vec3(0,0,0);
        up = glm::vec3(1,0,0);
    }
    else if(c == 3){
        cameraPos = glm::vec3(0,5,15);
        target = glm::vec3(0,0,500);
        up = glm::vec3(0,1,0);
    }
}

void Controller::loadTextures()
{
    glm::mat4 transform, textureTransform;
    

    
    // Checkerboard texture hard coded for now
    PPMImageLoader loader1;
    loader1.load("textures/grass.ppm");
    util::TextureImage* grass = new util::TextureImage(loader1.getPixels(), loader1.getWidth(), loader1.getHeight(), "grass");
    textures["grass"] = grass;

    //  White texture hard coded for now
    PPMImageLoader loader2;
    loader2.load("textures/earthmap.ppm");
    util::TextureImage* earthmap = new util::TextureImage(loader2.getPixels(), loader2.getWidth(), loader2.getHeight(), "earthmap");
    textures["earthmap"] = earthmap;

    loader1.load("textures/checkerboard.ppm");
    util::TextureImage* checkerboard = new util::TextureImage(loader1.getPixels(), loader1.getWidth(), loader1.getHeight(), "checkerboard");
    textures["checkerboard"] = checkerboard;

    loader1.load("textures/CastleWall.ppm");
    util::TextureImage* CastleWall = new util::TextureImage(loader1.getPixels(), loader1.getWidth(), loader1.getHeight(), "CastleWall");
    textures["CastleWall"] = CastleWall;

    loader1.load("textures/white.ppm");
    util::TextureImage* white = new util::TextureImage(loader1.getPixels(), loader1.getWidth(), loader1.getHeight(), "white");
    textures["white"] = white;

    loader1.load("textures/roof.ppm");
    util::TextureImage* roof = new util::TextureImage(loader1.getPixels(), loader1.getWidth(), loader1.getHeight(), "roof");
    textures["roof"] = roof;

}


void Controller::onkey(int key, int scancode, int action, int mods)
{

    cout << (char)key << " pressed" << endl;
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {  

        switch (key) {
            // press 1 to use camera 1
            case GLFW_KEY_1:
                cameraMode = 1;
                resetCameraLookAt(1);
                view.setCameraMode(1);
                break;

            // press 2 to use camera 2
            case GLFW_KEY_2:
                cameraMode = 2;
                resetCameraLookAt(2);
                view.setCameraMode(2);
                break;
            
            // press 3 to use camera 3 
            case GLFW_KEY_3:
                cameraMode = 3;
                resetCameraLookAt(3);
                view.setCameraMode(3);
                break;

            // press F to increase prop speed
            case GLFW_KEY_F:
                view.setSpeed(1);
                break;

            // press S to decrease prop speed
            case GLFW_KEY_S:
                view.setSpeed(-1);
                break;

            // press J to roll drone
            case GLFW_KEY_J:
                view.rollDrone();
                break;

            // press = to move drone forward
            case GLFW_KEY_EQUAL:  
                view.translateDrone(1.0f);
                break;
            
            // press - to move drone backwards
            case GLFW_KEY_MINUS:  
                view.translateDrone(-1.0f);
                break;

            // press D to reset drone to original position
            case GLFW_KEY_D:
                view.resetDronePosition();
                break;

            // press R to reset camera
            case GLFW_KEY_R:  
                cout << "Resetting trackball" << endl;
                cameraPos = initialCameraPos;
                theta = initialTheta;
                phi = initialPhi;
                up = glm::vec3(0.0f, 1.0f, 0.0f);
                break;

            // press left arro button to rotate drone left
            case GLFW_KEY_LEFT:  // Rotate left
                view.rotateDrone(-5.0f, 0.0f);
                break;

            // press right arrow button to rotate drone right
            case GLFW_KEY_RIGHT: // Rotate right 
                view.rotateDrone(5.0f, 0.0f);
                break;

            // press upp arrow button to rotate drone up
            case GLFW_KEY_UP:    // Rotate up 
                view.rotateDrone(0.0f, -5.0f);
                break;

            // press doen arrow button to rotate drone down
            case GLFW_KEY_DOWN:  // Rotate down 
                view.rotateDrone(0.0f, 5.0f);
                break;

            // press Z to increase FOV
            case GLFW_KEY_Z:
                view.updateFOV(5.0f);
                break;
            
            // press Y to decrease FOV
            case GLFW_KEY_Y:
                view.updateFOV(-5.0f);
                break;

            case GLFW_KEY_P:
                view.toggleShader();
                break;
        }
    }

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
    
    // manually apply rotations to avoid gimbal lock
    glm::mat4 rotation = glm::mat4(1.0f);

    // Rotate around the Y
    rotation = glm::rotate(rotation, theta, glm::vec3(0.0f, 1.0f, 0.0f));

    // Rotate around the X
    rotation = glm::rotate(rotation, phi, glm::vec3(1.0f, 0.0f, 0.0f));

    // Initial z
    glm::vec4 initialPos(0.0f, 0.0f, radius, 1.0f);

    // Apply rotation and update camera
    glm::vec4 transformedPos = rotation * initialPos;
    cameraPos = target + glm::vec3(transformedPos);

    // Compute up vector
    glm::vec3 newUp = glm::vec3(rotation * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
    up = newUp;
}



void Controller::dispose()
{
    view.closeWindow();
}

void Controller::error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}