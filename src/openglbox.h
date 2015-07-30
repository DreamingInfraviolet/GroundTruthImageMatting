#pragma once
#include <qopenglwidget.h>
#include <QOpenGLFunctions>
#include <qtimer.h>
#include <future>
#include "vertex.h"

class QOpenGLTexture;
class ImageShader;

/**
* This is the class that takes care of drawing the live view of the camera on the QT window.
* The first frame retrieves the image and processes it on a thread, and the second frame uploads and displays it.
* Define OPENGL_BOX_TICK to be the desired interval at which a frame is to be submitted in milliseconds.
* */

#ifndef OPENGL_BOX_TICK
#define OPENGL_BOX_TICK 50 // ~ 20 fps
#endif

class OpenGlBox :public  QOpenGLWidget, public QOpenGLFunctions
{
    Q_OBJECT

private:

    //The image shader to be used.
    ImageShader* mImageShader = nullptr;

    //Stores the video texture between frames.
    QOpenGLTexture* mVideoTexture = nullptr;

    //Filled by worker thread
    std::future<QImage*> mVideoImage; 

    //Used by worker thread
    std::vector<unsigned char> mVideoImageData;

    //Handles the frame timing
    QBasicTimer mBasicTimer;

    //The screen quad
    struct
    {
        Vertex2D quad[6];
        GLuint VBO;
    } mQuad;

    /** Initialises the screen quad to fill the screen. */
    void initialiseQuad();

    //The current instance of the class.
    static OpenGlBox* mInstance;
public:

    /** Trivial constructor registering the class. */
    OpenGlBox(QWidget* parent);

    /** Deregisters the class and frees resources. */
    ~OpenGlBox();

    /** Returns the current instance of the class. */
    static OpenGlBox* instance();

    /** Initialises the GL state. */
    void initializeGL() override;

    /** 
    * Every even frame downloads an image (if available) and launches a thread to process it.
    * Every odd frame uploads the image from the previous frame and displays it.
    * */
    void paintGL() override;

    /** Calls glViewport and updates the shader uniforms. */
    void resizeGL(int width, int height) override;

    /** Called on every timer tick: updates the frame. */
    void timerEvent(QTimerEvent *event);
};