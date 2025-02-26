#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include "View.h"
#include "Model.h"
#include "Callbacks.h"

class Controller: public Callbacks
{
public:
    Controller(Model& m,View& v,string filename);
    ~Controller();
    void run();
    virtual void reshape(int width, int height);
    virtual void dispose();
    virtual void onkey(int key, int scancode, int action, int mods);
    virtual void error_callback(int error, const char* description);
    virtual void mouseCallBack(int button,int action,int mods);
    virtual void onScroll(double xpos,double ypos);
private:
    void initScenegraph(string filename);
    void updateCameraPosition();
    View view;
    Model model;
    glm::vec3 cameraPos;
    glm::vec3 initialCameraPos;
    float initialTheta;
    float initialPhi;
    glm::vec3 target;
    glm::vec3 up;
    
    float radius, theta, phi;
    bool rotating;
    double lastX, lastY;
};

#endif