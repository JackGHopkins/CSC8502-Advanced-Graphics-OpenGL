#pragma once
#include "SceneNode.h"
#include "OGLRenderer.h"
#include "HeightMap.h"

class HeightMapNode : public SceneNode
{
public:
	HeightMap* heightMap;
	GLuint terrainTex;
	GLuint stoneTex;
	GLuint slopeTex;
	GLuint splatTex;
	virtual void Draw(OGLRenderer &r);
	HeightMapNode(const std::string& name, OGLRenderer &r, Shader *shader);
	~HeightMapNode() { delete heightMap; }
};