#include "Mesh.h"
#include "SGE.h"

const std::vector <SGEPosition> Mesh::defaultRectangleVertices = {
		{-0.5f,-0.5f,0.f},
		{0.5f,-0.5f,0.f},
		{0.5f,0.5f,0.f},
		{-0.5f,0.5f,0.f}
	};
const std::vector<uint32_t> Mesh::defaultRectangleIndices = { 0,1,2,2,3,0 };

const std::vector <SGEPosition> Mesh::defaultTriangleVertices = {
		{-0.5f,-0.5f,0.f},
		{0.5f,-0.5f,0.f},
		{0.5f,0.5f,0.f}
	};
const std::vector<uint32_t> Mesh::defaultTriangleIndices = { 0,1,2 };

Mesh Mesh::getDefaultRectangleMesh(const std::string name, const bool filled)
{
	return Mesh(name, defaultRectangleVertices, defaultRectangleIndices, filled);
}

Mesh Mesh::getDefaultTriangleMesh(const std::string name, const bool filled)
{
	return Mesh(name, defaultTriangleVertices, defaultTriangleIndices, filled);
}


Mesh::Mesh(const std::string name, const std::vector<SGEPosition> vertices, const std::vector<uint32_t> indices, const bool filled) :
			_name(name), _vertices(vertices), _indices(indices), _filled(filled)
{
    VkVertexInputAttributeDescription positionDescr{};
	positionDescr.binding = 0;
	positionDescr.location = 0;
	positionDescr.format = VK_FORMAT_R32G32B32_SFLOAT;
	positionDescr.offset = 0;

	_meshAttributeDescriptions.push_back(positionDescr);

	VkVertexInputBindingDescription vertexBindingDescription{};
	vertexBindingDescription.binding = 0;
	vertexBindingDescription.stride = sizeof(SgrVertex);
	vertexBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	_meshBindingDescriptions.push_back(vertexBindingDescription);

	VkDescriptorSetLayoutBinding uboLayoutBinding{};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	uboLayoutBinding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutBinding samplerLayoutBinding{};
	samplerLayoutBinding.binding = 1;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	samplerLayoutBinding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutBinding instanceUBOLayoutBinding{};
	instanceUBOLayoutBinding.binding = 2;
	instanceUBOLayoutBinding.descriptorCount = 1;
	instanceUBOLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	instanceUBOLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	instanceUBOLayoutBinding.pImmutableSamplers = nullptr;

    _additionalDataLayouts.push_back(uboLayoutBinding);
	_additionalDataLayouts.push_back(samplerLayoutBinding);
	_additionalDataLayouts.push_back(instanceUBOLayoutBinding);

	shaderVertex = defaultShaderVertex;
	shaderFragment = defaultShaderFragmentColor;

	verticesData = (void*)_vertices.data();
	verticesDataSize = _vertices.size() * sizeof(SgrVertex);
}

Mesh::Mesh(const std::string name, void* vertices, VkDeviceSize verticesSize, const std::vector<uint32_t> indices,
							std::vector<VkVertexInputBindingDescription> bindDescr,
							std::vector<VkVertexInputAttributeDescription> attrDescr,
							std::vector<VkDescriptorSetLayoutBinding> layoutBind,
							std::string vertexShader,
							std::string fragmentShader,
							const bool filled) : _name(name), _filled(filled), _indices(indices), verticesData(vertices), verticesDataSize(verticesSize)
{
	_meshBindingDescriptions = bindDescr;
	_meshAttributeDescriptions = attrDescr;
	_additionalDataLayouts = layoutBind;

	shaderVertex = vertexShader;
	shaderFragment = fragmentShader;
}

bool Mesh::init()
{
	if (SGE::renderer.addNewObjectGeometry(_name,
										verticesData, verticesDataSize, _indices,
										SGE::resourcesPath+shaderVertex, SGE::resourcesPath+shaderFragment, _filled,
										_meshBindingDescriptions, _meshAttributeDescriptions, _additionalDataLayouts) != sgrOK)
		return false;

	return true;
}

glm::vec2 Mesh::getTextureBindPoint()
{
	return _vertices[_indices[0]];
}

void Mesh::useTexture()
{
	shaderFragment = defaultShaderFragmentTexture;
}

void Mesh::generateInstanceData(GameObject* go, void* data)
{
	MeshInstanceData newData;
	newData.model = go->getModel();
	newData.color = go->getColor();
	newData.meshToTextureDelta = go->getDeltaTexture();
	newData.meshStart = go->getMeshStart();
	newData.textureStart = go->getTextureStart();

	*(MeshInstanceData*)data = newData;
}

bool Mesh::isCorrect()
{
	if (!_name.empty() && (!_vertices.empty() || verticesData)  && !_indices.empty())
		return true;

	return false;
}