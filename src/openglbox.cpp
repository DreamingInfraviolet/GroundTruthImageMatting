#include "openglbox.h"
#include "camera.h"
#include <qopengltexture.h>
#include "imageshader.h"
#include "colourshader.h"
#include "io.h"

OpenGlBox* OpenGlBox::mInstance = nullptr;
static unsigned zeroHist[256];

void OpenGlBox::initialiseQuads()
{
	//Screen quad
	mQuad.quad[0] = Vertex2D(-1.f, -1.f, 0.f, 0.f);
	mQuad.quad[1] = Vertex2D(1.f, -1.f, 1.f, 0.f);
	mQuad.quad[2] = Vertex2D(-1.f, 1.f, 0.f, 1.f);
	mQuad.quad[3] = Vertex2D(-1.f, 1.f, 0.f, 1.f);
	mQuad.quad[4] = Vertex2D(1.f, -1.f, 1.f, 0.f);
	mQuad.quad[5] = Vertex2D(1.f, 1.f, 1.f, 1.f);
	mQuad.VBO = ~0u;
	glGenBuffers(1, &mQuad.VBO);
	glBindBuffer(GL_ARRAY_BUFFER, mQuad.VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex2D) * 6, mQuad.quad, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), NULL);

	mHistQuad.quad[0] = Vertex2D(-1.f, -1.f, 0.f, 0.f);
	mHistQuad.quad[1] = Vertex2D(1.f, -1.f, 1.f, 0.f);
	mHistQuad.quad[2] = Vertex2D(-1.f, 1.f, 0.f, 1.f);
	mHistQuad.quad[3] = Vertex2D(-1.f, 1.f, 0.f, 1.f);
	mHistQuad.quad[4] = Vertex2D(1.f, -1.f, 1.f, 0.f);
	mHistQuad.quad[5] = Vertex2D(1.f, 1.f, 1.f, 1.f);

	mHistQuad.VBO = ~0u;
	glGenBuffers(1, &mHistQuad.VBO);
	glBindBuffer(GL_ARRAY_BUFFER, mHistQuad.VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex2D) * 6, mHistQuad.quad, GL_STATIC_DRAW);
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
		mWorkerReturnFuture = std::async(std::launch::async, [this]() ->WorkerReturn*
		{
			//Convert image
			if (this->mVideoImageData.size() == 0)
				return nullptr;

			QImage img;
			if (!img.loadFromData(&this->mVideoImageData[0], this->mVideoImageData.size(), "JPG"))
				return nullptr;

			WorkerReturn* out = new WorkerReturn();

			//Generate histogram
			//Create local copy to avoid pointer dereferencing
			unsigned int localHist[256];
			memset(localHist, 0, 256 * sizeof(unsigned));
			
			unsigned w = img.width();
			unsigned h = img.height();

			for (unsigned i = 0; i < w; ++i)
				for (unsigned j = 0; j < h; ++j)
				{
					QRgb px = img.pixel(i, j);
					unsigned r = (px & 0x00ff0000) >> 16;
					unsigned g = (px & 0x0000ff00) >> 8;
					unsigned b = (px & 0x000000ff);
					++localHist[(r + g + b) / 3];
				}

			out->image = std::move(img);
			memcpy(out->hist, localHist, 256*sizeof(unsigned));
			//


			return out;
		});
	}

	if (!thisState) //Retrieve processed jpg and update mVideoImage.
	{
		try
		{
			WorkerReturn* wr = mWorkerReturnFuture.get();
			if (!wr)
				return;
			else
			{
				delete mWorkerReturn;
				mWorkerReturn = wr;
			}

			if (mVideoTexture)
				delete mVideoTexture;

			mVideoTexture = new QOpenGLTexture(wr->image);
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
		mImageShader->setWindowSize(this->height(), this->width());
	}

	glDrawArrays(GL_TRIANGLES, 0, 6);

	//Draw histogram
	//Coordinates in range [-1,1]
	mHistShader->set();
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), NULL);

	float histWidth = 0.5f, histHeight = 0.3f;
	float histX = 0.02f, histY = 0.02f;

	//Draw box
	mHistShader->setColour(0.5f, 0.5f, 0.5f, 0.1f);
	mHistShader->setScale(histWidth, histHeight);
	mHistShader->setPos(histX, histY);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	//Draw lines
	float lineWidth = histWidth / 256.f;
	mHistShader->setColour(0.5f, 0.5f, 0.5f, 0.5);

	unsigned* hist;
	if (mWorkerReturn)
		hist = mWorkerReturn->hist;
	else
		hist = zeroHist;

	unsigned maxHist = 0;
	for (int i = 0; i < 256; ++i)
		if (hist[i]>maxHist)
			maxHist = hist[i];

	float maxHistf = maxHist / 256.f*histHeight;

	for (int i = 0; i < 256; ++i)
	{
		float lineHeight = hist[i] / 256.f * histHeight / maxHistf;
		if (lineHeight > histHeight)
			lineHeight = histHeight;
		mHistShader->setScale(lineWidth, lineHeight);
		mHistShader->setPos(histX + i * lineWidth, histY);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
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

	initialiseQuads();
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Inform("Creating Viewport shader");
	if (!mImageShader->load
		(
		//Vertex shader
		"#version 110\n"
		"attribute vec4 vertuv;"
		"uniform vec2 windowSize;"
		"uniform vec2 imageSize;"
		"varying vec2 uv;"

		"void main()"
		"{"
		"vec2 pos = vec2(vertuv.x, (vertuv.y*(imageSize.y/imageSize.x))*(windowSize.y/windowSize.x));"
		"gl_Position = vec4(pos,0,1);"
		"uv = vec2(vertuv.z, -vertuv.w);"
		"}",

		//Fragment shader
		"#version 130\n"
		"varying vec2 uv;"
		"uniform sampler2D diffuse;"

		"void main()"
		"{"
		"gl_FragColor = texture(diffuse, uv);"
		"}"
		))
	{
		close();
		return;
	}
	Inform("Done");

	Inform("Creating histogram shader");
	mHistShader = new ColourShader(this);
	if (!mHistShader->load(
		//Vertex shader
		"#version 110\n"
		"attribute vec4 vertuv;"
		"varying vec2 uv;"
		"uniform vec2 pos;"
		"uniform vec2 scale;"

		"void main()"
		"{"
		"vec2 newPos = (vec2(vertuv.x,vertuv.y)/2.0)+0.5;"
		"newPos = ((newPos*scale)+pos)*2.0 - 1.0;"
		"gl_Position = vec4(newPos,0,1);"
		"uv = vec2(vertuv.z, -vertuv.w);"
		"}",

		//Fragment shader
		"#version 130\n"
		"in vec2 uv;"
		"uniform vec4 colourin;"

		"void main()"
		"{"
		"gl_FragColor = colourin;"
		"}"))
	{
		close();
		return;
	}
	Inform("Done");

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
	delete mWorkerReturn;
}

OpenGlBox* OpenGlBox::instance()
{
	return mInstance;
}