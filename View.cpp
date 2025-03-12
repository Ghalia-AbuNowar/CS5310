#include "View.h"
#include <cstdio>
#include <cstdlib>
#include <vector>
using namespace std;
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "sgraph/GLScenegraphRenderer.h"
#include "sgraph/SceneGraphTextRenderer.h"
#include "VertexAttrib.h"

View::View(){
cameraMode =1;
}

View::~View(){
    
}

void View::init(Callbacks *callbacks,map<string,util::PolygonMesh<VertexAttrib>>& meshes) 
{
    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    window = glfwCreateWindow(800, 800, "Hello GLFW: Per-vertex coloring", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
     glfwSetWindowUserPointer(window, (void *)callbacks);

    //using C++ functions as callbacks to a C-style library
    glfwSetKeyCallback(window, 
    [](GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        reinterpret_cast<Callbacks*>(glfwGetWindowUserPointer(window))->onkey(key,scancode,action,mods);
    });

    
    // glfwSetMouseButtonCallback(window, 
    // [](GLFWwindow* window, int button, int action, int mods)
    // {
    //     reinterpret_cast<Callbacks*>(glfwGetWindowUserPointer(window))->mouseCallBack(button,action,mods);
    // });

    // glfwSetCursorPosCallback(window,
    // [](GLFWwindow* window, double xpos, double ypos){
    //     reinterpret_cast<Callbacks*>(glfwGetWindowUserPointer(window))->onScroll(xpos,ypos);
    // });
    
    glfwSetWindowSizeCallback(window, 
    [](GLFWwindow* window, int width,int height)
    {
        
        
        reinterpret_cast<Callbacks*>(glfwGetWindowUserPointer(window))->reshape(width,height);
    });

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);

    // create the shader program
    program.createProgram(string("shaders/default.vert"),
                          string("shaders/default.frag"));
    // assuming it got created, get all the shader variables that it uses
    // so we can initialize them at some point
    // enable the shader program
    program.enable();
    shaderLocations = program.getAllShaderVariables();

    
    /* In the mesh, we have some attributes for each vertex. In the shader
     * we have variables for each vertex attribute. We have to provide a mapping
     * between attribute name in the mesh and corresponding shader variable
     name.
     *
     * This will allow us to use PolygonMesh with any shader program, without
     * assuming that the attribute names in the mesh and the names of
     * shader variables will be the same.

       We create such a shader variable -> vertex attribute mapping now
     */
    map<string, string> shaderVarsToVertexAttribs;

    shaderVarsToVertexAttribs["vPosition"] = "position";
    
    
    for (typename map<string,util::PolygonMesh<VertexAttrib> >::iterator it=meshes.begin();
           it!=meshes.end();
           it++) {
        util::ObjectInstance * obj = new util::ObjectInstance(it->first);
        obj->initPolygonMesh(shaderLocations,shaderVarsToVertexAttribs,it->second);
        objects[it->first] = obj;
    }
    
	int window_width,window_height;
    glfwGetFramebufferSize(window,&window_width,&window_height);

    //prepare the projection matrix for perspective projection
	projection = glm::perspective(glm::radians(60.0f),(float)window_width/window_height,0.1f,10000.0f);
    glViewport(0, 0, window_width,window_height);

    frames = 0;
    time = glfwGetTime();

    renderer = new sgraph::GLScenegraphRenderer(modelview,objects,shaderLocations);
    textRenderer = new sgraph::SceneGraphTextRenderer();
}

void View::callTextRenderer(sgraph::IScenegraph *scenegraph){
    scenegraph->getRoot()->accept(textRenderer);
}


