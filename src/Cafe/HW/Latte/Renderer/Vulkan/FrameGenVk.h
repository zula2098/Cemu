#pragma once

#include <vulkan/vulkan.h>
#include <span>

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

	// Creates a compute pipeline from caller-provided SPIR-V. The caller owns the
	// pipeline layout and is responsible for descriptor-set layouts and barriers.
	VkResult CreateComputePipeline(
		VkDevice device,
		std::span<const uint32_t> spirv,
		VkPipelineLayout pipelineLayout,
		VkPipeline* pipeline);

	// Records a basic compute dispatch. Bind descriptor sets and issue the
	// required resource barriers in the caller's command buffer.
	bool RecordDispatch(
		VkCommandBuffer commandBuffer,
		VkPipeline pipeline,
		uint32_t groupCountX,
		uint32_t groupCountY,
		uint32_t groupCountZ = 1);
	// Records the prototype blend shader using its documented descriptor and
	// push-constant contract. The caller must transition images and keep them
	// alive until the command buffer has completed.
	bool RecordBlendDispatch(
		VkCommandBuffer commandBuffer,
		VkPipeline pipeline,
		VkPipelineLayout pipelineLayout,
		VkDescriptorSet descriptorSet,
		uint32_t width,
		uint32_t height,
		float blendFactor);
}
