#include "VKUtility.h"

VkResult sss::vulkan::vkutil::findMemoryTypeIndex(VkPhysicalDeviceMemoryProperties memoryProperties, uint32_t memoryTypeBitsRequirement, VkMemoryPropertyFlags requiredProperties, VkMemoryPropertyFlags preferredProperties, uint32_t & memoryTypeIndex)
{
	memoryTypeIndex = ~uint32_t(0);
	int bestResultBitCount = -1;

	for (uint32_t memoryIndex = 0; memoryIndex < memoryProperties.memoryTypeCount; ++memoryIndex)
	{
		const bool isRequiredMemoryType = memoryTypeBitsRequirement & (1 << memoryIndex);

		const VkMemoryPropertyFlags properties = memoryProperties.memoryTypes[memoryIndex].propertyFlags;
		const bool hasRequiredProperties = (properties & requiredProperties) == requiredProperties;

		if (isRequiredMemoryType && hasRequiredProperties)
		{
			uint32_t presentBits = properties & preferredProperties;

			// all preferred bits are present
			if (presentBits == preferredProperties)
			{
				memoryTypeIndex = memoryIndex;
				return VK_SUCCESS;
			}

			// only some bits are present -> count them
			int bitCount = 0;
			for (uint32_t bit = 0; bit < 32; ++bit)
			{
				bitCount += (presentBits & (1 << bit)) >> bit;
			}

			// save memory type with highest bit count of present preferred flags
			if (bitCount > bestResultBitCount)
			{
				bestResultBitCount = bitCount;
				memoryTypeIndex = memoryIndex;
			}
		}
	}

	return memoryTypeIndex != ~uint32_t(0) ? VK_SUCCESS : VK_ERROR_FEATURE_NOT_PRESENT;
}

VkResult sss::vulkan::vkutil::createImage(VkPhysicalDevice physicalDevice, VkDevice device, VkImageCreateInfo createInfo, VkMemoryPropertyFlags requiredFlags, VkMemoryPropertyFlags preferredFlags, VkImage &image, VkDeviceMemory &memory)
{
	VkResult result = vkCreateImage(device, &createInfo, nullptr, &image);

	if (result != VK_SUCCESS)
	{
		return result;
	}

	VkMemoryRequirements memoryRequirements;
	vkGetImageMemoryRequirements(device, image, &memoryRequirements);

	VkPhysicalDeviceMemoryProperties memoryProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

	uint32_t memoryTypeIndex;
	if (findMemoryTypeIndex(memoryProperties, memoryRequirements.memoryTypeBits, requiredFlags, preferredFlags, memoryTypeIndex) != VK_SUCCESS)
	{
		return VK_ERROR_FEATURE_NOT_PRESENT;
	}

	VkMemoryAllocateInfo memoryAllocateInfo{ VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
	memoryAllocateInfo.allocationSize = memoryRequirements.size;
	memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;

	result = vkAllocateMemory(device, &memoryAllocateInfo, nullptr, &memory);

	if (result != VK_SUCCESS)
	{
		vkDestroyImage(device, image, nullptr);
		return result;
	}

	result = vkBindImageMemory(device, image, memory, 0);

	if (result != VK_SUCCESS)
	{
		vkDestroyImage(device, image, nullptr);
		vkFreeMemory(device, memory, nullptr);
		return result;
	}

	return VK_SUCCESS;
}

VkResult sss::vulkan::vkutil::create2dImage(VkPhysicalDevice physicalDevice, VkDevice device, VkFormat format, uint32_t width, uint32_t height, VkImageUsageFlags usage, VkMemoryPropertyFlags requiredFlags, VkMemoryPropertyFlags preferredFlags, VkImage & image, VkDeviceMemory & memory)
{
	VkImageCreateInfo imageCreateInfo{ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
	imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	imageCreateInfo.format = format;
	imageCreateInfo.extent.width = width;
	imageCreateInfo.extent.height = height;
	imageCreateInfo.extent.depth = 1;
	imageCreateInfo.mipLevels = 1;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageCreateInfo.usage = usage;
	imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	return createImage(physicalDevice, device, imageCreateInfo, requiredFlags, preferredFlags, image, memory);
}