#pragma once
#include "RBaseShader.h"

#ifdef RND_GL
namespace RAPI
{
	class RGLShader : public RBaseShader
	{
	public:
		RGLShader();
		~RGLShader();

		/**
		* Loads the given shader
		*/
		bool CompileShaderAPI(EShaderType shaderType);

		/**
		 * Links a program from the shaders this is used with or takes one from cache if this already happened
		 */
		GLuint LinkShaderObjectAPI(RGLShader** shaders, size_t numShaders);

		/**
		 * Returns the unlinked shaderobject of this 
		 */
		GLuint GetShaderObjectAPI(){return ShaderObject;}
	protected:
		// Unlinked Shader-object
		GLuint ShaderObject;

		// Source which created the shader
		std::string ShaderSource;

		// Map of shader programs this is used with
		std::unordered_map<size_t, GLuint> ProgramMap;
	};
}
#endif
