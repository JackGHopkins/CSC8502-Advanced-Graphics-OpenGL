#pragma once
//#include "OGLRenderer.h"
#include <string>
#include <vector>
#include "glad\glad.h"
#include "GL/GL.h"
class Shader;

class ComputeShader
{
public:
	ComputeShader(const std::string& filename);
	~ComputeShader(void);
	GLuint  GetProgram() { return programID; }

	void Bind()		const;
	void Unbind()	const;
	void Dispatch(unsigned int x, unsigned int y = 0, unsigned int z = 0) const;

	//void GetThreadsInGroup(int& x, int& y, int& z) const;

protected:
	GLuint	programID;
	GLuint	shaderID;
	GLint	programValid;
	GLint	threadsInGroup[3];

	static std::vector<Shader*> allShaders;
};

