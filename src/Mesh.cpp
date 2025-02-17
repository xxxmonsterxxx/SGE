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
    VkVertexInputAttributeDescription positionDescr;
	positionDescr.binding = 0;
	positionDescr.location = 0;
	positionDescr.format = VK_FORMAT_R32G32B32_SFLOAT;
	positionDescr.offset = 0;

	_meshAttributeDescriptions.push_back(positionDescr);

	VkVertexInputBindingDescription vertexBindingDescription;
	vertexBindingDescription.binding = 0;
	vertexBindingDescription.stride = sizeof(SgrVertex);
	vertexBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	_meshBindingDescriptions.push_back(vertexBindingDescription);

	VkDescriptorSetLayoutBinding uboLayoutBinding;
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
}

bool Mesh::init()
{
	VkDeviceSize vertsSize = _vertices.size() * sizeof(SgrVertex);
	if (SGE::renderer.addNewObjectGeometry(_name,
										(void*)_vertices.data(), vertsSize, _indices,
										SGE::execPath+shaderVertex, _textured?SGE::execPath+shaderFragmentTexture:SGE::execPath+shaderFragmentColor, _filled,
										_meshBindingDescriptions, _meshAttributeDescriptions, _additionalDataLayouts) != sgrOK)
		return false;

	return true;
}

glm::vec2 Mesh::getTextureBindPoint()
{
	return _vertices[_indices[0]];
}