#pragma once

#include <vulkan/vulkan.h>

namespace FrameGenVk
{
	struct Capabilities
	{
		bool computeShader{ false };
		bool graphicsQueueSupportsCompute{ false };
		bool rgba8StorageImage{ false };
		bool rgba16fStorageImage{ false };

		bool IsSupported() const
		{
			// RGBA8 is the baseline path. FP16 remains optional.
			return computeShader && graphicsQueueSupportsCompute && rgba8StorageImage;
		}
	};

	Capabilities QueryCapabilities(VkPhysicalDevice physicalDevice, uint32_t graphicsQueueFamily);
}
