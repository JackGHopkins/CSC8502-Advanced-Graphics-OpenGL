#pragma once
#include <algorithm>
#include <vector>

#include "../nclgl/Camera.h"
#include "../nclgl/Frustum.h"
#include "../nclgl/HeightMap.h"
#include "../nclgl/Light.h"
#include "../nclgl/Mesh.h"
#include "../nclgl/OGLRenderer.h"
#include "../nclgl/SceneNode.h"
#include "../nclgl/Shader.h"

class Renderer : public OGLRenderer	{
public:
	Renderer(Window &parent);
	 ~Renderer(void);
	 void RenderScene()				override;
	 void UpdateScene(float msec)	override;
	 void ToggleAutomaticCamera();
	 void PrintCoord();

	 bool automaticCamera;

protected:
	void BuildNodeLists(SceneNode* from);
	void SortNodeLists();
	void ClearNodeLists();
	void DrawHeightMap();
	void DrawNodes();
	void DrawNodes(SceneNode* n);
	void DrawSkybox();
	void DrawWater();

	float waterRotate;
	float waterCycle;

	Camera* camera;
	Frustum frameFrustum;
	
	GLuint texture;
	GLuint normalMap;
	GLuint cubeMap;
	GLuint waterTex;
	GLuint earthTex;
	GLuint earthBump;

	HeightMap* heightMap;

	Light* light;

	Mesh*triangle;
	Mesh* quad;
	Mesh* cube;
	
	Shader* shader;
	Shader* lightShader;
	Shader* reflectShader;
	Shader* skyboxShader;
	
	SceneNode* root;

	std::vector<SceneNode*> transparentNodeList;
	std::vector<SceneNode*> nodeList;
};
