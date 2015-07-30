#include "shaderprogram.h"
#include <fstream>
#include <qopenglfunctions.h>
#include "io.h"

bool ShaderProgram::validate()
{
	GLint valstatus;
	mContext->glValidateProgram(m_id);
	mContext->glGetProgramiv(m_id, GL_VALIDATE_STATUS, &valstatus);
	if (!valstatus)
	{
		GLint length;
		mContext->glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &length);
		char* log = new char[length];
		mContext->glGetProgramInfoLog(m_id, length, NULL, log);
		Error(std::string("Error validating program:") + log);
		delete[] log;
		return false;
	}
	return true;
}
bool ShaderProgram::load(const std::string& vertstr, const std::string& fragstr)
{
	GLuint frag = mContext->glCreateShader(GL_FRAGMENT_SHADER);
	GLuint vert = mContext->glCreateShader(GL_VERTEX_SHADER);
	const char* fragPointer = fragstr.c_str();
	const char* vertPointer = vertstr.c_str();
	mContext->glShaderSource(frag, 1, &fragPointer, NULL);
	mContext->glShaderSource(vert, 1, &vertPointer, NULL);
	GLsizei logLength;
	GLint result;
	mContext->glCompileShader(frag);
	mContext->glGetShaderiv(frag, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		mContext->glGetShaderiv(frag, GL_INFO_LOG_LENGTH, &logLength);
		char* fragmentShaderErrorMessage = new char[logLength];
		mContext->glGetShaderInfoLog(frag, logLength, NULL, fragmentShaderErrorMessage);
		Error(std::string("Error loading fragment shader: ") + fragmentShaderErrorMessage);
		delete[] fragmentShaderErrorMessage;
		return false;
	}
	mContext->glCompileShader(vert);
	mContext->glGetShaderiv(vert, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		mContext->glGetShaderiv(vert, GL_INFO_LOG_LENGTH, &logLength);
		char* vertexShaderErrorMessage = new char[logLength];
		mContext->glGetShaderInfoLog(vert, logLength, NULL, vertexShaderErrorMessage);
		Error(std::string("Error loading vertex shader: ") + vertexShaderErrorMessage);
		delete[] vertexShaderErrorMessage;
		return false;
	}
	GLuint prog = mContext->glCreateProgram();
	mContext->glAttachShader(prog, frag);
	mContext->glAttachShader(prog, vert);
	mContext->glLinkProgram(prog);
	mContext->glDeleteShader(frag);
	mContext->glDeleteShader(vert);
	mContext->glGetProgramiv(prog, GL_LINK_STATUS, &result);
	if (!result)
	{
		mContext->glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLength);
		char* ProgramErrorMessage = new char[std::max(logLength, int(1))];
		mContext->glGetProgramInfoLog(prog, logLength, NULL, &ProgramErrorMessage[0]);
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
			mContext->glDetachShader(m_vert, m_id);
		if (m_frag)
			mContext->glDetachShader(m_frag, m_id);
		mContext->glDeleteProgram(m_id);
	}
}

void ShaderProgram::set()
{
	mContext->glUseProgram(m_id);
}

ShaderProgram::ShaderProgram(QOpenGLFunctions* context)
{
	mContext = context;
}