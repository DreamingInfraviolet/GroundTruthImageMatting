#pragma once
#include "shaderprogram.h"

/**
* A class that defines the basic shader used to draw images in OpeNGL.
* */
class QOpenGLFunctions;

class ImageShader : public ShaderProgram
{
private:

	int mDiffuseID = -1;
	int mWindowSizeID = -1;
	int imageSizeID = -1;

	virtual void prepare() override;

public:

	ImageShader(QOpenGLFunctions* context);
	void setWindowSize(int width, int height);

	void setImageSize(int width, int height);
};