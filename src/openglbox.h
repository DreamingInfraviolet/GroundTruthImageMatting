#pragma once
#include <qopenglwidget.h>
#include <QOpenGLFunctions>
#include <qtimer.h>
#include <future>
#include "vertex.h"

class QOpenGLTexture;
class ImageShader;

class OpenGlBox :public  QOpenGLWidget, public QOpenGLFunctions
{
	Q_OBJECT

private:
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

	static OpenGlBox* mInstance;
public:
	OpenGlBox(QWidget* parent);

	~OpenGlBox();

	static OpenGlBox* instance()
	{
		return mInstance;
	}

	void initializeGL() override;

	void paintGL() override;

	void resizeGL(int width, int height) override;

	void timerEvent(QTimerEvent *event);
};