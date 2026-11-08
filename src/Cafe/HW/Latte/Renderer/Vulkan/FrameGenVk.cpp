#include "Cafe/HW/Latte/Renderer/Vulkan/FrameGenVk.h"

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
}
