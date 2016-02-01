#pragma once

#ifdef RND_GL

#include <GL/glu.h>
#include <GL/gl.h>

#include <vector>
#include <string>

#include "RSingleton.h"

typedef GLubyte *(*__glGetStringi)(GLenum, GLuint);

namespace RAPI
{
    namespace RTools
    {
        class GLExtMgr : public RSingleton<GLExtMgr>
        {
            SINGLETONCLASS(GLExtMgr)

        public:

            bool initGLEM()
            {
                glGetIntegerv(GL_NUM_EXTENSIONS, reinterpret_cast<GLint *>(&this->m_numExtensions_));
                glGetIntegerv(GL_MAJOR_VERSION, reinterpret_cast<GLint *>(&this->m_glMajorVersion_));
                glGetIntegerv(GL_MINOR_VERSION, reinterpret_cast<GLint *>(&this->m_glMinorVersion_));

                if (!glGetString(GL_VERSION)[0])
                    return false;

                this->m_glVersion_ = std::string(reinterpret_cast<const char *>(glGetString(GL_VERSION)));
                this->m_glShadingVersion_ = std::string(
                        reinterpret_cast<const char *>(glGetString(GL_SHADING_LANGUAGE_VERSION)));

                if (this->m_glMajorVersion_ < 3)
                    return false;

                for (uint32_t i = 0; i <= (this->m_numExtensions_ - 1); i++)
                {
                    char *str = reinterpret_cast<char *>(glGetStringi(GL_EXTENSIONS, i));
                    if (str)
                        this->m_extensions_.push_back(str);
                }

                return true;
            };

            uint32_t getNumExtensions()
            {
                return m_numExtensions_;
            }

            uint32_t getGLMajorVersion()
            {
                return m_glMajorVersion_;
            }

            uint32_t getGLMinorVersion()
            {
                return m_glMinorVersion_;
            }

            std::string getGLVersionString()
            {
                return this->m_glVersion_;
            }

            std::string getGLShadingVersionString()
            {
                return this->m_glShadingVersion_;
            }

            // OPENGL FUNCTIONS

            GLubyte *glGetStringi(GLenum name, GLuint index)
            {
                typedef GLubyte *(__cdecl *__glGetStringi)(GLenum, GLuint);
                auto ptr = glGetFunctionAddress("glGetStringi");
                if (ptr)
                    return reinterpret_cast<__glGetStringi>(ptr)(name, index);
                else
                    return nullptr;
            }

        private:

            void *glGetFunctionAddress(const std::string &name)
            {
                void *p = reinterpret_cast<void *>(wglGetProcAddress(name.c_str()));
                if (p == 0 ||
                    (p == reinterpret_cast<void *>(0x1)) ||
                    (p == reinterpret_cast<void *>(0x2)) ||
                    (p == reinterpret_cast<void *>(0x3)) ||
                    (p == reinterpret_cast<void *>(-1)))
                {
                    HMODULE module = LoadLibraryA("opengl32.dll");
                    p = reinterpret_cast<void *>(GetProcAddress(module, name.c_str()));
                }

                return p;
            }

            uint32_t m_numExtensions_;
            uint32_t m_glMajorVersion_;
            uint32_t m_glMinorVersion_;
            std::string m_glVersion_;
            std::string m_glShadingVersion_;
            std::vector<std::string> m_extensions_;

        };
    }
}

#endif