#include "openglbox.h"
#include "camera.h"
#include <qopengltexture.h>
#include "imageshader.h"

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
	glClear(GL_COLOR_BUFFER_BIT);
	mImageShader->set();
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), NULL);
	//

	//Live stream:

	//If state is true, get the image from the camera and launch thread to process it.
	//If it is false, retrieve the image and upload it.
	static bool state = true;
	bool error = false;

	//Using a loop to allow breaking
	for (int i = 0; i < int(state); ++i)
	{
		//Get jpg
		CameraList* cl = CameraList::instance();
		if (cl == nullptr)
			break;

		Camera* camera = cl->activeCamera();
		if (camera == nullptr)
			break;

		mVideoImageData.swap(camera->getLiveImage());

		//Start processing on different thread
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
	if(!state) //Retrieve processed jpg and upload it
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

	state = !state;

	if (mVideoTexture)
		mVideoTexture->bind();

	if (mVideoTexture)
		mImageShader->setImageSize(mVideoTexture->height(), mVideoTexture->width());

	//
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
		"float windowAspect = windowSize.x/windowSize.y;"
		"vec2 pos;"

		"pos = vec2(vertuv.x, vertuv.y*(imageSize.x/imageSize.y));"

		"{"
		"pos = vec2(pos.x, pos.y*windowAspect);"
		"}"



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
	mBasicTimer.start(50, this);
}

void OpenGlBox::timerEvent(QTimerEvent *event)
{
	update();
}

OpenGlBox::~OpenGlBox()
{
	delete mImageShader;
	delete mVideoTexture;
}