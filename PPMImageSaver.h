#ifndef __PPMIMAGESAVER_H_
#define __PPMIMAGESAVER_H_

#include "ImageSaver.h"
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <fstream>

/**
 * @brief This class is used to load an image in the ASCII PPM format 
 * 
 */
class PPMImageSaver: public ImageSaver {
    
    public:
        PPMImageSaver(){}

        void save(string filename,GLubyte *image,int width,int height){
            std::ofstream file(filename, std::ios::binary);
            
            if (!file) {
                std::cerr << "Failed to open file for saving: " << filename << std::endl;
                return;
            }

           
            file << "P6\n";  
            file << width << " " << height << "\n";  
            file << "255\n"; 
            for (int i = 0; i < width * height * 3; ++i) {
                file.put(image[i]); 
            }

            file.close();
            std::cout << "Image saved to " << filename << std::endl;
        }

};

#endif