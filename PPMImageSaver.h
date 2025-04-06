#ifndef __PPMIMAGESAVER_H_
#define __PPMIMAGESAVER_H_

#include "ImageSaver.h"
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <fstream>
#include <glad/glad.h>
#include <string>

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

           
            file << "P3\n";  
            file << width << " " << height << "\n";  
            file << "255\n"; 
  

            int pixelCount = width * height;
            for (int i = 0; i < pixelCount; i++) {
                int idx = i * 3;
                // Convert each channel to int so it prints nicely
                int r = static_cast<int>(image[idx + 0]);
                int g = static_cast<int>(image[idx + 1]);
                int b = static_cast<int>(image[idx + 2]);

                // Each pixel on its own line: "r g b"
                file << r << " " << g << " " << b << "\n";
            }

            file.close();
            std::cout << "Image saved to " << filename << std::endl;
        }

};

#endif