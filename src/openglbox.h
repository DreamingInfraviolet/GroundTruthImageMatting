#pragma once
#include <qopenglwidget.h>
#include <QOpenGLFunctions>

class OpenGlBox :public  QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT
public:
	OpenGlBox(QWidget* parent) : QOpenGLWidget(parent) {}

	void initializeGL()
		override
	{
		initializeOpenGLFunctions();
		glClearColor(1, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void paintGL() override{ glClear(GL_COLOR_BUFFER_BIT); }
	void resizeGL(int width, int height) override
	{}


};