void View::display(sgraph::IScenegraph *scenegraph) {
    
    program.enable();
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    //glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_FRONT_FACE);

    
    
    modelview.push(glm::mat4(1.0));
   if(cameraMode==1){
    modelview.top() = modelview.top() * glm::lookAt(glm::vec3(200.0f, 250.0f, 250.0f),glm::vec3(0,0,0),glm::vec3(0,1,0));
    }
    else if(cameraMode == 2){
        modelview.top() = modelview.top() * glm::lookAt(glm::vec3(0, 500.0f, 0),glm::vec3(0,0,0),glm::vec3(1,0,0));
    }
    else if(cameraMode == 3){
        modelview.top() = modelview.top() * glm::lookAt(glm::vec3(0.0f, 5.0f, 5.0f),glm::vec3(0,0,500),glm::vec3(0,1,0))
        * glm::inverse(this->getDroneTransformation());
    }

    // get the time so that you can use it to make a roll
    double currentTime = glfwGetTime();
    double deltaTime = currentTime - lastUpdateTime;
    lastUpdateTime = currentTime;

    // Update roll animation
    updateRolling(scenegraph, deltaTime);

    // Update the prop graph rotation
    updatePropellerRotation(scenegraph, "r-blades1", currentTime);
    updatePropellerRotation(scenegraph, "r-blades2", currentTime);
    updatePropellerRotation(scenegraph, "r-blades3", currentTime);
    updatePropellerRotation(scenegraph, "r-blades4", currentTime);


    //send projection matrix to GPU    
    glUniformMatrix4fv(shaderLocations.getLocation("projection"), 1, GL_FALSE, glm::value_ptr(projection));
    

    //draw scene graph here
    scenegraph->getRoot()->accept(renderer);

    
    
    modelview.pop();
    glFlush();
    program.disable();
    
    glfwSwapBuffers(window);
    glfwPollEvents();
    frames++;
    double currenttime = glfwGetTime();
    if ((currenttime-time)>1.0) {
        printf("Framerate: %2.0f\r",frames/(currenttime-time));
        frames = 0;
        time = currenttime;
    }
    

}

bool View::shouldWindowClose() {
    return glfwWindowShouldClose(window);
}


void View::setCameraMode(int c){
    cameraMode = c;
}
void View::closeWindow() {
    for (map<string,util::ObjectInstance *>::iterator it=objects.begin();
           it!=objects.end();
           it++) {
          it->second->cleanup();
          delete it->second;
    } 
    glfwDestroyWindow(window);

    glfwTerminate();
}

void View::setSpeed(int delta){

    if (delta < 0 && speed != 1){
        speed += delta;
    }
    else if (delta > 0){
        speed += delta;
    }
    
}

glm::mat4 View::getDroneTransformation(){
    glm::mat4 droneTransformation = glm::mat4(1.0f);
    droneTransformation = droneTransformation * glm::translate(glm::mat4(1.0f),glm::vec3(30,100,30));
    droneTransformation = droneTransformation * droneZRotation;
    droneTransformation = droneTransformation * droneYRotation;
    droneTransformation = droneTransformation * droneXRotation;
    return droneTransformation;
}

void View::updatePropellerRotation(sgraph::IScenegraph *scenegraph, string nodeName, float currentTime) {

    // Rotation speed (adjust if needed)
    float rotationSpeed = speed * 200.0f;  // Increase speed to see changes

    // Compute new rotation angle 
    float rotationAngle = glm::radians(rotationSpeed * currentTime); 

    // Find the node inside the scenegraph
    auto nodes = scenegraph->getNodes();
    if (nodes.find(nodeName) != nodes.end()) {
        // cast the node to RotateTransform
        sgraph::RotateTransform *node = dynamic_cast<sgraph::RotateTransform *>(nodes[nodeName]); 

        // Check if the cast was successful
        if (node) {
            node->updateRotation(rotationAngle);
        } else {
            std::cerr << "Error: Node is not a RotateTransform!" << std::endl;
        }
    }

}


void View::translateDrone(sgraph::IScenegraph *scenegraph, float speedSign) {
    auto nodes = scenegraph->getNodes();

    // get the left/right rotation
    float yawRadians = 0.0f;
    if (nodes.find("r-drone-yaw") != nodes.end()) {
        sgraph::RotateTransform *yawNode = dynamic_cast<sgraph::RotateTransform *>(nodes["r-drone-yaw"]);
        if (yawNode) {
            yawRadians = glm::radians(droneYaw);  
        }
    }

    // get the up/down rotation
    float pitchRadians = 0.0f;
    if (nodes.find("r-drone-pitch") != nodes.end()) {
        sgraph::RotateTransform *pitchNode = dynamic_cast<sgraph::RotateTransform *>(nodes["r-drone-pitch"]);
        if (pitchNode) {
            pitchRadians = glm::radians(dronePitch);  
        }
    }

    // find the forward direction based on the rotation and speed of propellers
    float forwardX = sin(yawRadians) * cos(pitchRadians) * speed * speedSign;
    float forwardY = sin(pitchRadians) * speed * speedSign;                    
    float forwardZ = cos(yawRadians) * cos(pitchRadians) * speed * speedSign;  

    // translate in proper direction
    if (nodes.find("t-drone") != nodes.end()) {
        sgraph::TranslateTransform *translateNode = dynamic_cast<sgraph::TranslateTransform *>(nodes["t-drone"]);
        if (translateNode) {
            translateNode->updateTranslation(forwardX, -forwardY, forwardZ);
            droneTranslation = glm::translate(glm::mat4(1.0f),glm::vec3(forwardX, -forwardY, forwardZ));
        }
    }

}



