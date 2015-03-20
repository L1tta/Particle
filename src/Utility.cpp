#include "Utility.h"
#include <cstdio>
#include <iostream>
#include "gl_core_4_4.h"

#include <vector>

bool loadShaderType(char* a_fileName,
	unsigned int a_shaderType,
	GLuint* a_result)
{
	bool loadSucceeded = true;

	//load the shader file
	FILE* shaderFile = fopen(a_fileName, "r");
	
	//check if shader file loaded successfully
	if (shaderFile == 0)
	{
		//close the file
		fclose(shaderFile);
	}
	else
	{
		//find out how long the file is
		fseek(shaderFile, 0, SEEK_END);
		int shaderFileLength = ftell(shaderFile);
		fseek(shaderFile, 0, SEEK_SET);

		//allocate enough space for the file
		char* shaderSource = new char[shaderFileLength];

		//read the file and update the length to be accurate
		shaderFileLength = fread(shaderSource, 1, shaderFileLength, shaderFile);
		
		//create the shader based on the type that got passed in
		unsigned int shaderHandle = glCreateShader(a_shaderType);

		//compile the shader
		glShaderSource(shaderHandle, 1, &shaderSource, &shaderFileLength);
		glCompileShader(shaderHandle);

		//check the shader for errors
		if (checkShaderError(shaderHandle, GL_COMPILE_STATUS, a_shaderType) == true)
		{
			//only give the result to the caller if we succeeded
			*a_result = shaderHandle;
		}
		else
		{
			loadSucceeded = false;
		}
		//lcleanup the stuff we allocated
		delete[] shaderSource;
		fclose(shaderFile);
	}
	return loadSucceeded;
}

bool loadShaders(char* a_vertShaderFile, 
	char* a_geomShaderFile,
	char* a_fragShaderFile, 
	GLuint* a_result)
{
	bool loadSucceeded = true;

	*a_result = glCreateProgram();
	
	unsigned int vertShader;
	loadShaderType(a_vertShaderFile, GL_VERTEX_SHADER, &vertShader);
	glAttachShader(*a_result, vertShader);
	glDeleteShader(vertShader);

	if (a_geomShaderFile != nullptr)
	{
		unsigned int geomShader;
		if (loadShaderType(a_geomShaderFile, GL_GEOMETRY_SHADER, &geomShader))
		{
			glAttachShader(*a_result, geomShader);
			glDeleteShader(geomShader);
		}
		else
		{
			std::cout << "Error: Failed to compile Geometry shader!" << std::endl;
		}
	}
	if (a_fragShaderFile != nullptr)
	{
		unsigned int fragShader;
		if (loadShaderType(a_fragShaderFile, GL_FRAGMENT_SHADER, &fragShader))
		{
			glAttachShader(*a_result, fragShader);
			glDeleteShader(fragShader);
		}
	}

	glLinkProgram(*a_result);

	if (checkProgramError(*a_result, GL_LINK_STATUS) == false)
	{
		loadSucceeded = false;
	}

	return loadSucceeded;
}

bool checkProgramError(unsigned int a_program, unsigned int a_statusType)
{
	//Error checking
	int success = GL_FALSE;

	glGetProgramiv(a_program, a_statusType, &success);
	if (success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetProgramiv(a_program, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength <= 0)
		{
			return false;
		}
		std::vector<GLchar> infoLog(infoLogLength);
		glGetProgramInfoLog(a_program, infoLogLength, &infoLogLength, &infoLog[0]);
		std::cout << "Error: Failed to link shader program!" << std::endl;
		for (unsigned int i = 0; i < infoLog.size(); ++i)
		{
			std::cout << infoLog[i];
		}
		std::cout << std::endl;
		return false;
	}
	return true;
}

bool checkShaderError(unsigned int a_shader,
	unsigned int a_statusType, 
	unsigned int a_shaderType)
{
	//Error checking
	int success = GL_FALSE;
	glGetShaderiv(a_shader, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetShaderiv(a_shader, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength <= 0)
		{
			return false;
		}
		std::vector<GLchar> infoLog(infoLogLength);
		glGetShaderInfoLog(a_shader, infoLogLength, &infoLogLength, &infoLog[0]);
		std::cout << "Error: Failed to link shader program! ShaderType: " << a_shaderType <<  std::endl;
		for (unsigned int i = 0; i < infoLog.size(); ++i)
		{
			std::cout << infoLog[i];
		}
		std::cout << std::endl;
		return false;
	}
	return true;
}