#include "colourshader.h"

#include <QOpenGLFunctions>

void ColourShader::prepare()
{
	set();
	mContext->glBindAttribLocation(m_id, 0, "vertuv");
	mColourId = mContext->glGetUniformLocation(m_id, "colourin");
	mScaleId = mContext->glGetUniformLocation(m_id, "scale");
	mPosId = mContext->glGetUniformLocation(m_id, "pos");


}


void ColourShader::setColour(float r, float g, float b, float a)
{
	mContext->glUniform4f(mColourId, r,g,b,a);
}


ColourShader::ColourShader(QOpenGLFunctions* context)
	: ShaderProgram(context) {}


void ColourShader::setScale(float x, float y)
{
	mContext->glUniform2f(mScaleId, x, y);
}

void ColourShader::setPos(float x, float y)
{
	mContext->glUniform2f(mPosId, x, y);
}