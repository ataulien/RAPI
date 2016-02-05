#include "pch.h"
#include "RGLShader.h"
#include "Logger.h"
#include "RTools.h"

#ifdef RND_GL
using namespace RAPI;


RAPI::RGLShader::RGLShader()
{
	ShaderObject = 0;
}

RAPI::RGLShader::~RGLShader()
{
	glDeleteShader(ShaderObject);

	for(auto s : ProgramMap)
	{
		glDeleteProgram(s.second);
	}
}

void CheckShader(GLuint id, GLuint type, GLint *ret, const char *onfail)
{
	//Check if something is wrong with the shader
	switch(type) {
	case(GL_COMPILE_STATUS):
		glGetShaderiv(id, type, ret);
		if(*ret == false){
			int infologLength =  0;
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, &infologLength);
			GLchar* buffer = new GLchar[infologLength];
			GLsizei charsWritten = 0;
			LogInfo() << onfail << std::endl;
			glGetShaderInfoLog(id, infologLength, &charsWritten, buffer);
			LogInfo() << buffer << std::endl;

			delete[] buffer;
		}
		break;
	case(GL_LINK_STATUS):
		glGetProgramiv(id, type, ret);
		if(*ret == false){
			int infologLength =  0;
			glGetProgramiv(id, GL_INFO_LOG_LENGTH, &infologLength);
			GLchar* buffer = new GLchar[infologLength];
			GLsizei charsWritten = 0;
			LogInfo() << onfail << std::endl;
			glGetProgramInfoLog(id, infologLength, &charsWritten, buffer);
			LogInfo() << buffer << std::endl;

			delete[] buffer;
		}
		break;
	default:
		break;
	}
}


/**
* Loads the given shader
*/
bool RGLShader::CompileShaderAPI(EShaderType shaderType)
{
	GLuint s = glCreateShader(shaderType);
	CheckGlError();

	if(IsFromMemory)
	{
		ShaderSource = ShaderFile;
	}
	else
	{ 
		FILE* f = fopen(ShaderFile.c_str(), "rb");

		if(!f)
		{
			LogWarn() << "Failed to find shaderfile: " << ShaderFile;
			return false;
		}

		fseek(f, 0, SEEK_END);

		// Get filesize
		ShaderSource.resize(ftell(f));
		fseek(f, 0, SEEK_SET);

		// Read file
		fread(&ShaderSource[0], ShaderSource.size(), 1, f);

		fclose(f);
	}

	// Load the shadersource
	const char* src = ShaderSource.c_str();
	size_t n = 1;
	glShaderSource(s, 1, &src, nullptr);
	CheckGlError();

	// Compile...
	glCompileShader(s);
	CheckGlError();

	GLint ret;
	CheckShader(s, GL_COMPILE_STATUS, &ret, "unable to compile shader!");

	// TODO: Error logging and only replace old shader if new one successfully compiled!
	if(ShaderObject)
	{
		glDeleteShader(ShaderObject);
	}

	ShaderObject = s;

	return true;
}


/**
* Links a program from the shaders this is used with or takes one from cache if this already happened
*/
GLuint RGLShader::LinkShaderObjectAPI(const std::vector<RGLShader*>& shaders)
{
	// Hash the pointers to the shaders we got here
	size_t hash = RTools::HashVector(shaders);

	// Check if we already have that constelation
	auto it = ProgramMap.find(hash);
	if(it != ProgramMap.end())
		return (*it).second;

	// If not, link a new program
	GLuint program = glCreateProgram();
	CheckGlError();

	// Attach all of our shaders
	for(RGLShader* s : shaders)
	{
		glAttachShader(program, s->GetShaderObjectAPI());
	}

	// Now link everything together and add it to the map
	glLinkProgram(program);
	CheckGlError();

	GLint ret;
	CheckShader(program, GL_LINK_STATUS, &ret, "unable to link shader!");

	ProgramMap[hash] = program;

	return program;
}
#endif