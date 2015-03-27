#ifndef DEFERREDRENDERING_H_
#define DEFERREDRENDERING_H_
#include "Application.h"
#include "Camera.h"
#include "FBXFile.h"

class DeferredRendering : public Application
{
public:

	virtual bool startup();
	virtual void shutdown();
	virtual bool update();
	virtual void draw();

	FlyCamera camera;
	vec4 m_background_color;

	unsigned int m_fbo;
	unsigned int m_fboTexture;
	unsigned int m_fboDepth;
	unsigned int m_vao;
	unsigned int m_vbo;
	unsigned int m_ibo;
	unsigned int m_program;
	unsigned int m_gpassFBO;
	unsigned int m_albedoTexture;
	unsigned int m_positionTexture;
	unsigned int m_normalTexture;
	unsigned int m_gpassDepth;
	unsigned int m_lightFBO;
	unsigned int m_lightTexture;
	unsigned int loadShader(unsigned int type, const char* path);
	void createDrawShader();
	unsigned int m_gpassShader;
	void createOpenGLBuffers(FBXFile* fbx);
	void drawDirectionalLight(const glm::vec3& direction,
		const glm::vec3& diffuse);
	unsigned int m_directionLightShader;
	unsigned int m_quadVAO;
	unsigned int m_compositeShader;
	float timer = 0.0f;

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
	void cleanupOpenGLBuffers(FBXFile* fbx);
	FBXFile* m_fbx;
};

#endif //DEFERREDRENDERING_H_