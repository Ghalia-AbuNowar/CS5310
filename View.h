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
#include "Light.h"
#include "sgraph/IScenegraph.h"
#include "sgraph/GLScenegraphRenderer.h"
#include "TextureImage.h"
#include "sgraph/LightRenderer.h"
#include <glm/glm.hpp>
#include "sgraph/RayScenegraphRenderer.h"


#include <unordered_map>
#include <stack>
using namespace std;

enum RenderMode {
    OPENGL,
    RAYTRACE
};

class View
{
    class LightLocation {
    public:
        int ambient,diffuse,specular,position;
        int spotDirection, spotAngle;
        LightLocation()
        {
        ambient = diffuse = specular = position = -1;
        spotDirection = spotAngle = -1;
        }

    };

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
    void translateDrone(float speedSign);
    void resetDronePosition();
    void rotateDrone(float yawOffset, float pitchOffset);
    
    // set camera
    void setCameraMode(int c);

    // update rotation
    void updatePropellerRotation(string nodeName, float currentTime);

    // propeller speed control
    void setSpeed(int delta);

    // checks if the drone is rolling
    void rollDrone();

    //  causes the drone to roll
    void updateRolling(float deltaTime);

    // retrives the mat4 matrix for the drone transformations
    glm::mat4 getDroneTransformation();

    // update the fov angle
    void updateFOV(float delta);

    // updates the frustum scale
    void updateFrustumScale();

    // hides the active camera frustum
    void hideActiveFrustum();

    // initialize nodes i need to use
    void initScenegraphNodes(sgraph::IScenegraph *scenegraph);

    void applyFrustum2Rotation(float angle);

    void initShaderVariables(); 

    void setTextures(std::map<std::string, util::TextureImage*> textures);

    // void setLights(map<string,util::Light> lights);

    void toggleShader();
    void initializeShader(string frag,string vert,map<string,util::PolygonMesh<VertexAttrib>>& meshes);
    void lightTranformation();
    void initLights(sgraph::IScenegraph *scenegraph);

    // ray tracer
    void toggleRenderMode();

    // preform ray tracing
    void performRayTracing(sgraph::IScenegraph *scenegraph);

    // set up camera for ray tracing 
    Ray generateCameraRay(int x, int y, int width, int height) ;

    // shade to calculate color of point
    glm::vec4 shade(vector<util::Light> lights, glm::vec3 position,glm::vec3 normal, util::Material objMaterial);


private: 

    vector<glm::mat4> collectedTransformations;

    
    glm::mat4 lookAtMatrix;
    GLFWwindow* window;
    util::ShaderProgram program;
    util::ShaderProgram program2;
    util::ShaderLocationsVault shaderLocations;

    vector<util::Light> lights;
    //the shader locations for all lights, for convenience
    vector<LightLocation> lightLocations;
    vector<string> lightNodeNames;


    map<string,util::PolygonMesh<VertexAttrib>> ViewMeshes;
    map<string,util::ObjectInstance *> objects;
    glm::mat4 projection;
    stack<glm::mat4> modelview;
    sgraph::GLScenegraphRenderer* renderer;
    sgraph::SGNodeVisitor *textRenderer;
    sgraph::LightRenderer *lightRenderer;
    int frames;
    double time;
    int cameraMode;
    // copy of the scengraph 
    sgraph::IScenegraph *originalScenegraph = nullptr;

    // speed
    float speed = 1.0f;

    // rotation anglees of drone
    float droneYaw = 0.0f;   
    float dronePitch = 0.0f; 

    // rolling for drone 
    float rollAngle = 0.0f;  
    bool isRolling = false;  
    float rollSpeed = 180.0f; 
    double lastUpdateTime = 0.0;
    

    //storing the drone transformations
    glm::mat4 droneTranslation;
    glm::mat4 droneXRotation = glm::mat4(1.0f);
    glm::mat4 droneYRotation = glm::mat4(1.0f);
    glm::mat4 droneZRotation = glm::mat4(1.0f);


    // fov angle
    float fieldOfView;

    // hash to store the nodes for efficiency 
    unordered_map<std::string, sgraph::TransformNode*> cachedNodes;

    // map for textures 
    map<std::string, unsigned int> textureMap;
    int ShaderMode = 0;

    // ray tracer renderer
    sgraph::RayScenegraphRenderer *rayTracerRenderer;
    
    // defult render: OpenGL
    RenderMode renderMode = OPENGL;  

    // Flag to check if raytracing is required
    bool raytraceRequested = false;  

    // Perform ray tracing and save output to an image
    void performRayTracing();  


};

#endif