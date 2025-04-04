#ifndef __IMAGESAVER_H_
#define __IMAGESAVER_H_

#include <string>
#include <glad/glad.h>

/**
 * @brief This abstract class represents an image loader. There would be 
 * one subclass for each file format
 * 
 */
class ImageSaver {
    public:
        ImageSaver(){}
        virtual void save(string filename,GLubyte *image,int width , int height) = 0;

};

#endif