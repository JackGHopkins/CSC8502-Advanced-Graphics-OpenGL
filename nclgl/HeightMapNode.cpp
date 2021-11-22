#include "HeightMapNode.h"

void HeightMapNode::Draw(OGLRenderer& r)
{
	PreDraw(r);
	glUniform1i(glGetUniformLocation(shader->GetProgram(),
		"diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, terrainTex);

	glUniform1i(glGetUniformLocation(shader->GetProgram(),
		"stoneTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, stoneTex);

	glUniform1i(glGetUniformLocation(shader->GetProgram(),
		"slopeTex"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, slopeTex);

	glUniform1i(glGetUniformLocation(shader->GetProgram(),
		"splatTex"), 3);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, splatTex);

	glUniform2f(glGetUniformLocation(shader->GetProgram(),
		"resolution"), heightMap->GetHeightmapSize().x, heightMap->GetHeightmapSize().z);

	heightMap->Draw();
}

HeightMapNode::HeightMapNode(const std::string& name, OGLRenderer &r, Shader*shader) 
	: SceneNode(nullptr, Vector4(1,1,1,1),shader)
{
	heightMap = new HeightMap(name);
	terrainTex = SOIL_load_OGL_texture(TEXTUREDIR"forest.JPG",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	if (!terrainTex) {
		return;
	}

	r.SetTextureRepeating(terrainTex, true);


	stoneTex = SOIL_load_OGL_texture(TEXTUREDIR"rocks.JPG",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	if (!stoneTex) {
		return;
	}

	r.SetTextureRepeating(stoneTex, true);

	slopeTex = SOIL_load_OGL_texture(TEXTUREDIR"slope.JPG",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	if (!slopeTex) {
		return;
	}

	r.SetTextureRepeating(slopeTex, true);
	splatTex = SOIL_load_OGL_texture(TEXTUREDIR"island splatmap.JPG",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	if (!splatTex) {
		return;
	}

	r.SetTextureRepeating(splatTex, true);
}
