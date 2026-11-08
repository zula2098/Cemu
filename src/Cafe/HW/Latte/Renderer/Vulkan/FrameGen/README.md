# FrameGen Vulkan prototype

This directory contains the first compute-stage building block for the Android
Vulkan frame-generation experiment.

## Current status

- `FrameGenVk.cpp` detects compute/storage-image support and exposes helpers
  for creating a compute pipeline from SPIR-V and recording dispatch.
- `shaders/blend.comp` is a deliberately simple temporal-blend baseline.
  It produces a midpoint-like image by blending the previous and current
  frames. It is useful for validating descriptors, storage-image writes,
  dispatch dimensions and synchronization.
- This shader is **not optical flow** and will show ghosting on moving objects.
  It must not be presented as production frame interpolation.

## Descriptor and pipeline contract

`FrameGenVk::CreateBlendPipelineLayout()` creates the matching descriptor-set
layout (two combined image samplers and one storage image) and pipeline layout
(with a single 32-bit compute push constant). The caller owns both objects and
must destroy the pipeline layout before its descriptor-set layout.

Set 0:
- binding 0: combined image sampler for the previous frame
- binding 1: combined image sampler for the current frame
- binding 2: storage image, `VK_FORMAT_R8G8B8A8_UNORM`, for the output

Push constants:
- one 32-bit float, `blendFactor` (normally 0.5 for the inserted frame)

Workgroup size: 8 x 8 x 1. Dispatch with
`ceil(width / 8)` by `ceil(height / 8)` groups.

The caller must transition sampled inputs to shader-read layout and the output
to `VK_IMAGE_LAYOUT_GENERAL`, then insert the appropriate compute-write to
subsequent-read/present barrier. `FrameGenVk::RecordBlendDispatch()` binds the
compute pipeline and descriptor set, pushes the 32-bit blend factor, and
calculates dispatch dimensions for the shader's 8x8 workgroups. It does not
perform image transitions or guess Cemu's command-buffer lifecycle.

## Next steps

1. Compile this GLSL to SPIR-V with the glslang toolchain already used by Cemu.
2. Wire descriptors, push constants and image allocation through Cemu's
   Vulkan resource/lifetime systems.
3. Identify the actual final-composition image and safe pre-present insertion
   point.
4. Replace temporal blending with motion estimation and motion-compensated
   interpolation, then benchmark on target hardware.
