#pragma once
#include <io.h>
#include <QGLWidget>
/**A virtual base class to give essential shader creation functionality to derived classes.*/
class ShaderProgram
{
private:
	GLuint m_vert = 0;
	GLuint m_frag = 0;
protected:
	GLuint m_id = 0;
	/**Performs shader-specific setup. Override, but don't call directly.*/
	virtual void prepare() {} //called from within load(...) function.
public:
	/**Returns the ID of the shader.*/
	GLint id() const { return m_id; }
	/**
	Initialises the shader from a source file.
	@param vertstr The source of the vertex shader.
	@param fragstr The source of the fragment shader.
	@return The success value.
	*/
	bool load(const std::string& vertstr, const std::string& fragstr);
	/**Checks whether the shader is working properly.*/
	bool validate();
	/**The virtual destructor to do basic cleanup.*/
	virtual ~ShaderProgram();
	/**Sets the shader as the current one in the rendering pipeline.*/
	void set();
};