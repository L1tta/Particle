#ifndef _UTILITY_H_
#define _UTILITY_H_

typedef unsigned int GLuint;

bool loadShaderType(char* a_fileName, 
	unsigned int a_shaderType, 
	GLuint* a_result);

bool loadShaders(char* a_vertShaderFile, 
	char* a_geomShaderFile, 
	char* a_fragShaderFile, 
	GLuint* a_result);

bool checkProgramError(unsigned int a_program, 
	unsigned int a_statusType);

bool checkShaderError(unsigned int a_shader,
	unsigned int a_statusType,
	unsigned int a_shaderType);

#endif