/**
 * Reset Drone to original position
 */
void View::resetDronePosition(sgraph::IScenegraph *scenegraph) {
    auto nodes = scenegraph->getNodes();

    // reset the rotation 
    if (nodes.find("r-drone-yaw") != nodes.end()) {
        sgraph::RotateTransform *yawNode = dynamic_cast<sgraph::RotateTransform *>(nodes["r-drone-yaw"]);
        if (yawNode) {
            yawNode->resetRotation();
        }
    }


    // Reset rotation
    if (nodes.find("r-drone-pitch") != nodes.end()) {
        sgraph::RotateTransform *pitchNode = dynamic_cast<sgraph::RotateTransform *>(nodes["r-drone-pitch"]);
        if (pitchNode) {
            pitchNode->resetRotation();
        }
    }

    // reset translation
    if (nodes.find("t-drone") != nodes.end()) {
        sgraph::TranslateTransform *translateNode = dynamic_cast<sgraph::TranslateTransform *>(nodes["t-drone"]);
        if (translateNode) {
            translateNode->resetTranslation();  //  Reset drone position
            std::cout << " Drone reset to initial position!\n";

        }
    }

    // rest the drone angle attributes to zero
    droneYaw = 0.0f;
    dronePitch = 0.0f;
    speed = 1;
}

void View::rotateDrone(sgraph::IScenegraph *scenegraph, float yawOffset, float pitchOffset) {

    auto nodes = scenegraph->getNodes();
    
    // horizontal rotation around Y-axis
    if (nodes.find("r-drone-yaw") != nodes.end()) {
        sgraph::RotateTransform *yawNode = dynamic_cast<sgraph::RotateTransform *>(nodes["r-drone-yaw"]);
        if (yawNode) {
            droneYaw += yawOffset;
            yawNode->updateRotation(glm::radians(droneYaw));
            droneYRotation = glm::rotate(glm::mat4(1.0f),droneYaw,glm::vec3(0,1,0));
        }
    }

    // vertical rotation around X-axis
    if (nodes.find("r-drone-pitch") != nodes.end()) {
        sgraph::RotateTransform *pitchNode = dynamic_cast<sgraph::RotateTransform *>(nodes["r-drone-pitch"]);
        if (pitchNode) {
        
            dronePitch += pitchOffset;
            pitchNode->updateRotation(glm::radians(dronePitch));
            droneXRotation = glm::rotate(glm::mat4(1.0f),dronePitch,glm::vec3(1,0,0));
        }
    }
}

void View::rollDrone() {
    if (isRolling) return;  

    isRolling = true;  
    rollAngle = 0.0f;  
}

void View::updateRolling(sgraph::IScenegraph *scenegraph, float deltaTime) {
    if (!isRolling) return;  // Exit if not rolling

    auto nodes = scenegraph->getNodes();
    if (nodes.find("r-drone-roll") != nodes.end()) {
        sgraph::RotateTransform *rollNode = dynamic_cast<sgraph::RotateTransform *>(nodes["r-drone-roll"]);
        if (rollNode) {
            rollAngle += rollSpeed * deltaTime;  //  change roll angle
            rollNode->updateRotation(glm::radians(rollAngle));

            //  Stop roll after completing 360 degrees
            if (rollAngle >= 360.0f) {
                rollAngle = 0.0f;

                //  Reset roll
                isRolling = false;  

                //  Reset to original position
                rollNode->updateRotation(0.0f);  
            }
        }
    }
}












