//#include "Animation.h"
//
//#include "gl_core_4_4.h"
//#include "GLFW/glfw3.h"
//#include "Gizmos.h"
//#include "Vertex.h"
//
//bool Animation::startup()
//{
//	if (Application::startup() == false)
//	{
//		return false;
//	}
//	glClearColor(0.3f,0.3f,0.3f,1.0f);
//	glEnable(GL_DEPTH_TEST);
//	Gizmos::create();
//
//	camera = FlyCamera(1280.0f / 720.0f, 10.0f);
//	camera.setLookAt(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0));
//	camera.sensitivity = 3;
//
//	m_file = new FBXFile();
//	m_file->load("./data/models/characters/Marksman/Marksman.fbx");
//
//	m_file->initialiseOpenGLTextures();
//	GenerateGLMeshes(m_file);
//
//	// load shaders
//
//	const char* vsSource = "#version 410\n \
//							layout(location=0) in vec4 Position; \
//							layout(location=1) in vec2 TexCoord; \
//							layout(location=2) in vec4 indices; \
//							layout(location=3) in vec4 weights; \
//							out vec2 vTexCoord; \
//							uniform mat4 ProjectionView; \
//							uniform mat4 world; \
//							const int Max_Bones = 128; \
//							uniform mat4 bones[Max_Bones]; \
//							void main() { \
//							vTexCoord = TexCoord; \
//							ivec4 indices = ivec4(indices); \
//							vec4 final_position = vec4(0, 0, 0, 0); \
//							final_position += bones[indices.x] * Position * weights.x; \
//							final_position += bones[indices.y] * Position * weights.y; \
//							final_position += bones[indices.z] * Position * weights.z; \
//							final_position += bones[indices.w] * Position * weights.w; \
//							final_position.w = 1; \
//							gl_Position = ProjectionView * world * Position; \
//	   						}";
//
//	const char* fsSource = "#version 410\n \
//							in vec2 vTexCoord; \
//							out vec4 FragColor; \
//							uniform sampler2D diffuse; \
//							void main() { \
//							FragColor = texture(diffuse,vTexCoord);\
//							}";
//	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
//	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
//	glCompileShader(vertexShader);
//	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
//	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
//	glCompileShader(fragmentShader);
//	m_program = glCreateProgram();
//	glAttachShader(m_program, vertexShader);
//	glAttachShader(m_program, fragmentShader);
//	glLinkProgram(m_program);
//	glDeleteShader(vertexShader);
//	glDeleteShader(fragmentShader);
//
//	return true;
//}
//
//void Animation::shutdown()
//{
//	m_file->unload();
//	delete m_file;
//	Gizmos::destroy();
//	Application::shutdown();
//}
//
//bool Animation::update()
//{
//	if (Application::update() == false)
//	{
//		return false;
//	}
//	Gizmos::clear();
//
//	float dt = (float)glfwGetTime();
//	glfwSetTime(0.0);
//
//	vec4 white(1);
//	vec4 black(0, 0, 0, 1);
//	for (int i = 0; i <= 20; ++i)
//	{
//		Gizmos::addLine(vec3(-10 + i, -0.01, -10), vec3(-10 + i, -0.01, 10),
//			i == 10 ? white : black);
//		Gizmos::addLine(vec3(-10, -0.01, -10 + i), vec3(10, -0.01, -10 + i),
//			i == 10 ? white : black);
//	}
//
//	FBXSkeleton* skele = m_file->getSkeletonByIndex(0);
//	FBXAnimation* anim = m_file->getAnimationByIndex(0);
//
//	skele->evaluate(anim, 0);
//	glUseProgram(m_program);
//	for (unsigned int i = 0; i < skele->m_boneCount; i++)
//	{
//		skele->m_nodes[i]->updateGlobalTransform();
//		mat4 node_global = skele->m_nodes[i]->m_globalTransform;
//		vec3 node_pos = node_global[3].xyz;
//
//		Gizmos::addAABBFilled(node_pos, vec3(0.5f), vec4(1, 0, 0, 1), &node_global);
//		if (skele->m_nodes[i]->m_parent != nullptr)
//		{
//			vec3 parent_pos = skele->m_nodes[i]->m_parent->m_globalTransform[3].xyz;
//			Gizmos::addLine(node_pos, parent_pos, vec4(0, 1, 0, 1));
//		}
//	}
//
//	camera.update(dt);
//
//	return true;
//}
//
//void Animation::draw()
//{
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//	Gizmos::draw(camera.proj, camera.view);
//
//	int view_projection = 
//		glGetUniformLocation(m_program, "ProjectionView");
//	glUniformMatrix2fv(view_projection, 1, GL_FALSE, (float*)&camera.view_proj);
//
//	int diffuse_uniform =
//		glGetUniformLocation(m_program, "diffuse");
//	glUniform1i(diffuse_uniform, 0);
//
//	FBXSkeleton * skeleton = m_file->getSkeletonByIndex(0);
//
//	skeleton->updateBones();
//	int bones_uniform = 
//		glGetUniformLocation(m_program, "bones");
//
//	glUniformMatrix4fv(bones_uniform, skeleton->m_boneCount, GL_FALSE, 
//						(float*)skeleton->m_bones);
//
//	for (unsigned int i = 0; i < m_meshs.size(); i++)
//	{
//		FBXMeshNode* curr_mesh = m_file->getMeshByIndex(i);
//		mat4 world_transfrom = curr_mesh->m_globalTransform;
//
//		FBXMaterial* mesh_material = curr_mesh->m_material;
//
//		glActiveTexture(GL_TEXTURE0);
//		glBindTexture(GL_TEXTURE_2D, 
//			mesh_material->textures[FBXMaterial::DiffuseTexture]->handle);
//
//		int world_uniform =
//			glGetUniformLocation(m_program, "world");
//		glUniformMatrix4fv(world_uniform, 1, GL_FALSE, (float*)&world_transfrom);
//
//		int bones_handle = glGetUniformLocation(m_program, "bones");
//
//		glBindVertexArray(m_meshs[i].m_VAO);
//		glDrawElements(GL_TRIANGLES, m_meshs[i].index_count, GL_UNSIGNED_INT, 0);
//	}
//
//	glfwSwapBuffers(m_window);
//	glfwPollEvents();
//}
//
//void Animation::GenerateGLMeshes(FBXFile* fbx)
//{
//	unsigned int Mesh_count = fbx->getMeshCount();
//
//	m_meshs.resize(Mesh_count);
//
//	for (unsigned int mesh_index = 0; mesh_index < Mesh_count; mesh_index++)
//	{
//		FBXMeshNode* curr_mesh = fbx->getMeshByIndex(mesh_index);
//
//		m_meshs[mesh_index].index_count = curr_mesh->m_indices.size();
//
//		glGenBuffers(1, &m_meshs[mesh_index].m_VBO);
//		glGenBuffers(1, &m_meshs[mesh_index].m_IBO);
//		glGenVertexArrays(1, &m_meshs[mesh_index].m_VAO);
//
//		glBindVertexArray(m_meshs[mesh_index].m_VAO);
//
//		glBindBuffer(GL_ARRAY_BUFFER, m_meshs[mesh_index].m_VBO);
//		glBufferData(GL_ARRAY_BUFFER, sizeof(FBXVertex)* curr_mesh->m_vertices.size(), curr_mesh->m_vertices.data(), GL_STATIC_DRAW);
//
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_meshs[mesh_index].m_IBO);
//		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)* curr_mesh->m_indices.size(), curr_mesh->m_indices.data(), GL_STATIC_DRAW);
//
//		glEnableVertexAttribArray(0); // pos
//		glEnableVertexAttribArray(1); // texcoord
//		glEnableVertexAttribArray(2); // 
//		glEnableVertexAttribArray(3); // 
//
//		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::PositionOffset);
//		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::TexCoord1Offset);
//		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::IndicesOffset);
//		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::WeightsOffset);
//
//		glBindVertexArray(0);
//		glBindBuffer(GL_ARRAY_BUFFER, 0);
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//
//	}
//
//}
//
//void Animation::EvaluateSkeleton(FBXAnimation* anim, FBXSkeleton* skeleton, float timer)
//{
//	float fps = 24.f;
//	int current_frame = (int)(timer * fps);
//	for (unsigned int track_index = 0; track_index < anim->m_trackCount; track_index++)
//	{
//		int bone_index = anim->m_tracks[track_index].m_keyframeCount;
//		int track_frame_count = current_frame % track_frame_count;
//
//		FBXKeyFrame curr_frame = anim->m_tracks[track_index].m_keyframes[current_frame];
//		FBXKeyFrame next_frame = anim->m_tracks[track_index].m_keyframes[(current_frame + 1) % track_frame_count];
//
//		float time_since_frameflip = timer - ((int)(timer * fps));
//		float t = time_since_frameflip * fps;
//
//		vec3 new_pos = glm::mix(curr_frame.m_translation, next_frame.m_translation, t);
//		vec3 new_scale = glm::mix(curr_frame.m_scale, next_frame.m_scale, t);
//		glm::quat new_rot = glm::slerp(curr_frame.m_rotation, next_frame.m_rotation, t);
//		mat4 local_transform = glm::translate(new_pos)*glm::toMat4(new_rot)*glm::scale(new_scale);
//		int bone_index = anim->m_tracks[track_index].m_boneIndex;
//		if (bone_index < skeleton->m_boneCount)
//		{
//			skeleton->m_nodes[bone_index]->m_localTransform = local_transform;
//		}
//	}
//}
//
//void Animation::UpdateBones(FBXSkeleton* skeleton)
//{
//	for (unsigned int bone_index = 0; bone_index < skeleton->m_boneCount; bone_index++)
//	{
//		int parent_index = skeleton->m_parentIndex[bone_index];
//		if (parent_index == -1)
//		{
//			skeleton->m_bones[bone_index] = skeleton->m_nodes[bone_index]->m_localTransform;
//		}
//		else
//		{
//			skeleton->m_bones[bone_index] = skeleton->m_nodes[bone_index]->m_localTransform*skeleton->m_bones[parent_index];
//		}
//		skeleton->m_bones[bone_index] = skeleton->m_bindPoses[bone_index] * skeleton->m_bones[bone_index];
//	}
//}