#pragma once
#include "volk.h"
#include <vector>

namespace sss
{
	namespace vulkan
	{
		class VKContext;

		class SwapChain
		{
		public:
			explicit SwapChain(const VKContext &context, uint32_t width, uint32_t height);
			SwapChain(SwapChain &) = delete;
			SwapChain(SwapChain &&) = delete;
			SwapChain &operator= (const SwapChain &) = delete;
			SwapChain &operator= (const SwapChain &&) = delete;
			~SwapChain();
			void recreate(uint32_t width, uint32_t height);
			VkExtent2D getExtent() const;
			VkFormat getImageFormat() const;
			operator VkSwapchainKHR() const;
			VkImage getImage(size_t index) const;
			VkImageView getImageView(size_t index) const;
			size_t getImageCount() const;

		private:
			enum {MAX_SWAPCHAIN_IMAGES = 3};

			const VKContext &m_context;
			VkSwapchainKHR m_swapChain;
			VkFormat m_imageFormat;
			VkExtent2D m_extent;
			std::vector<VkImage> m_images;
			std::vector<VkImageView> m_imageViews;

			void create(uint32_t width, uint32_t height);
			void destroy();
		};
	}
}