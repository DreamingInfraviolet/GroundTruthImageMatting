#pragma once
#include <qopenglwidget.h>
#include <QOpenGLFunctions>
#include <qopengltexture.h>
#include <qtimer.h>
#include <future>
#include "vertex.h"

class OpenGlBox :public  QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT

private:

	friend class ShaderProgram;
	friend class ImageShader;

	ImageShader* mImageShader = nullptr;
	QOpenGLTexture* mVideoTexture = nullptr;
	std::future<QImage*> mVideoImage; //Filled by worker thread
	std::vector<unsigned char> mVideoImageData; //Used by worker thread
	QBasicTimer mBasicTimer;

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

	void timerEvent(QTimerEvent *event);
};

extern OpenGlBox* gOpenGlBox;