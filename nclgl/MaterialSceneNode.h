#pragma once
#include "SceneNode.h"
class MeshMaterial;

class MaterialSceneNode :
	public SceneNode
{
	MeshMaterial* material;
	vector<GLuint> matTextures;
public:
	MaterialSceneNode(Shader *shader);
	~MaterialSceneNode();
	void Draw(OGLRenderer& r);
};

