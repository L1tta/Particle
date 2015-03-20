#ifndef _VERTEX2_H_
#define _VERTEX2_H_
#define GLM_SWIZZLE

#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "gl_core_4_4.h"


using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;

struct Vertex2
{
	vec4 position;
	vec4 color;
};

struct VertexTexCoord
{
	vec4 position;
	vec2 texCoord;
};

struct VertexNormal
{
	vec4 position;
	vec4 normal;
	vec4 tangent;
	vec2 texCoord;
};

struct OpenGLData
{
	unsigned int m_VAO;
	unsigned int m_VBO;
	unsigned int m_IBO;
	unsigned int m_indexCount;
};

#endif