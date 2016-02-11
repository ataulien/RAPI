#pragma once
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <functional>
#include <stdio.h>

#define USE_LOG

#ifndef WIN32
#define FAILED(x) (x != 0)
#else
#include <windows.h>
#endif

#ifdef _MSC_VER
#define FUNCTION_SIGNATURE __FUNCSIG__
#elif defined(__GNUC__)
#define FUNCTION_SIGNATURE __PRETTY_FUNCTION__
#endif



#define LEB(x) {if(!x){LogError() << #x " failed!";}}
#define LEB_R(x) {if(!x){LogError() << #x " failed!"; return false;}}

/** Checks for errors and logs them, HRESULT hr needs to be declared */
#define LE(x) { hr = (x); if(FAILED(hr)){LogError() << #x " failed with code: " << hr << "!"; }/*else{ LogInfo() << L#x << L" Succeeded."; }*/ }

/** Returns hr if failed (HRESULT-function, hr needs to be declared)*/
#define LE_R(x) { hr = (x); if(FAILED(hr)){LogError() << #x " failed with code: " << hr << "!"; return hr;} }

/** Returns nothing if failed (void-function)*/
#define LE_RV(x) { hr = (x); if(FAILED(hr)){LogError() << #x " failed with code: " << hr << "!"; return;} }

/** Returns false if failed (bool-function) */
#define LE_RB(x) { hr = (x); if(FAILED(hr)){LogError() << #x " failed with code: " << hr << "!"; return false;} }

#ifdef WIN32
#define ErrorBox(Msg) MessageBoxA(NULL,Msg,"Error!",MB_OK|MB_ICONERROR|MB_TOPMOST)
#define InfoBox(Msg) MessageBoxA(NULL,Msg,"Info!",MB_OK|MB_ICONASTERISK|MB_TOPMOST)
#define WarnBox(Msg) MessageBoxA(NULL,Msg,"Warning!",MB_OK|MB_ICONEXCLAMATION|MB_TOPMOST)
#else
#define ErrorBox(Msg) /** TODO **/
#define InfoBox(Msg)
#define WarnBox(Msg)
#endif

/** Logging macros 
	Usage: LogInfo() << L"Loaded Texture: " << TextureName; 
	*/


#define LogInfo() Log("Info",__FILE__, __LINE__, FUNCTION_SIGNATURE)
#define LogWarn() Log("Warning",__FILE__, __LINE__, FUNCTION_SIGNATURE, true)
#define LogError() Log("Error",__FILE__, __LINE__, FUNCTION_SIGNATURE, true)


/** Displays a messagebox and loggs its content */
#define LogInfoBox() Log("Info",__FILE__, __LINE__, FUNCTION_SIGNATURE, false, 1)
#define LogWarnBox() Log("Warning",__FILE__, __LINE__, FUNCTION_SIGNATURE, true, 2)
#define LogErrorBox() Log("Error",__FILE__, __LINE__, FUNCTION_SIGNATURE, true, 3)

#define STRINGX(x) #x
#define STRING(x) STRINGX(x)

// Builds a string that can be read by visual studio to jump directly to the sourcecode
#define VS_LOG(file, line, function, msg) (file"(" STRING(line) "): [" function "] ")

#ifdef USE_LOG
class Log
{
public:

	// Append starting information here and wait for more messages using the <<-operator
	Log(const char* type,const  char* file, int line,const  char* function, bool includeInfo=false, unsigned int messageBox=0 )
	{
		if(includeInfo)
			m_Info << file << "(" << line << "):" << "[" << function << "] "; // {0}({1}): <message here>
		else
			m_Info << type << ": ";

		m_MessageBoxStyle = messageBox;
	}

	// Flush the log message to wherever we need to
	~Log()
	{
		Flush();
	}

	/** Clears the logfile */
	static void Clear()
	{
#ifdef WIN32
		char path[MAX_PATH + 1];
		GetModuleFileNameA(NULL, path, MAX_PATH);
		s_LogFile = std::string(path);
		s_LogFile = s_LogFile.substr(0, s_LogFile.find_last_of('\\')+1);
		s_LogFile += "Log.txt";
#else
		s_LogFile = "./Log.txt";
#endif

		FILE* f;
		f = fopen(s_LogFile.c_str(),"w");
		fclose(f);
	}

	/** STL stringstream feature */
	template< typename T >
	inline Log& operator << ( const T &obj )
	{
		m_Message << obj;
		return *this;
	}

