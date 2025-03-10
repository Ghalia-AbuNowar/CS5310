//! [code]

#include <glad/glad.h>
#include "View.h"
#include "Model.h"
#include "Controller.h"
#include <iostream>

int main(int argc,char *argv[]) {
    string filename;
    
    
    if (argc > 1 && argv[1] != nullptr) {
        filename = argv[1];
    } else {
        std::cerr << "Using default scene file." << std::endl;
        filename = "scenegraphmodels/face-hierarchy-commands.txt"; 
    }

    Model model;
    View view;
    Controller controller(model,view,filename);
    controller.run();
}

//! [code]
