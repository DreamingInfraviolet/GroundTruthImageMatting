#pragma once
#include "shaderprogram.h"

class QOpenGLFunctions;

/**
* A class that defines the basic shader used to draw images in OpenGL.
* */
class ColourShader : public ShaderProgram
{
private:

    //Iniform IDs.
    int mColourId = -1;
	int mScaleId = -1;
	int mPosId = -1;
    //Initialises the uniform IDs.
    virtual void prepare() override;

public:

    /**
    * Basic constructor.
    * param context The OpenGL Functions class indicating the desired context.
    */
    ColourShader(QOpenGLFunctions* context);

    /**
    * Sets the colour uniform.
    * */
    void setColour(float r, float g, float b, float a);

	void setScale(float x, float y);

	void setPos(float x, float y);
};