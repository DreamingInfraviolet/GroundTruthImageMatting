#pragma once
#include <qopenglwidget.h>
#include <QOpenGLFunctions>
#include <qopengltexture.h>
#include "vertex.h"

class OpenGlBox :public  QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT

private:

	friend class ShaderProgram;
	friend class ImageShader;

	ImageShader* mImageShader = nullptr;
	QOpenGLTexture* mVideoTexture = nullptr;

	struct
	{
		Vertex2D quad[6];
		GLuint VBO;
	} mQuad;

	void initialiseQuad();

public:
	OpenGlBox(QWidget* parent);

	~OpenGlBox();

	void initializeGL();

	void paintGL() override;

	void resizeGL(int width, int height) override;

	void updateVideoTexture();

};

extern OpenGlBox* gOpenGlBox;