#include "shaderprogram.h"
#include "openglbox.h"
#include <QGLWidget>
#include <fstream>
bool ShaderProgram::validate()
{
	GLint valstatus;
	gOpenGlBox->glValidateProgram(m_id);
	gOpenGlBox->glGetProgramiv(m_id, GL_VALIDATE_STATUS, &valstatus);
	if (!valstatus)
	{
		GLint length;
		gOpenGlBox->glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &length);
		char* log = new char[length];
		gOpenGlBox->glGetProgramInfoLog(m_id, length, NULL, log);
		Error(std::string("Error validating program:") + log);
		delete[] log;
		return false;
	}
	return true;
}
bool ShaderProgram::load(const std::string& vertstr, const std::string& fragstr)
{
	GLuint frag = gOpenGlBox->glCreateShader(GL_FRAGMENT_SHADER);
	GLuint vert = gOpenGlBox->glCreateShader(GL_VERTEX_SHADER);
	const char* fragPointer = fragstr.c_str();
	const char* vertPointer = vertstr.c_str();
	gOpenGlBox->glShaderSource(frag, 1, &fragPointer, NULL);
	gOpenGlBox->glShaderSource(vert, 1, &vertPointer, NULL);
	GLsizei logLength;
	GLint result;
	gOpenGlBox->glCompileShader(frag);
	gOpenGlBox->glGetShaderiv(frag, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		gOpenGlBox->glGetShaderiv(frag, GL_INFO_LOG_LENGTH, &logLength);
		char* fragmentShaderErrorMessage = new char[logLength];
		gOpenGlBox->glGetShaderInfoLog(frag, logLength, NULL, fragmentShaderErrorMessage);
		Error(std::string("Error loading fragment shader: ") + fragmentShaderErrorMessage);
		delete[] fragmentShaderErrorMessage;
		return false;
	}
	gOpenGlBox->glCompileShader(vert);
	gOpenGlBox->glGetShaderiv(vert, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		gOpenGlBox->glGetShaderiv(vert, GL_INFO_LOG_LENGTH, &logLength);
		char* vertexShaderErrorMessage = new char[logLength];
		gOpenGlBox->glGetShaderInfoLog(vert, logLength, NULL, vertexShaderErrorMessage);
		Error(std::string("Error loading vertex shader: ") + vertexShaderErrorMessage);
		delete[] vertexShaderErrorMessage;
		return false;
	}
	GLuint prog = gOpenGlBox->glCreateProgram();
	gOpenGlBox->glAttachShader(prog, frag);
	gOpenGlBox->glAttachShader(prog, vert);
	gOpenGlBox->glLinkProgram(prog);
	gOpenGlBox->glDeleteShader(frag);
	gOpenGlBox->glDeleteShader(vert);
	gOpenGlBox->glGetProgramiv(prog, GL_LINK_STATUS, &result);
	if (!result)
	{
		gOpenGlBox->glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLength);
		char* ProgramErrorMessage = new char[std::max(logLength, int(1))];
		gOpenGlBox->glGetProgramInfoLog(prog, logLength, NULL, &ProgramErrorMessage[0]);
		Error(std::string("Linking error: ") + ProgramErrorMessage);
		delete[] ProgramErrorMessage;
		return false;
	}
	m_frag = frag;
	m_vert = vert;
	m_id = prog;
	prepare();
	if (!validate())
		return false;
	return true;
}
ShaderProgram::~ShaderProgram()
{
	if (m_id)
	{
		if (m_vert)
			gOpenGlBox->glDetachShader(m_vert, m_id);
		if (m_frag)
			gOpenGlBox->glDetachShader(m_frag, m_id);
		gOpenGlBox->glDeleteProgram(m_id);
	}
}
void ShaderProgram::set()
{
	gOpenGlBox->glUseProgram(m_id);
}