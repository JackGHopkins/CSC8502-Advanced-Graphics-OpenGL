#pragma once
#include "../nclgl/OGLRenderer.h"
#include "../nclgl/Camera.h"
#include "../nclgl/SceneNode.h"
#include "../nclgl/CubeRobot.h"
#include "../nclgl/Frustum.h"
#include "../nclgl/Mesh.h"
#include "../nclgl/Shader.h"

class Renderer : public OGLRenderer {
public:
	Renderer(Window& parent);
	~Renderer(void);

	void UpdateScene(float dt) override;
	void RenderScene() override;

protected:
	void BuildNodeLists(SceneNode* from);
	void SortNodeLists();
	void ClearNodeLists();
	void DrawNode();
	void DrawNode(SceneNode* n);

	SceneNode* root;
	Camera* camera;
	Mesh* quad;
	Mesh* cube;
	Shader* shader;
	GLuint texture;

	Frustum frameFrustum;

	std::vector<SceneNode*> transparentNodeList;
	std::vector<SceneNode*> nodeList;

	SceneNode* root;
	Camera* camera;
	Mesh* cube;
	Shader* shader;
};
