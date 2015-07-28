#include "openglbox.h"
#include "imageshader.h"
#include "camera.h"

OpenGlBox* gOpenGlBox = nullptr;

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

	updateVideoTexture();
	if (mVideoTexture)
		mVideoTexture->bind();

	//
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

OpenGlBox::OpenGlBox(QWidget* parent) : QOpenGLWidget(parent)
{
	gOpenGlBox = this;
}

void OpenGlBox::resizeGL(int width, int height)
{
	glViewport(0, 0, width, height);
}

void OpenGlBox::initializeGL()
{
	Inform("Initialising OpenGL.");

	initializeOpenGLFunctions();

	mImageShader = new ImageShader();

	initialiseQuad();
	glClearColor(1, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (!mImageShader->load
		(
		//Vertex shader
		"#version 130\n"
		"in vec4 vertuv;\n"
		"out vec2 uv;\n"
		"void main()\n"
		"{\n"
		"gl_Position = vec4(vertuv.x, vertuv.y, 0, 1);\n"
		"uv = vec2(vertuv.z, vertuv.w);\n"
		"}\n",

		//Fragment shader
		"#version 130\n"
		"out vec4 colour;\n"
		"in vec2 uv;\n"
		"uniform sampler2D diffuse;\n"
		"void main()\n"
		"{\n"
		"colour = texture(diffuse, uv);\n"
		"}\n"
		))
	{
		close();
		return;
	}
}

OpenGlBox::~OpenGlBox()
{
	delete mImageShader;
	delete mVideoTexture;
}

void OpenGlBox::updateVideoTexture()
{
	CameraList* cl = CameraList::instance();
	if (cl == nullptr)
		return;

	Camera* camera = cl->activeCamera();
	if (camera == nullptr)
		return;

	std::vector<unsigned char> imageData = camera->getLiveImage();

	if (imageData.size() == 0)
		return;

	QImage img;
	img.loadFromData(&imageData[0], imageData.size(), "JPG");
	delete mVideoTexture;
	mVideoTexture = new QOpenGLTexture(img);
}