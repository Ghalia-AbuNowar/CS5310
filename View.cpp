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
#include "sgraph/RayScenegraphRenderer.h"

#include "PPMImageSaver.h"

#include "Light.h"
#include <cmath>
#include <algorithm>
#include <unordered_map>


View::View():fieldOfView(60.0f){
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

    window = glfwCreateWindow(800, 800, "Assignment 6", NULL, NULL);
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

    rayTracerRenderer = new sgraph::RayScenegraphRenderer(modelview);
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

void View::initLights(sgraph::IScenegraph *scenegraph){
    lightRenderer->getCollectedLights().clear();
    scenegraph->getRoot()->accept(lightRenderer);
    lights = lightRenderer->getCollectedLights();
    lightNodeNames = lightRenderer->getLightNodeNames();
    collectedTransformations = lightRenderer->getCollectedTransformations();
}


void View::display(sgraph::IScenegraph *scenegraph) {

    if (renderMode == RAYTRACE && raytraceRequested) {
        // Perform ray tracing
        performRayTracing(scenegraph);
        // Ray tracing once
        raytraceRequested = false;  
        toggleRenderMode();
    }
     

    // ------------------------------------------- //
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


    // collect the lights and clear any previous lights
    
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
        //cout<<"Pos : "<<pos.x<< " " << pos.y<<" "<<pos.z<< " "<<pos.w<< endl;
        pos = modelview.top() * collectedTransformations[i] * pos;
        direction = modelview.top() * collectedTransformations[i] * direction;
        //cout<<"Pos : "<<pos.x<< " " << pos.y<<" "<<pos.z<< " "<<pos.w<< endl;
        //cout<<"Spot Direction : "<<lights[i].getSpotDirection().x<< " " << lights[i].getSpotDirection().y<<" "<<lights[i].getSpotDirection().z<< " "<<lights[i].getSpotDirection().w<< endl;
        //cout<<"Angle : "<<lights[i].getSpotCutoff()<< endl;
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

    
    // get the time so that you can use it to make a roll
    double currentTime = glfwGetTime();
    double deltaTime = currentTime - lastUpdateTime;
    lastUpdateTime = currentTime;



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
used to toggle ray tracer
*/
void View::toggleRenderMode() {
    if (renderMode == OPENGL) {
        // Switch to ray tracing mode
        renderMode = RAYTRACE;  
        // Indicate that ray tracing should happen
        raytraceRequested = true;  
        std::cout << "Switched to Ray Tracing Mode \n";
    } else {
        // Switch back to OpenGL mode
        renderMode = OPENGL;  
        // No need to raytrace
        raytraceRequested = false;  
        std::cout << "Switched to OpenGL Mode\n";
    }
}

Ray View::generateCameraRay(int x, int y, int width, int height) 
{
    // // used notes to do this
    // float vx = (-width/2) + x;
    // float vy = (-height/2) + y;
    // float vz = (-0.5 * height)/tan(glm::radians(0.5 * fieldOfView));

    // glm::vec3 origin(0.0f, 0.0f, 100.0f);       // camera away from origin
    // glm::vec3 direction = glm::vec3(vx, vy, vz); 
    // return Ray(origin, direction);

    // coordinates for the camera 1
    glm::vec3 cameraPos(200.0f, 250.0f, 250.0f);
    glm::vec3 target(0.0f, 0.0f, 0.0f);
    glm::vec3 up(0.0f, 1.0f, 0.0f);

    // Calculate view matrix using lookAT
    glm::mat4 viewMatrix = glm::lookAt(cameraPos, target, up);
    
    // Calculate ray direction in camera space
    float vx = (-width/2) + x;
    float vy = (-height/2) + y;
    float vz = (-0.5 * height)/tan(glm::radians(0.5 * fieldOfView));
    
    // normalized direction vector in camera space
    glm::vec3 directionCameraSpace = glm::normalize(glm::vec3(vx, vy, vz));
    
    // Transform to world space by inversing matrix
    glm::mat4 invView = glm::inverse(viewMatrix);
    glm::vec4 directionWorldSpace4 = invView * glm::vec4(directionCameraSpace, 0.0f);
    glm::vec3 direction = glm::normalize(glm::vec3(directionWorldSpace4));
    
    // camera origin is origin of ray
    glm::vec3 origin = cameraPos;
    
    return Ray(origin, direction);
}


void View::performRayTracing(sgraph::IScenegraph *scenegraph)
{
    // Get the current framebuffer size
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    // 2D image buffer
   vector<std::vector<glm::vec3>> image(height, std::vector<glm::vec3>(width, glm::vec3(0.0f)));

    // identity
    modelview.push(glm::mat4(1.0f));

    // Loop over each pixel
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            

            // Generate ray for pixel (x,y)
            Ray camRay = generateCameraRay(x, y, width, height);

            // Prepare a hit record for the intersection
            HitRecord hr;
            hr.t = std::numeric_limits<float>::infinity();

            // Provide the ray and hit record to RayScenegraphRenderer
            rayTracerRenderer->setRayAndHit(&camRay, &hr);

            // traverse the scene graph with the ray
            scenegraph->getRoot()->accept(rayTracerRenderer);

            // Check if hit
            if (hr.t < std::numeric_limits<float>::infinity()) {
                // image[y][x] = glm::vec3(1,1,1);
                glm::vec3 point = hr.point;
                glm::vec3 normal = hr.normal;
                util::Material objMaterial = hr.objMaterial;
                image[y][x] = glm::vec3(shade(scenegraph,lights,point,normal,objMaterial,x,y,width,height));
            }
            else{
                image[y][x] = glm::vec3(0,0,0);
            }
        
        }
        if (y % 10 == 0 || y == height - 1) {
            float progress = (float)(y + 1) / height * 100.0f;
            std::cout << "\rRay tracing progress: " << (int)progress << "%" << std::flush;
        }
        
    }

    modelview.pop(); 

    //  Convert the glm::vec3  to GLubyte buffer
    std::vector<GLubyte> buffer(width * height * 3);
    {
        int index = 0;
        for (int row = height -1; row >= 0; --row) {
            for (int col = 0; col < width; ++col) {
                glm::vec3 color = image[row][col];

                // Map from [0..1] float to [0..255] byte
                GLubyte r = static_cast<GLubyte>(glm::clamp(color.r * 255.0f, 0.0f, 255.0f));
                GLubyte g = static_cast<GLubyte>(glm::clamp(color.g * 255.0f, 0.0f, 255.0f));
                GLubyte b = static_cast<GLubyte>(glm::clamp(color.b * 255.0f, 0.0f, 255.0f));

                buffer[index++] = r;
                buffer[index++] = g;
                buffer[index++] = b;
            }
        }
    }

    // save image
    PPMImageSaver saver;
    saver.save("raytrace_output.ppm", buffer.data(), width, height);

    std::cout << "Ray tracing done! Saved to raytrace_output.ppm\n";
}

