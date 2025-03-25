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
#include "sgraph/LightRenderer.h"
#include "VertexAttrib.h"

#include "Light.h"

#include <unordered_map>


View::View():droneTranslation(glm::translate(glm::mat4(1.0f),glm::vec3(30,100,30))), fieldOfView(60.0f){
cameraMode =1;
}

View::~View(){
    
}

void View::init(Callbacks *callbacks,map<string,util::PolygonMesh<VertexAttrib>>& meshes) 
{
    ViewMeshes = meshes;
    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    window = glfwCreateWindow(800, 800, "Assignment 4", NULL, NULL);
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

    
    glfwSetMouseButtonCallback(window, 
    [](GLFWwindow* window, int button, int action, int mods)
    {
        reinterpret_cast<Callbacks*>(glfwGetWindowUserPointer(window))->mouseCallBack(button,action,mods);
    });

    glfwSetCursorPosCallback(window,
    [](GLFWwindow* window, double xpos, double ypos){
        reinterpret_cast<Callbacks*>(glfwGetWindowUserPointer(window))->onScroll(xpos,ypos);
    });
    
    glfwSetWindowSizeCallback(window, 
    [](GLFWwindow* window, int width,int height)
    {
        
        
        reinterpret_cast<Callbacks*>(glfwGetWindowUserPointer(window))->reshape(width,height);
    });

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);

    initializeShader("shaders/phong-multiple.vert","shaders/phong-multiple.frag",meshes);
    
	int window_width,window_height;
    glfwGetFramebufferSize(window,&window_width,&window_height);

    //prepare the projection matrix for perspective projection
	projection = glm::perspective(glm::radians(60.0f),(float)window_width/window_height,0.1f,10000.0f);
    glViewport(0, 0, window_width,window_height);

    frames = 0;
    time = glfwGetTime();

    
    textRenderer = new sgraph::SceneGraphTextRenderer();
    lightRenderer = new sgraph::LightRenderer();

}

void View::initializeShader(string frag,string vert,map<string,util::PolygonMesh<VertexAttrib>>& meshes){
    // create the shader program
    program.createProgram(frag,
                          vert);
    program.enable();
    shaderLocations = program.getAllShaderVariables();

    initShaderVariables();

    map<string, string> shaderVarsToVertexAttribs;

    shaderVarsToVertexAttribs["vPosition"] = "position";
    shaderVarsToVertexAttribs["vNormal"] = "normal";
    shaderVarsToVertexAttribs["vTexCoord"] = "texcoord";
    
    for (typename map<string,util::PolygonMesh<VertexAttrib> >::iterator it=meshes.begin();
           it!=meshes.end();
           it++) {
        util::ObjectInstance * obj = new util::ObjectInstance(it->first);
        obj->initPolygonMesh(shaderLocations,shaderVarsToVertexAttribs,it->second);
        objects[it->first] = obj;
    }
    renderer = new sgraph::GLScenegraphRenderer(modelview, objects,textureMap, shaderLocations);
}


void View::toggleShader(){
    if(ShaderMode == 0){
        initializeShader("shaders/toon.vert","shaders/toon.frag",ViewMeshes);
        ShaderMode = 1;
    }
    else{
        initializeShader("shaders/phong-multiple.vert","shaders/phong-multiple.frag",ViewMeshes);
        ShaderMode = 0;
    }
}


