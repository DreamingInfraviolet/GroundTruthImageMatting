#pragma once
#include "shaderprogram.h"
#include <qimage.h>
#include "openglbox.h"

/**
* A class that defines the basic shader used to draw images in OpeNGL.
* */

class ImageShader : public ShaderProgram
{
private:

	int mDiffuseID = -1;
	int mWindowSizeID = -1;
	int imageSizeID = -1;

	virtual void prepare() override
	{
		set();
		gOpenGlBox->glBindAttribLocation(m_id, 0, "vertuv");
		mDiffuseID = gOpenGlBox->glGetUniformLocation(m_id, "diffuse");
		mWindowSizeID = gOpenGlBox->glGetUniformLocation(m_id, "windowSize");
		imageSizeID = gOpenGlBox->glGetUniformLocation(m_id, "imageSize");
		gOpenGlBox->glUniform1i(mDiffuseID, 0);
	}

public:

	void setWindowSize(int width, int height)
	{
		gOpenGlBox->glUniform2f(mWindowSizeID, float(width), float(height));
	}

	void setImageSize(int width, int height)
	{
		gOpenGlBox->glUniform2f(imageSizeID, float(width), float(height));
	}
};