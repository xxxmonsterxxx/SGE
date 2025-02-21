#include "Model.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "SGE.h"	

#include "tiny_obj_loader.h"

#include <filesystem>


bool Model::loadGLTFData(std::string path)
{
	return false;
}

bool Model::loadObjData(std::string path)
{
	std::string fullPath = SGE::getExecPath() + path;
	std::filesystem::path fsPath(path);
	std::string modelName = fsPath.filename().string();

	std::string objPath = SGE::getExecPath() + path + "/" + modelName + ".obj";
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, objPath.c_str(), fullPath.c_str())) {
		return false;
	}

	bool noMaterials = materials.empty();
	if (noMaterials) {
		
	}

	noMaterials = true;

	std::vector<int> addedTextures;
	std::vector<SgrImage*> textures;
	std::vector<ObjModelVertex> verts;
	for (const auto& shape : shapes) {
		for (const auto& index : shape.mesh.indices) {
			ObjModelVertex v{};
			v.vertex.x = attrib.vertices[3 * index.vertex_index + 0];
			v.vertex.y = attrib.vertices[3 * index.vertex_index + 1];
			v.vertex.z = attrib.vertices[3 * index.vertex_index + 2];

			v.texCoord = { attrib.texcoords[2 * index.texcoord_index + 0],
								 1.0f - attrib.texcoords[2 * index.texcoord_index + 1] };

			v.texInd = 0;
			v.color[3] = 1;
			if (!noMaterials) {
				int requiredTexture = shape.mesh.material_ids[0];

				v.color[3] = materials[requiredTexture].dissolve;
				if (materials[requiredTexture].diffuse_texname.empty()) {
					v.texInd = -1;
					v.color[0] = materials[requiredTexture].diffuse[0];
					v.color[1] = materials[requiredTexture].diffuse[1];
					v.color[2] = materials[requiredTexture].diffuse[2];
				}
				else {
					bool added = std::find(addedTextures.begin(), addedTextures.end(), requiredTexture) != addedTextures.end();

					if (!added) {
						SgrImage* texture = nullptr;
						std::string textName = fullPath + "/" + materials[requiredTexture].diffuse_texname;
						if (TextureManager::createTextureImage(textName, texture) != sgrOK)
							return false;
						else {
							textures.push_back(texture);
							addedTextures.push_back(requiredTexture);
							v.texInd = (int)(textures.size()) - 1;
						}
					}
					else {
						auto it = std::find(addedTextures.begin(), addedTextures.end(), requiredTexture);
						v.texInd = (int)std::distance(addedTextures.begin(), it);
						if (v.texInd < 0 || v.texInd > addedTextures.size() - 1) {
							return false;
						}
					}
				}
			}

			verts.push_back(v);
			indices.push_back((uint32_t)(verts.size()) - 1);
		}
	}



	return true;
}

void Model::normalize()
{
	/*SgrVertex centroid{ 0,0,0 };
	for (const auto& v : vertices) {
		centroid += v.vertex;
	}

	for (auto& v : vertices) {
		v.vertex -= (centroid / (float)vertices.size());
	}*/
}

Model::Model(std::string name, std::string modelPath, ModelType type)
{
	switch (type) {
		case OBJ:
			loadObjData(modelPath);
			break;
		case GLTF:
			loadGLTFData(modelPath);
	};

	normalize();
	
	mesh = new Mesh(name, vertices, indices, objBindingDescription, objAttributeDescription, objLayoutBindings, true);
}
