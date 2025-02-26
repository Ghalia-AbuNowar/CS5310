//! [code]

#include <glad/glad.h>
#include "View.h"
#include "Model.h"
#include "Controller.h"
#include <iostream>

int main(int argc,char *argv[]) {
    string filename = argv[1];
    Model model;
    View view;
    Controller controller(model,view,filename);
    controller.run();
}

//! [code]
