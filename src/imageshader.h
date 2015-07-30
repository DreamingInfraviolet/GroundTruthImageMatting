#pragma once
#include "shaderprogram.h"

class QOpenGLFunctions;

/**
* A class that defines the basic shader used to draw images in OpenGL.
* */
class ImageShader : public ShaderProgram
{
private:

    //Iniform IDs.
    int mDiffuseID = -1;
    int mWindowSizeID = -1;
    int imageSizeID = -1;

    //Initialises the uniform IDs.
    virtual void prepare() override;

public:

    /**
    * Basic constructor.
    * param context The OpenGL Functions class indicating the desired context.
    */
    ImageShader(QOpenGLFunctions* context);

    /**
    * Sets the window size unform.
    * @param width The width of the window
    * @param height The height of the window
    * */
    void setWindowSize(int width, int height);

    /**
    * Sets the image size unform.
    * @param width The width of the image
    * @param height The height of the image
    * */
    void setImageSize(int width, int height);
};