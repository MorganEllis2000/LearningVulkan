#pragma once

const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

// Indices (locations) of Queue Families (if they exist at all)
struct QueueFamilyIndices {
	int graphicsFamily = -1;
	int presentationFamily = -1; // Location of presentation queue family

	// Check if queue families are valid
	bool isValid() {
		return graphicsFamily >= 0 && presentationFamily >= 0;
	}
};

struct SwapChainDetails {
	VkSurfaceCapabilitiesKHR surfaceCapabilities;				// Surface properties, e.g. image size/extent
	std::vector<VkSurfaceFormatKHR> formats;					// Surface image formats e.g. RGBA and size of each colour
	std::vector<VkPresentModeKHR> presentationModes;
};

struct SwapchainImage {
	VkImage image;
	VkImageView imageView;
};