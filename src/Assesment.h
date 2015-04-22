#ifndef _ASSESMENT_H_
#define _ASSESMENT_H_

#include "Application.h"
#include "Camera.h"
#include "Vertex2.h"
#include "GPUParticle.h"
#include "FBXFile.h"
#include "AntTweakBar.h"

class Assesment : public Application
{
public:
	virtual bool startup();
	virtual void shutdown();
	virtual bool update();
	virtual void draw();
	void buildGrid(vec2 realDim, glm::ivec2 dims);
	void buildPerlinTexture(glm::ivec2 dims, int octaves, float persistance);

	TwBar* m_bar;

	OpenGLData m_planeMesh;
	unsigned int m_programID;
	unsigned int m_texture;
	unsigned int m_tprogramID;
	unsigned int m_texture2;
	unsigned int m_texture3;
	unsigned int m_perlinTexture;
	unsigned int m_fbxprogramID;
	unsigned int m_RockTexture;
	unsigned int m_GrassTexture;
	unsigned int m_WaterTexture;
	unsigned int m_vbo;
	unsigned int m_vao;
	unsigned int m_ibo;
	float* m_perlinData;
	FlyCamera m_Camera;
	float dims;
	float size;
	int octaves;
	float persistance;

	GPUParticleEmitter* m_emitter;
	float Time;

	bool load(const char* a_filename, FBXFile::UNIT_SCALE a_scale = FBXFile::UNITS_METER, bool a_loadTextures = true, bool a_loadAnimations = true, bool a_flipTextureY = true);
	unsigned int getMeshCount() const;
	FBXMeshNode* getMeshByIndex(unsigned int a_index) const;
	unsigned int m_vertexAttributes;
	FBXMaterial* m_material;
	std::vector<FBXVertex> m_vertices;
	std::vector<unsigned int> m_indices;
	glm::mat4 m_localTransform;
	glm::mat4 m_globalTransform;
	void* m_userData;
	void createOpenGLBuffers(FBXFile* fbx);
	void cleanupOpenGLBuffers(FBXFile* fbx);
	FBXFile* m_fbx;
	FBXFile* m_fbx2;

	vec3 m_lightDirection;

};



#endif