void View::initShaderVariables() {
  //get input variables that need to be given to the shader program
  for (int i = 0; i < lights.size(); i++)
    {
      LightLocation ll;
      stringstream name;

      name << "light[" << i << "]";
      ll.ambient = shaderLocations.getLocation(name.str() + "" +".ambient");
      ll.diffuse = shaderLocations.getLocation(name.str() + ".diffuse");
      ll.specular = shaderLocations.getLocation(name.str() + ".specular");
      ll.position = shaderLocations.getLocation(name.str() + ".position");
      ll.spotDirection = shaderLocations.getLocation(name.str() + ".spotDirection");
      ll.spotAngle = shaderLocations.getLocation(name.str() + ".spotAngle");

      lightLocations.push_back(ll);
    }
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

    glm::mat4 textureTransform = glm::mat4(1.0f); 
    glUniformMatrix4fv(shaderLocations.getLocation("texturematrix"),
                       1, GL_FALSE,
                       glm::value_ptr(textureTransform));
    
    modelview.push(glm::mat4(1.0));
   if(cameraMode==1){
    modelview.top() = modelview.top() * lookAtMatrix;
    }
    else if(cameraMode == 2){
        float angle = (float)glfwGetTime() * 0.3;

        modelview.top() = modelview.top() * lookAtMatrix * glm::inverse(glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0,1,0))
        * glm::translate(glm::mat4(1.0f), glm::vec3(0,0,30)));

        // apply rotation to frustum 2
        applyFrustum2Rotation(angle);
    }
    else if(cameraMode == 3){
        modelview.top() = modelview.top() * glm::lookAt(glm::vec3(0,5,15),glm::vec3(0,0,500),glm::vec3(0,1,0))
        * glm::inverse(this->getDroneTransformation());
    }

    // collect the lights and clear any previous lights
    lightRenderer->getCollectedLights().clear();
    scenegraph->getRoot()->accept(lightRenderer);
    lights = lightRenderer->getCollectedLights();
    collectedTransformations = lightRenderer->getCollectedTransformations();
    // Print the number of lights collected
    //std::cout << "Number of lights collected: " << lights.size() << std::endl;

    // initialize shader variables with the light count
    lightLocations.clear();
    initShaderVariables();

    // Set the light count 
    glUniform1i(shaderLocations.getLocation("numLights"), lights.size());

    // Apply light properties
    for (int i = 0; i < lights.size(); i++) {
    
        glm::vec4 pos = lights[i].getPosition();
        glm::vec4 direction = lights[i].getSpotDirection();
        cout<<"Pos : "<<pos.x<< " " << pos.y<<" "<<pos.z<< " "<<pos.w<< endl;
        pos = modelview.top() * collectedTransformations[i] * pos;
        direction = modelview.top() * collectedTransformations[i] * direction;
        cout<<"Pos : "<<pos.x<< " " << pos.y<<" "<<pos.z<< " "<<pos.w<< endl;
        cout<<"Spot Direction : "<<lights[i].getSpotDirection().x<< " " << lights[i].getSpotDirection().y<<" "<<lights[i].getSpotDirection().z<< " "<<lights[i].getSpotDirection().w<< endl;
        cout<<"Angle : "<<lights[i].getSpotCutoff()<< endl;
        // position
        glUniform4fv(lightLocations[i].position, 1, glm::value_ptr(pos));
    
        // Set light colors
        glUniform3fv(lightLocations[i].ambient, 1, glm::value_ptr(lights[i].getAmbient()));
        glUniform3fv(lightLocations[i].diffuse, 1, glm::value_ptr(lights[i].getDiffuse()));
        glUniform3fv(lightLocations[i].specular, 1, glm::value_ptr(lights[i].getSpecular()));

        // spot light
        glUniform3fv(lightLocations[i].spotDirection, 1, glm::value_ptr(direction));
        glUniform1f(lightLocations[i].spotAngle, lights[i].getSpotCutoff());
}



    // hides active camera frustum 
    hideActiveFrustum();
    
    // get the time so that you can use it to make a roll
    double currentTime = glfwGetTime();
    double deltaTime = currentTime - lastUpdateTime;
    lastUpdateTime = currentTime;

    // Update roll animation
    updateRolling(deltaTime);

    // Update the prop graph rotation
    updatePropellerRotation("r-blades1", currentTime);
    updatePropellerRotation("r-blades2", currentTime);
    updatePropellerRotation("r-blades3", currentTime);
    updatePropellerRotation("r-blades4", currentTime);


    //send projection matrix to GPU    
    glUniformMatrix4fv(shaderLocations.getLocation("projection"), 1, GL_FALSE, glm::value_ptr(projection));
    

    // //draw scene graph here
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