glm::vec4 View::shade(sgraph::IScenegraph *scenegraph,vector<util::Light> lights, glm::vec3 position,glm::vec3 normal, util::Material objMaterial,int x, int y,int width,int height){
   


    int numOfLights = lights.size();
    vector<bool> LightsTrue(numOfLights, false);

    for(int i=0;i<numOfLights;i++){
        Ray Shadowray;
        Shadowray.origin = position;
        Shadowray.direction = glm::vec3(lights[i].getPosition()) - position;
        Shadowray.origin = Shadowray.origin + 0.01f*Shadowray.direction;
        sgraph::RayScenegraphRenderer *shadowrenderer = new sgraph::RayScenegraphRenderer(modelview);
        HitRecord shadowHT;
        shadowHT.t = std::numeric_limits<float>::infinity();
        shadowrenderer->setRayAndHit(&Shadowray,&shadowHT);
        scenegraph->getRoot()->accept(shadowrenderer);
        if(shadowHT.t<std::numeric_limits<float>::infinity()){
            LightsTrue[i] = true;
        }
        else {
            LightsTrue[i] = false; 
        }
    }


    glm::vec4 fColor = glm::vec4(0,0,0,1);
    glm::vec3 lightVec,viewVec,reflectVec;
    glm::vec3 normalView;
    glm::vec3 ambient,diffuse,specular;
    float nDotL,rDotV;
    for(int i=0;i<numOfLights;i++){
        if(LightsTrue[i] == false){
        if (lights[i].getPosition().w!=0)
            lightVec = normalize(glm::vec3(lights[i].getPosition()) - position);
        else
            lightVec = normalize(-lights[i].getPosition());

        // Determine if light is a spotlight
        bool isSpot = lights[i].getSpotCutoff()> 0.0;

        // Spotlight condition
        bool insideCone = true;
        if (isSpot){
            float theta = dot(normalize(-lightVec), normalize(glm::vec3(lights[i].getSpotDirection())));
            insideCone = theta > cos(glm::radians(lights[i].getSpotCutoff())); // SHOULD THIS BE IN RADIANS
        }

        normalView = normalize(normal);
        nDotL = dot(normalView,lightVec);

        viewVec = -position;
        viewVec = normalize(viewVec);

        reflectVec = reflect(-lightVec,normalView);
        reflectVec = normalize(reflectVec);

        rDotV = std::max(dot(reflectVec,viewVec),0.0f);

        ambient = glm::vec3(objMaterial.getAmbient()) * lights[i].getAmbient();
        diffuse = glm::vec3(objMaterial.getDiffuse()) * lights[i].getDiffuse() * std::max(nDotL,0.0f);
        if (nDotL>0)
            specular = glm::vec3(objMaterial.getSpecular()) * lights[i].getSpecular() * std::pow(rDotV,objMaterial.getShininess());
        else
            specular = glm::vec3(0,0,0);

        if (isSpot) {
            if (insideCone){
                fColor = fColor + glm::vec4(ambient + diffuse, 1.0);
            } 

        } 
        
        if (!isSpot) {
           fColor = fColor + glm::vec4(ambient + diffuse + specular, 1.0);
       }
    }
    }
    return fColor;
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







