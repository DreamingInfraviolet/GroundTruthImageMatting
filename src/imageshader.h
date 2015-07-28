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

	int mDiffuseID=-1;

	virtual void prepare() override
	{
		set();
		gOpenGlBox->glBindAttribLocation(m_id, 0, "vertuv");
		mDiffuseID = gOpenGlBox->glGetUniformLocation(m_id, "diffuse");
		gOpenGlBox->glUniform1i(mDiffuseID, 0);
	}

public:
};