void View::setLookAt(glm::mat4 lookAt){
    lookAtMatrix = lookAt;
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

/*
changes the speed attribute by delta, used to control propeller and drone speed
*/
void View::setSpeed(int delta){

    // clamp speed to 1
    if (delta < 0 && speed != 1){
        speed += delta;
    }
    else if (delta > 0){
        speed += delta;
    }
    
}

/*
applies drone transformations to be able to use them in the camera
*/
glm::mat4 View::getDroneTransformation(){
    glm::mat4 droneTransformation = glm::mat4(1.0f);
    droneTransformation = droneTransformation * droneTranslation;
    droneTransformation = droneTransformation * droneYRotation;
    droneTransformation = droneTransformation * droneXRotation;
    droneTransformation = droneTransformation * droneZRotation;

    return droneTransformation;
}

/*
    animates propeller rotation
*/
void View::updatePropellerRotation(string nodeName, float currentTime) {

    // speed of rotation and angle
    float rotationSpeed = speed * 200.0f;  
    float rotationAngle = glm::radians(rotationSpeed * currentTime); 

    // Rotate only the specified r-proppeller node
    sgraph::RotateTransform *propellerNode = dynamic_cast<sgraph::RotateTransform *>(cachedNodes[nodeName]);
    if (propellerNode) {
        propellerNode->updateRotation(rotationAngle);
    }
}


/*
    Translates the drone and frustum 3 by changing the translation nodes
*/
void View::translateDrone(float speedSign) {

    // find forward movement direction based on current rotation
    float yawRadians = glm::radians(droneYaw);
    float pitchRadians = glm::radians(dronePitch);

    float forwardX = sin(yawRadians) * cos(pitchRadians) * speed * speedSign;
    float forwardY = sin(pitchRadians) * speed * speedSign;                    
    float forwardZ = cos(yawRadians) * cos(pitchRadians) * speed * speedSign;  

    // Update the drone's translation and droneTranslation matrix
    auto translateNode = dynamic_cast<sgraph::TranslateTransform *>(cachedNodes["t-drone"]);
    translateNode->updateTranslation(forwardX, -forwardY, forwardZ);
    droneTranslation = droneTranslation * glm::translate(glm::mat4(1.0f), glm::vec3(forwardX, -forwardY, forwardZ));

    // Update Frustum 3's translation to follow drone
    auto translateFrustum = dynamic_cast<sgraph::TranslateTransform *>(cachedNodes["t-frustum-3"]);
    translateFrustum->updateTranslation(forwardX, -forwardY, forwardZ);
}

/**
 * Reset Drone and frustum 3 to original position by changing drone transaltion and rotation angle
 */
void View::resetDronePosition() {

    // rest drone position
    if (cachedNodes["r-drone-yaw"]) dynamic_cast<sgraph::RotateTransform *>(cachedNodes["r-drone-yaw"])->resetRotation();
    if (cachedNodes["r-drone-pitch"]) dynamic_cast<sgraph::RotateTransform *>(cachedNodes["r-drone-pitch"])->resetRotation();
    if (cachedNodes["r-drone-roll"]) dynamic_cast<sgraph::RotateTransform *>(cachedNodes["r-drone-roll"])->resetRotation();
    if (cachedNodes["t-drone"]) dynamic_cast<sgraph::TranslateTransform *>(cachedNodes["t-drone"])->resetTranslation();

    //  Reset Frustum 3 (drone frustum) Transformations
    if (cachedNodes["r-frustum-3-yaw"]) dynamic_cast<sgraph::RotateTransform *>(cachedNodes["r-frustum-3-yaw"])->resetRotation();
    if (cachedNodes["r-frustum-3-pitch"]) dynamic_cast<sgraph::RotateTransform *>(cachedNodes["r-frustum-3-pitch"])->resetRotation();
    if (cachedNodes["r-frustum-3-roll"]) dynamic_cast<sgraph::RotateTransform *>(cachedNodes["r-frustum-3-roll"])->resetRotation();
    if (cachedNodes["t-frustum-3"]) dynamic_cast<sgraph::TranslateTransform *>(cachedNodes["t-frustum-3"])->resetTranslation();

    std::cout << "Drone and Frustum 3 reset to initial position!\n";

    //  Reset transformation matrices
    droneTranslation = glm::translate(glm::mat4(1.0f), glm::vec3(30,100,30));
    droneXRotation = glm::mat4(1.0f);
    droneYRotation = glm::mat4(1.0f);
    droneZRotation = glm::mat4(1.0f);

    // rest the drone angle attributes to zero
    droneYaw = 0.0f;
    dronePitch = 0.0f;
    speed = 1;
}

/*
    Rotate Drone and frustum 3 by changing drone rotation node
*/
void View::rotateDrone(float yawOffset, float pitchOffset) {
    // Reverse yaw direction for first-person mode
    if (cameraMode == 3) yawOffset = -yawOffset;  

    // Apply Yaw Rotation
    if (cachedNodes["r-drone-yaw"]) {
        droneYaw += yawOffset;
        dynamic_cast<sgraph::RotateTransform *>(cachedNodes["r-drone-yaw"])->updateRotation(glm::radians(droneYaw));
        droneYRotation = glm::rotate(glm::mat4(1.0f), glm::radians(droneYaw), glm::vec3(0,1,0));
    }

    //  Apply Pitch Rotation
    if (cachedNodes["r-drone-pitch"]) {
        dronePitch += pitchOffset;
        dynamic_cast<sgraph::RotateTransform *>(cachedNodes["r-drone-pitch"])->updateRotation(glm::radians(dronePitch));
        droneXRotation = glm::rotate(glm::mat4(1.0f), glm::radians(dronePitch), glm::vec3(1,0,0));
    }

    //  Apply rotations to Frustum 3
    if (cachedNodes["r-frustum-3-yaw"]) dynamic_cast<sgraph::RotateTransform *>(cachedNodes["r-frustum-3-yaw"])->updateRotation(glm::radians(droneYaw));
    if (cachedNodes["r-frustum-3-pitch"]) dynamic_cast<sgraph::RotateTransform *>(cachedNodes["r-frustum-3-pitch"])->updateRotation(glm::radians(dronePitch));
}

/*
checks if the drone is rolling
*/
void View::rollDrone() {
    if (isRolling) return;  

    isRolling = true;  
    rollAngle = 0.0f;  
}

/*
    roll the drone and frustum in the Y axis of the drone
*/
void View::updateRolling(float deltaTime) {
    // Exit if not rolling
    if (!isRolling) return;  

    // increment roll angle over time
    rollAngle += rollSpeed * deltaTime;
    if (rollAngle >= 360.0f) {
        rollAngle = 0.0f;
        // Stop roll after full rotation
        isRolling = false;  
    }

    //  Update the drone's roll angle
    auto rollNode = dynamic_cast<sgraph::RotateTransform *>(cachedNodes["r-drone-roll"]);
    rollNode->updateRotation(glm::radians(rollAngle));

    // Apply roll to the frustum
    auto frustumRoll = dynamic_cast<sgraph::RotateTransform *>(cachedNodes["r-frustum-3-roll"]);
    frustumRoll->updateRotation(glm::radians(rollAngle));

    //  Store the new roll transformation
    droneZRotation = glm::rotate(glm::mat4(1.0f), glm::radians(rollAngle), glm::vec3(0, 0, 1));
}

/**
 * update the FOV, projection matrix and then update the frustum
 */
void View::updateFOV(float delta) {

    // clamp fov value to avoid glitches
    fieldOfView += delta;
    if (fieldOfView < 30.0f) fieldOfView = 30.0f;
    if (fieldOfView > 120.0f) fieldOfView = 120.0f;

    std::cout << "New FOV: " << fieldOfView << " degrees\n";

    // Update projection matrix
    int window_width, window_height;
    glfwGetFramebufferSize(window, &window_width, &window_height);
    projection = glm::perspective(glm::radians(fieldOfView), 
                                  (float)window_width / window_height, 
                                  0.1f, 10000.0f);

    //  update all frustum scales
    updateFrustumScale();
}

/*
    updates the frustums' shapes by scaling them
*/
void View::updateFrustumScale() {
   
    // new scale values based on FOV
    float frustumScaleFactor = tan(glm::radians(fieldOfView) / 2.0f);  
    float newXScale = frustumScaleFactor * 2.0f;
    float newYScale = frustumScaleFactor * 2.0f;
    float newZScale = frustumScaleFactor * 4.0f;

    // update frustum scale
    dynamic_cast<sgraph::ScaleTransform *>(cachedNodes["s-frustum-1"])->updateScale(newXScale, newYScale, newZScale);
    dynamic_cast<sgraph::ScaleTransform *>(cachedNodes["s-frustum-2"])->updateScale(newXScale, newYScale, newZScale);
    dynamic_cast<sgraph::ScaleTransform *>(cachedNodes["s-frustum-3"])->updateScale(newXScale, newYScale, newZScale);
}

/*Hides active frustum*/
void View::hideActiveFrustum() {
    
    float frustumScaleFactor = tan(glm::radians(fieldOfView) / 2.0f);
    float originalXScale = frustumScaleFactor * 2.0f;
    float originalYScale = frustumScaleFactor * 2.0f;
    float originalZScale = frustumScaleFactor * 4.0f;

    std::string activeFrustum = "";

    // Determine which camera is active
    if (cameraMode == 1) {
        activeFrustum = "s-frustum-1";
    } 
    else if (cameraMode == 2) {
        activeFrustum = "s-frustum-2";
    } 
    else if (cameraMode == 3) {
        activeFrustum = "s-frustum-3";
    }

    // Hide active frustum and show the non-active ones
    for (const std::string& frustumNode : {"s-frustum-1", "s-frustum-2", "s-frustum-3"}) {
        sgraph::ScaleTransform *scaleNode = dynamic_cast<sgraph::ScaleTransform *>(cachedNodes[frustumNode]);
        if (scaleNode) {
            if (frustumNode == activeFrustum) {
                scaleNode->updateScale(0.0f, 0.0f, 0.0f);  
            } else {
                scaleNode->updateScale(originalXScale, originalYScale, originalZScale);  
            }
        }
    }
}

/*
rotate frustum 2 with camera
*/
void View::applyFrustum2Rotation(float angle) {
    if (cachedNodes["r-frustum-2-y"]) { 
        sgraph::RotateTransform *frustumRotationNode = dynamic_cast<sgraph::RotateTransform *>(cachedNodes["r-frustum-2-y"]);
        
        // Update rotation
        frustumRotationNode->updateRotation(angle);
    }
}

void View::setTextures(std::map<std::string, util::TextureImage*> textures) {
    glEnable(GL_TEXTURE_2D);

    for (auto& pair : textures) {
        unsigned int texID;
        glGenTextures(1, &texID);
        glBindTexture(GL_TEXTURE_2D, texID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //if the s-coordinate goes outside (0,1), repeat it
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); //if the t-coordinate goes outside (0,1), repeat it
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);


        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, pair.second->getWidth(), pair.second->getHeight(), 0,
                     GL_RGB, GL_UNSIGNED_BYTE, pair.second->getImage());

        glGenerateMipmap(GL_TEXTURE_2D);
        textureMap[pair.first] = texID;

       
    }
}


/*
    creats a hash of the nodes that are used for this scene to help efficieny
*/
void View::initScenegraphNodes(sgraph::IScenegraph *scenegraph) {
    auto nodes = scenegraph->getNodes();

    // map of nodes
    std::vector<std::string> importantNodes = {
        "r-drone-yaw", "r-drone-pitch", "r-drone-roll", "t-drone", "r-frustum-2-y",
        "r-frustum-3-yaw", "r-frustum-3-pitch", "r-frustum-3-roll", "t-frustum-3",
        "s-frustum-1", "s-frustum-2", "s-frustum-3", "r-blades1", "r-blades2", "r-blades3", "r-blades4"
    };

    // assign values for nodes
    for (const std::string& nodeName : importantNodes) {
        if (nodes.find(nodeName) != nodes.end()) {
            cachedNodes[nodeName] = dynamic_cast<sgraph::TransformNode*>(nodes[nodeName]);
        }
    }
}





