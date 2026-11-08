#include "Cafe/HW/Latte/Renderer/Vulkan/FrameGenVk.h"
#include "Cafe/HW/Latte/Renderer/Vulkan/VulkanAPI.h"

#include <vector>

namespace FrameGenVk
{
	Capabilities QueryCapabilities(VkPhysicalDevice physicalDevice, uint32_t graphicsQueueFamily)
	{
		Capabilities result{};
		if (physicalDevice == VK_NULL_HANDLE)
			return result;

		VkPhysicalDeviceFeatures2 features{};
		features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
		vkGetPhysicalDeviceFeatures2(physicalDevice, &features);
		result.computeShader = features.features.computeShader == VK_TRUE;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
		if (graphicsQueueFamily < queueFamilyCount)
		{
			std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());
			const auto& graphicsQueue = queueFamilies[graphicsQueueFamily];
			result.graphicsQueueSupportsCompute = graphicsQueue.queueCount > 0 && (graphicsQueue.queueFlags & VK_QUEUE_COMPUTE_BIT) != 0;
			}

		VkFormatProperties rgba8Properties{};
		vkGetPhysicalDeviceFormatProperties(physicalDevice, VK_FORMAT_R8G8B8A8_UNORM, &rgba8Properties);
		result.rgba8StorageImage = (rgba8Properties.optimalTilingFeatures & VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT) != 0;

		VkFormatProperties rgba16fProperties{};
		vkGetPhysicalDeviceFormatProperties(physicalDevice, VK_FORMAT_R16G16B16A16_SFLOAT, &rgba16fProperties);
		result.rgba16fStorageImage = (rgba16fProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT) != 0;

		return result;
	}

	VkResult CreateComputePipeline(
		VkDevice device,
		std::span<const uint32_t> spirv,
		VkPipelineLayout pipelineLayout,
		VkPipeline* pipeline)
	{
		if (pipeline)
			*pipeline = VK_NULL_HANDLE;
		if (device == VK_NULL_HANDLE || pipelineLayout == VK_NULL_HANDLE || pipeline == nullptr ||
			spirv.empty() || (spirv.size_bytes() % sizeof(uint32_t)) != 0)
			return VK_ERROR_INITIALIZATION_FAILED;

		VkShaderModuleCreateInfo shaderModuleInfo{};
		shaderModuleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		shaderModuleInfo.codeSize = spirv.size_bytes();
		shaderModuleInfo.pCode = spirv.data();

		VkShaderModule shaderModule = VK_NULL_HANDLE;
		VkResult result = vkCreateShaderModule(device, &shaderModuleInfo, nullptr, &shaderModule);
		if (result != VK_SUCCESS)
			return result;

		VkPipelineShaderStageCreateInfo stageInfo{};
		stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		stageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
		stageInfo.module = shaderModule;
		stageInfo.pName = "main";

		VkComputePipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		pipelineInfo.stage = stageInfo;
		pipelineInfo.layout = pipelineLayout;

		result = vkCreateComputePipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, pipeline);
		vkDestroyShaderModule(device, shaderModule, nullptr);
		return result;
	}

	bool RecordDispatch(
		VkCommandBuffer commandBuffer,
		VkPipeline pipeline,
		uint32_t groupCountX,
		uint32_t groupCountY,
		uint32_t groupCountZ)
	{
		if (commandBuffer == VK_NULL_HANDLE || pipeline == VK_NULL_HANDLE ||
			groupCountX == 0 || groupCountY == 0 || groupCountZ == 0)
			return false;

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);
		vkCmdDispatch(commandBuffer, groupCountX, groupCountY, groupCountZ);
		return true;
	}
}
