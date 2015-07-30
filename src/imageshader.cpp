#include "imageshader.h"
#include <QOpenGLFunctions>

void ImageShader::prepare()
{
	set();
	mContext->glBindAttribLocation(m_id, 0, "vertuv");
	mDiffuseID = mContext->glGetUniformLocation(m_id, "diffuse");
	mWindowSizeID = mContext->glGetUniformLocation(m_id, "windowSize");
	imageSizeID = mContext->glGetUniformLocation(m_id, "imageSize");
	mContext->glUniform1i(mDiffuseID, 0);
}


void ImageShader::setWindowSize(int width, int height)
{
	mContext->glUniform2f(mWindowSizeID, float(width), float(height));
}

void ImageShader::setImageSize(int width, int height)
{
	mContext->glUniform2f(imageSizeID, float(width), float(height));
}

ImageShader::ImageShader(QOpenGLFunctions* context)
	: ShaderProgram(context) {}