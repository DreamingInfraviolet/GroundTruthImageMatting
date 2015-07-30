#include "openglbox.h"
#include "camera.h"
#include <qopengltexture.h>
#include "imageshader.h"
#include "io.h"

OpenGlBox* OpenGlBox::mInstance = nullptr;

void OpenGlBox::initialiseQuad()
{
	mQuad.quad[0] = Vertex2D(-1.f, -1.f, 0.f, 0.f);
	mQuad.quad[1] = Vertex2D(1.f, -1.f, 1.f, 0.f);
	mQuad.quad[2] = Vertex2D(-1.f, 1.f, 0.f, 1.f);
	mQuad.quad[3] = Vertex2D(-1.f, 1.f, 0.f, 1.f),
	mQuad.quad[4] = Vertex2D(1.f, -1.f, 1.f, 0.f);
	mQuad.quad[5] = Vertex2D(1.f, 1.f, 1.f, 1.f);
	mQuad.VBO = ~0u;
	glGenBuffers(1, &mQuad.VBO);
	glBindBuffer(GL_ARRAY_BUFFER, mQuad.VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex2D) * 6, mQuad.quad, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), NULL);
}

void OpenGlBox::paintGL()
{
	//Prepare
	glClear(GL_COLOR_BUFFER_BIT);
	mImageShader->set();
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), NULL);

	//Live stream

	//If state is true, get the image from the camera and launch thread to process it.
	//If it is false, retrieve the image and upload it.
	//An intermediary variable, thisState, is used in case of early return. 
	static bool state = true;
	int thisState = state;
	state = !state;

	if (thisState)
	{
		//Check camera
		CameraList* cl = CameraList::instance();
		if (cl == nullptr)
			return;

		Camera* camera = cl->activeCamera();
		if (camera == nullptr)
			return;

		//Get jpg
		mVideoImageData.swap(camera->getLiveImage());

		//Launch processing thread
		mVideoImage = std::async(std::launch::async, [this]() ->QImage*
		{
			if (this->mVideoImageData.size() == 0)
				return nullptr;

			QImage* img = new QImage();
			if (!img->loadFromData(&this->mVideoImageData[0], this->mVideoImageData.size(), "JPG"))
			{
				delete img;
				return nullptr;
			}
			return img;
		});
	}
	if(!thisState) //Retrieve processed jpg and update mVideoImage.
	{
		try
		{
			QImage* image = mVideoImage.get();
			if (!image)
				return;

			if (mVideoTexture)
				delete mVideoTexture;

			mVideoTexture = new QOpenGLTexture(*image);
			delete image;
		}
		catch (const std::future_error&)
		{
			//Likely called if the camera is not yet ready. Patience!
			return;
		}

	}

	//If there is a valid image object, show it.
	if (mVideoTexture)
	{
		mVideoTexture->bind();
		mImageShader->setImageSize(mVideoTexture->width(), mVideoTexture->height());
	}

	glDrawArrays(GL_TRIANGLES, 0, 6);
}

OpenGlBox::OpenGlBox(QWidget* parent) : QOpenGLWidget(parent)
{
	mInstance = this;
}

void OpenGlBox::resizeGL(int width, int height)
{
	glViewport(0, 0, width, height);
	mImageShader->setWindowSize(width, height);
}

void OpenGlBox::initializeGL()
{
	Inform("Initialising OpenGL.");

	initializeOpenGLFunctions();

	mImageShader = new ImageShader(this);

	initialiseQuad();
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (!mImageShader->load
		(
		//Vertex shader
		"#version 130\n"
		"in vec4 vertuv;"
		"uniform vec2 windowSize;"
		"uniform vec2 imageSize;"
		"out vec2 uv;"

		"void main()"
		"{"
			"vec2 pos = vec2(vertuv.x, (vertuv.y*(imageSize.y/imageSize.x))*(windowSize.x/windowSize.y));"
			"gl_Position = vec4(pos,0,1);"
			"uv = vec2(vertuv.z, -vertuv.w);"
		"}",

		//Fragment shader
		"#version 130\n"
		"out vec4 colour;"
		"in vec2 uv;"
		"uniform sampler2D diffuse;"

		"void main()"
		"{"
			"colour = texture(diffuse, uv);"
		"}"
		))
	{
		close();
		return;
	}

	//Set fps (update every n milliseconds)
	mBasicTimer.start(OPENGL_BOX_TICK, this);

	Inform("OpenGL ready");
}

void OpenGlBox::timerEvent(QTimerEvent *event)
{
	update();
}

OpenGlBox::~OpenGlBox()
{
	mInstance = nullptr;
	delete mImageShader;
	delete mVideoTexture;
}

OpenGlBox* OpenGlBox::instance()
{
	return mInstance;
}