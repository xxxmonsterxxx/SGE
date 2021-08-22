#include "Mesh.h"
#include "SGE.h"

Mesh::Mesh(std::string name, std::vector<SGEPosition> vertices, std::vector<uint16_t> indices) :
			_name(name), _vertices(vertices), _indices(indices)
{
    VkVertexInputAttributeDescription positionDescr;
	positionDescr.binding = 0;
	positionDescr.location = 0;
	positionDescr.format = VK_FORMAT_R32G32_SFLOAT;
	positionDescr.offset = offsetof(Sgr2DVertex, position);

	VkVertexInputAttributeDescription colorDescr;
	colorDescr.binding = 0;
	colorDescr.location = 1;
	colorDescr.format = VK_FORMAT_R32G32B32_SFLOAT;
	colorDescr.offset = offsetof(Sgr2DVertex, color);

	_meshAttributeDescriptions.push_back(positionDescr);
    _meshAttributeDescriptions.push_back(colorDescr);

	VkVertexInputBindingDescription vertexBindingDescription;
	vertexBindingDescription.binding = 0;
	vertexBindingDescription.stride = sizeof(Sgr2DVertex);
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
	if (SGE::renderer.addNewObjectGeometry(_name,
										pointsToSGRVertex(_vertices), _indices,
										shaderVertex, shaderFragment,
										_meshBindingDescriptions, _meshAttributeDescriptions, _additionalDataLayouts) != sgrOK)
		return false;

	return true;
}