	inline Log& operator << ( std::wostream& (*fn)(std::wostream&) )
	{
		m_Message << fn;
		return *this;
	}

	/** Called when the object is getting destroyed, which happens immediately if simply calling the constructor of this class */
	inline void Flush()
	{	
		FILE* f;
		f = fopen(s_LogFile.c_str(),"a");

		// Append log-data to file
		if(f)
		{
			fputs(m_Info.str().c_str(),f);
			fputs(m_Message.str().c_str(),f);
			fputs("n",f);

			fclose(f);

			// Do callback
			if(s_LogCallback)
				s_LogCallback(m_Message.str());
		}

#ifdef WIN32
		OutputDebugString((m_Info.str() + m_Message.str() + "\n").c_str());
#endif

		// Pop a messagebox, if wanted
		switch(m_MessageBoxStyle)
		{
		case 1:
			InfoBox(m_Message.str().c_str());
			break;

		case 2:
			WarnBox(m_Message.str().c_str());
			break;

		case 3:
			ErrorBox(m_Message.str().c_str());
			break;
		}
	}

	/** Sets the function to be called when a log should be printed */
	static void SetLogCallback(std::function<void(const std::string&)> fn)
	{
		s_LogCallback = fn;
	}

private:

	static std::function<void(const std::string&)> s_LogCallback;
	static std::string s_LogFile;

	std::stringstream m_Info; // Contains an information like "Info", "Warning" or "Error"
	std::stringstream m_Message; // Text to write into the logfile
	unsigned int m_MessageBoxStyle; // Style of the messagebox if needed
};
#else

class Log
{
public:
	Log(const char* Type,const  char* File, int Line,const  char* Function, bool bIncludeInfo=false, UINT MessageBox=0 ) {}

	~Log(){	}

	/** Clears the logfile */
	static void Clear()	{ }

	/** STL stringstream feature */
	template< typename T >
	inline Log& operator << ( const T &obj )
	{
		return *this;
	}

	inline Log& operator << ( std::wostream& (*fn)(std::wostream&) )
	{
		return *this;
	}

	/** Called when the object is getting destroyed, which happens immediately if simply calling the constructor of this class */
	inline void Flush()	{ }

	/** Sets the function to be called when a log should be printed */
	static void SetLogCallback(std::function<void(const std::string&)> fn) {}
private:

};

#endif

#ifdef RND_GL

static void GetFirstNMessages(GLuint numMsgs)
{
	GLint maxMsgLen = 0;
	glGetIntegerv(GL_MAX_DEBUG_MESSAGE_LENGTH, &maxMsgLen);

	std::vector<GLchar> msgData(numMsgs * maxMsgLen);
	std::vector<GLenum> sources(numMsgs);
	std::vector<GLenum> types(numMsgs);
	std::vector<GLenum> severities(numMsgs);
	std::vector<GLuint> ids(numMsgs);
	std::vector<GLsizei> lengths(numMsgs);

	GLuint numFound = glGetDebugMessageLog(numMsgs, (GLsizei)msgData.size(), &sources[0], &types[0], &ids[0], &severities[0], &lengths[0], &msgData[0]);

	sources.resize(numFound);
	types.resize(numFound);
	severities.resize(numFound);
	ids.resize(numFound);
	lengths.resize(numFound);

	std::vector<std::string> messages;
	messages.reserve(numFound);

	std::vector<GLchar>::iterator currPos = msgData.begin();
	for(size_t msg = 0; msg < lengths.size(); ++msg)
	{
		messages.push_back(std::string(currPos, currPos + lengths[msg] - 1));
		currPos = currPos + lengths[msg];

		LogInfo() << messages.back();
	}
}

#define CheckGlError() { \
	{ \
		GLenum err(glGetError()); \
		\
		while(err != GL_NO_ERROR) {\
			std::string error;\
			\
			switch(err) {\
			case GL_INVALID_OPERATION:      error = "INVALID_OPERATION";      break;\
			case GL_INVALID_ENUM:           error = "INVALID_ENUM";           break;\
			case GL_INVALID_VALUE:          error = "INVALID_VALUE";          break;\
			case GL_OUT_OF_MEMORY:          error = "OUT_OF_MEMORY";          break;\
			case GL_INVALID_FRAMEBUFFER_OPERATION:  error = "INVALID_FRAMEBUFFER_OPERATION";  break;\
			}\
			\
			LogError() << "GL_" << error;\
			err = glGetError();\
			GetFirstNMessages(10);\
		}\
	}\
}
#endif