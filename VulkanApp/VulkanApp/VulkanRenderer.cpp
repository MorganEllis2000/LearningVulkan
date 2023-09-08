#include "VulkanRenderer.h"

VulkanRenderer::VulkanRenderer()
{
}

VulkanRenderer::~VulkanRenderer()
{
}

int VulkanRenderer::init(GLFWwindow* newWindow)
{
	window = newWindow;

	try {
		CreateInstance();
		GetPhysicalDevice();
		CreateLogicalDevice();
	}
	catch (const std::runtime_error& e) {
		printf("ERROR: %s\n", e.what());
		return EXIT_FAILURE;
	}

	return 0;
}

void VulkanRenderer::CleanUp()
{
	vkDestroyDevice(mainDevice.logicalDevice, nullptr);
	vkDestroyInstance(instance, nullptr);
}



void VulkanRenderer::GetPhysicalDevice()
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

	// If no devices available, then none support vulkan!
	if (deviceCount == 0) {
		throw std::runtime_error("Can't find GPUs that support");
	}

	// Get list of Physical Devices
	std::vector<VkPhysicalDevice> deviceList(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, deviceList.data());

	for (const auto& device : deviceList) {
		if (CheckDeviceSuitable(device)) {
			mainDevice.physicalDevice = device;
			break;
		}
	}

}

void VulkanRenderer::CreateInstance()
{
	// Information about the application itself
	// Most data here doesn't affect the program
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Vulkan App";										// Custom name of the app
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);							// Custom version of the app
	appInfo.pEngineName = "No Engine";												// Custom engine name
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);								// Custom engine version
	appInfo.apiVersion = VK_API_VERSION_1_3;										// The vulkan version <--- this one does

	// Creation informations for a VKInstance
	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	// Create list to hold instance extensions
	std::vector<const char*> instanceExtensions = std::vector<const char*>();

	uint32_t glfwExtensionCount = 0;												// GLFW may require multiple extensions
	const char** glfwExtensions;													// Extensions passed as arrat of cstrings, so need pointer (the array) to the pointer
	
	// Get glfw extensions
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	// Add glfw extensions to list of extensions
	for (size_t i = 0; i < glfwExtensionCount; i++) {
		instanceExtensions.push_back(glfwExtensions[i]);
	}

	// Check instance extenions supported
	//if (CheckInstanceExtensionsSupport(&instanceExtensions)) {
	//	throw std::runtime_error("VkInstance does not support required extensions");
	//}

	createInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
	createInfo.ppEnabledExtensionNames = instanceExtensions.data();

	// TODO: Set up validation layers that instance will use
	createInfo.enabledLayerCount = 0;
	createInfo.ppEnabledLayerNames = nullptr;

	// Create instance
	VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);

	if (result != VK_SUCCESS) {
		throw std::runtime_error("Failed to create a Vulkan Instance");
	}
}

void VulkanRenderer::CreateLogicalDevice()
{
	QueueFamilyIndices indices = getQueueFamilies(mainDevice.physicalDevice);

	// Queue the lgocial device needs to create and info to do so (only 1 for now, will add more later)
	VkDeviceQueueCreateInfo queueCreateInfo = {};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;					
	queueCreateInfo.queueFamilyIndex = indices.graphicsFamily;							// The index of the family to create a queue from
	queueCreateInfo.queueCount = 1;														// Number of queues to create
	float priority = 1.0f;
	queueCreateInfo.pQueuePriorities = &priority;										// Vulkan needs to know how to handle multiple queues, so decide priority (1 = highest priority)

	// Information to create logical device (sometimes called 'device')
	VkDeviceCreateInfo deviceCreateInfo = {};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.queueCreateInfoCount = 1;											// Number of queue create infos
	deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;								// List of queue create infos so device can create required
	deviceCreateInfo.enabledExtensionCount = 0;											// Number of enabled logical device extensions
	deviceCreateInfo.ppEnabledExtensionNames = nullptr;									// List of enabled logical device extensions
	

	VkPhysicalDeviceFeatures deviceFeatures = {};


	deviceCreateInfo.pEnabledFeatures = &deviceFeatures;


	// Create the logical device for the given physical device
	VkResult result = vkCreateDevice(mainDevice.physicalDevice, &deviceCreateInfo, nullptr, &mainDevice.logicalDevice);

	if (result != VK_SUCCESS) {
		throw std::runtime_error("Failed to create a Logical Device!");
	}

	// Queues are created at the same time as the device...
	// So we want handle to queues
	// From given logical device of given queue family, of given queue index (0 since only one queue), place reference in given VkQueue 
	vkGetDeviceQueue(mainDevice.logicalDevice, indices.graphicsFamily, 0, &graphicsQueue);
}

bool VulkanRenderer::CheckInstanceExtensionsSupport(std::vector<const char*>* checkExtensions)
{
	// Need to get number of extensions to create array of correct size to hold extensions
	uint32_t extensionsCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, nullptr);

	// Create a list of vKExtensionsProperties using count
	std::vector<VkExtensionProperties> extensions(extensionsCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, extensions.data());

	// Check if given extensions are in list of available extensions
	for (const auto& checkExtension : *checkExtensions) {
		bool hasExtension = false;
		for (const auto& extension : extensions) {
			if (!strcmp(checkExtension, extension.extensionName)) {
				hasExtension = true;
				break;
			}
			else {
				printf("Extension name: %s\n", extension.extensionName);
			}
		}
		if (!hasExtension) {
			
			return false;
		}
	}

	return true;
}

bool VulkanRenderer::CheckDeviceSuitable(VkPhysicalDevice device)
{
	//// Information about the device itself (ID, name, type, vender, etc)
	//VkPhysicalDeviceProperties deviceProperties;
	//vkGetPhysicalDeviceProperties(device, &deviceProperties);

	//// Information about what the device can do (geo shader, tess shdaer, wide lines, etc)
	//VkPhysicalDeviceFeatures deviceFeatures;
	//vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	QueueFamilyIndices indicies = getQueueFamilies(device);

	return indicies.isValid();
}

QueueFamilyIndices VulkanRenderer::getQueueFamilies(VkPhysicalDevice device)
{
	QueueFamilyIndices indices;

	// Get all queue family property info for the given device
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilyList(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilyList.data());

	// Go through each queue family and check if it has at least 1 of the required types of queue
	int i = 0;
	for (const auto& queueFamily : queueFamilyList) {
		// First check if queue family has at least 1 queue in that faimly (could have no queues)
		// Queue can be miltiple types defined through bitfield. need to bitwise AND with VK_QUEUE_*_BIT to check if has required
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i; // If queue family is valid, then get index
		}

		// Check if queue family indices are in a valid state, stop searching if so
		if (indices.isValid()) {
			break;
		}

		i++;
	}

	return indices;
}