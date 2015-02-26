#ifndef _VERTEX_H_
#define _VERTEX_H_
#define GLM_SWIZZLE

#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "gl_core_4_4.h"


using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;

struct Vertex
{
	vec4 position;
	vec4 colour;
};

#endif //VERTEX_H_