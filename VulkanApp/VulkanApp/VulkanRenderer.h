#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <vector>

#include "Utilities.h"

class VulkanRenderer
{
public:
	VulkanRenderer();
	~VulkanRenderer();

	int init(GLFWwindow* newWindow);
	void CleanUp();

protected:

	
private:
	GLFWwindow* window;

	// Vulkan Componenets
	VkInstance instance;
	struct {
		VkPhysicalDevice physicalDevice;
		VkDevice logicalDevice;
	} mainDevice;
	VkQueue graphicsQueue;

	// - Get Functions
	void GetPhysicalDevice();


	// Vulkan Functions
	// - Create Functions
	void CreateInstance();
	void CreateLogicalDevice();

	// - Support Functions
	// -- Checker Functions
	bool CheckInstanceExtensionsSupport(std::vector<const char*>* checkExtensions);
	bool CheckDeviceSuitable(VkPhysicalDevice device);

	// -- Getter Functions
	QueueFamilyIndices getQueueFamilies(VkPhysicalDevice device);


};

