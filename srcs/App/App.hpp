#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <set>
#include <cstdint> // Necessary for uint32_t
#include <limits> // Necessary for std::numeric_limits
#include <algorithm> // Necessary for std::clamp
#include <glm/glm.hpp>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

struct Vertex {
	glm::vec2 pos;
	glm::vec3 color;

	static VkVertexInputBindingDescription getBindingDescription() {
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 2;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		return attributeDescriptions;
	}
};

const std::vector<Vertex> vertices = {
	{{0.0f, -0.5f}, {0.0f, 0.0f, 0.0f}},
	{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
	{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
};

const std::vector<const char *> validationLayers = {
	"VK_LAYER_KHRONOS_validation",
};

const std::vector<const char *> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

static std::vector<char> readFile(const std::string& filename) {
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("failed to open file!");
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);
	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();
	return buffer;
}


class App
{
	public:
		void run()
		{
			initWindow();
			initVulkan();
			mainLoop();
			cleanup();
		}

	private:
		GLFWwindow *_window;
		VkInstance _instance;
		VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;
		VkDevice _device;
		VkQueue _graphicsQueue;
		VkQueue _presentQueue;
		VkSurfaceKHR _surface;
		VkSwapchainKHR _swapChain;
		std::vector<VkImage> _swapChainImages;
		std::vector<VkImageView> _swapChainImageViews;
		VkFormat _swapChainImageFormat;
		VkExtent2D _swapChainExtent;
		VkRenderPass _renderPass;
		VkPipelineLayout _pipelineLayout;
		VkPipeline _graphicsPipeline;
		std::vector<VkFramebuffer> _swapChainFramebuffers;
		VkCommandPool _commandPool;
		VkCommandBuffer _commandBuffer;
		VkSemaphore _imageAvailableSemaphore;
		VkSemaphore _renderFinishedSemaphore;
		VkFence _inFlightFence;
		std::vector<VkFence> inFlightFences;
		bool framebufferResized = false;
		VkBuffer _vertexBuffer;
		VkDeviceMemory _vertexBufferMemory;




		void initWindow()
		{
			glfwInit();
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
			_window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
			glfwSetWindowUserPointer(_window, this);
			glfwSetFramebufferSizeCallback(_window, framebufferResizeCallback);
		}

		static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
			auto app = reinterpret_cast<App*>(glfwGetWindowUserPointer(window));
			app->framebufferResized = true;
		}


		void createInstance()
		{
			if (enableValidationLayers && !checkValidationLayerSupport())
			{
				throw std::runtime_error("validation layers requested but not available");
			}

			VkApplicationInfo appInfo{};
			appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			appInfo.pApplicationName = "Vulkan";
			appInfo.applicationVersion = VK_MAKE_API_VERSION(1, 0, 0, 0);
			appInfo.pEngineName = "No engine";
			appInfo.engineVersion = VK_MAKE_API_VERSION(1, 0, 0, 0);
			appInfo.apiVersion = VK_API_VERSION_1_0;

			VkInstanceCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			createInfo.pApplicationInfo = &appInfo;

			uint32_t glfwExtensionsCount = 0;
			const char **glfwExtensions;
			glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);
			createInfo.enabledExtensionCount = glfwExtensionsCount;
			createInfo.ppEnabledExtensionNames = glfwExtensions;
			if (enableValidationLayers)
			{
				createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
				createInfo.ppEnabledLayerNames = validationLayers.data();
			}
			else
			{
				createInfo.enabledLayerCount = 0;
			}

			/*   required on macos   */

			std::vector<const char *> requiredExtensions;

			for (uint32_t i = 0; i < glfwExtensionsCount; i++)
			{
				requiredExtensions.emplace_back(glfwExtensions[i]);
			}

			requiredExtensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
			createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
			createInfo.enabledExtensionCount = (u_int32_t)requiredExtensions.size();
			createInfo.ppEnabledExtensionNames = requiredExtensions.data();

			/*************************/

			if (vkCreateInstance(&createInfo, nullptr, &_instance) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create vkInstance");
			}
		}

		bool checkValidationLayerSupport()
		{
			uint32_t layerCount;

			vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
			std::vector<VkLayerProperties> availableLayers(layerCount);
			vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

			for (const char *layerName : validationLayers)
			{
				bool layerFound = false;

				for (const auto &layerProperty : availableLayers)
				{
					if (strcmp(layerName, layerProperty.layerName) == 0)
					{
						layerFound = true;
						break;
					}
				}

				if (!layerFound)
					return (false);
			}
			return (true);
		}

		void pickPhysicalDevice()
		{
			uint32_t deviceCount = 0;

			vkEnumeratePhysicalDevices(_instance, &deviceCount, nullptr);
			if (deviceCount == 0)
			{
				throw std::runtime_error("No GPU found");
			}
			std::vector<VkPhysicalDevice> deviceList(deviceCount);
			vkEnumeratePhysicalDevices(_instance, &deviceCount, deviceList.data());

			for (const auto &device : deviceList)
			{
				if (isDeviceSuitable(device))
				{
					_physicalDevice = device;
					break;
				}
			}

			if (_physicalDevice == VK_NULL_HANDLE)
			{
				throw std::runtime_error("No suitable physical device found");
			}
		}

		struct QueueFamilyIndices
		{
			std::optional<uint32_t> graphicsFamily;
			std::optional<uint32_t> presentFamily;

			bool isComplete()
			{
				return graphicsFamily.has_value() && presentFamily.has_value();
			}
		};

		bool isDeviceSuitable(VkPhysicalDevice device)
		{
			QueueFamilyIndices indices = findQueueFamilies(device);
			bool extensionsSupported = checkDeviceExtensionsSupport(device);
			bool swapChainAdequate = false;

			if (extensionsSupported)
			{
				SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
				swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
			}
			return indices.isComplete() && extensionsSupported && swapChainAdequate;
		}

		bool checkDeviceExtensionsSupport(VkPhysicalDevice device)
		{
			uint32_t extensionsCount;

			vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionsCount, nullptr);
			std::vector<VkExtensionProperties> extensions(extensionsCount);
			vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionsCount, extensions.data());

			std::set<std::string> requiredExtenions(deviceExtensions.begin(), deviceExtensions.end());
			for (const auto &extension : extensions)
			{
				requiredExtenions.erase(extension.extensionName);
			}

			return requiredExtenions.empty();
		}

		QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device)
		{
			QueueFamilyIndices indices;

			uint32_t queueFamilyCount = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

			std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

			int i = 0;
			for (const auto &queueFamily : queueFamilies)
			{
				VkBool32 presentSupport = false;
				vkGetPhysicalDeviceSurfaceSupportKHR(device, i, _surface, &presentSupport);
				if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
					indices.graphicsFamily = i;
				if (presentSupport)
					indices.presentFamily = i;

				if (indices.isComplete())
					break;
				i++;
			}

			return indices;
		}

		void createLogicalDevice()
		{
			QueueFamilyIndices indices = findQueueFamilies(_physicalDevice);

			std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
			std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

			float queuePriority = 1.0f;
			for (uint32_t queueFamily : uniqueQueueFamilies)
			{
				VkDeviceQueueCreateInfo queueCreateInfo{};
				queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				queueCreateInfo.queueFamilyIndex = queueFamily;
				queueCreateInfo.queueCount = 1;
				queueCreateInfo.pQueuePriorities = &queuePriority;
				queueCreateInfos.push_back(queueCreateInfo);
			}

			VkPhysicalDeviceFeatures deviceFeature = {};

			VkDeviceCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			createInfo.pQueueCreateInfos = queueCreateInfos.data();
			createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
			createInfo.pEnabledFeatures = &deviceFeature;
			createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
			createInfo.ppEnabledExtensionNames = deviceExtensions.data();

			if (enableValidationLayers)
			{
				createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
				createInfo.ppEnabledLayerNames = validationLayers.data();
			}
			else
			{
				createInfo.enabledLayerCount = 0;
			}

			if (vkCreateDevice(_physicalDevice, &createInfo, nullptr, &_device) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create logical device");
			}

			vkGetDeviceQueue(_device, indices.graphicsFamily.value(), 0, &_graphicsQueue);
			vkGetDeviceQueue(_device, indices.presentFamily.value(), 0, &_presentQueue);
		}

		void createSurface()
		{
			if (glfwCreateWindowSurface(_instance, _window, nullptr, &_surface) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create window surface");
			}
		}

		struct SwapChainSupportDetails
		{
			VkSurfaceCapabilitiesKHR capabilities;
			std::vector<VkSurfaceFormatKHR> formats;
			std::vector<VkPresentModeKHR> presentModes;
		};

		VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats)
		{
			for (const auto &availableFormat : availableFormats)
			{
				if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				{
					return availableFormat;
				}
			}
			return availableFormats[0];
		}

		VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availableModes)
		{
			for (const auto &availableMode : availableModes)
			{
				if (availableMode == VK_PRESENT_MODE_MAILBOX_KHR)
				{
					return availableMode;
				}
			}
			return VK_PRESENT_MODE_FIFO_KHR;
		}

		VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
			if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
			{
				return capabilities.currentExtent;
			}
			else
			{
				int width, height;
				glfwGetFramebufferSize(_window, &width, &height);

				VkExtent2D actualExtent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
				actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
				actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
				return actualExtent;
			}
		}

		SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device)
		{ 
			SwapChainSupportDetails details;

			vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, _surface, &details.capabilities);

			uint32_t formatCount;
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, _surface, &formatCount, nullptr);
			if (formatCount != 0)
			{
				details.formats.resize(formatCount);
				vkGetPhysicalDeviceSurfaceFormatsKHR(device, _surface, &formatCount, details.formats.data());
			}
			uint32_t presentModeCount;
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, _surface, &presentModeCount, nullptr);
			if (presentModeCount != 0)
			{
				details.presentModes.resize(presentModeCount);
				vkGetPhysicalDeviceSurfacePresentModesKHR(device, _surface, &presentModeCount, details.presentModes.data());
			}

			return details;
		}

		void createSwapChain()
		{
			SwapChainSupportDetails swapChainSupport =  querySwapChainSupport(_physicalDevice);

			VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
			VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
			VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

			uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
			if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
			{
				imageCount = swapChainSupport.capabilities.maxImageCount;
			}

			VkSwapchainCreateInfoKHR createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			createInfo.surface = _surface;
			createInfo.imageFormat = surfaceFormat.format;
			createInfo.minImageCount = imageCount;
			createInfo.imageColorSpace = surfaceFormat.colorSpace;
			createInfo.imageExtent = extent;
			createInfo.imageArrayLayers = 1;
			createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

			QueueFamilyIndices indices = findQueueFamilies(_physicalDevice);
			uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

			if (indices.graphicsFamily != indices.presentFamily)
			{
				createInfo.imageSharingMode =  VK_SHARING_MODE_CONCURRENT;
				createInfo.queueFamilyIndexCount = 2;
				createInfo.pQueueFamilyIndices = queueFamilyIndices;
			}
			else
			{
				createInfo.imageSharingMode =  VK_SHARING_MODE_EXCLUSIVE;
				createInfo.queueFamilyIndexCount = 0;
				createInfo.pQueueFamilyIndices = nullptr;
			}
			createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
			createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
			createInfo.presentMode =  presentMode;
			createInfo.clipped = VK_TRUE;
			createInfo.oldSwapchain = VK_NULL_HANDLE;

			if (vkCreateSwapchainKHR(_device, &createInfo, nullptr, &_swapChain) != VK_SUCCESS)
			{
				throw std::runtime_error("Can't create swapchain");
			}

			vkGetSwapchainImagesKHR(_device, _swapChain, &imageCount, nullptr);
			_swapChainImages.resize(imageCount);
			vkGetSwapchainImagesKHR(_device, _swapChain, &imageCount, _swapChainImages.data());
			_swapChainImageFormat = surfaceFormat.format;
			_swapChainExtent = extent;

		}

		void createImageView(void)
		{
			_swapChainImageViews.resize(_swapChainImages.size());

			for (size_t i = 0; i < _swapChainImages.size(); i++)
			{
				VkImageViewCreateInfo createInfo {};
				createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
				createInfo.image = _swapChainImages[i];
				createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
				createInfo.format = _swapChainImageFormat;
				createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
				createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
				createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
				createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
				createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				createInfo.subresourceRange.baseMipLevel = 0;
				createInfo.subresourceRange.levelCount = 1;
				createInfo.subresourceRange.baseArrayLayer = 0;
				createInfo.subresourceRange.layerCount = 1;

				if (vkCreateImageView(_device, &createInfo, nullptr, &_swapChainImageViews[i]) != VK_SUCCESS)
				{
					throw std::runtime_error("Can't create image view");
				}
			}
		}

		VkShaderModule createShaderModule(const std::vector<char>& code)
		{
			VkShaderModuleCreateInfo createInfo {};
			createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			createInfo.codeSize = code.size();
			createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

			VkShaderModule shaderModule;
			if (vkCreateShaderModule(_device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
				throw std::runtime_error("Error while loading shader module");
			}

			return shaderModule;
		}

		void createGraphicsPipeline()
		{
			auto vertShaderCode = readFile("/Users/hugo/Desktop/Workspace/42/scop/srcs/shaders/vert.spv");
			auto fragShaderCode = readFile("/Users/hugo/Desktop/Workspace/42/scop/srcs/shaders/frag.spv");

			VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
			VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

			VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
			vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
			vertShaderStageInfo.module = vertShaderModule;
			vertShaderStageInfo.pName = "main";

			VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
			fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
			fragShaderStageInfo.module = fragShaderModule;
			fragShaderStageInfo.pName = "main";

			VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

			VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
			vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			vertexInputInfo.vertexBindingDescriptionCount = 0;
			vertexInputInfo.pVertexBindingDescriptions = nullptr; // Optional
			vertexInputInfo.vertexAttributeDescriptionCount = 0;
			vertexInputInfo.pVertexAttributeDescriptions = nullptr; // Optional

			auto bindingDescription = Vertex::getBindingDescription();
			auto attributeDescriptions = Vertex::getAttributeDescriptions();

			vertexInputInfo.vertexBindingDescriptionCount = 1;
			vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
			vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
			vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

			VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
			inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			inputAssembly.primitiveRestartEnable = VK_FALSE;

			VkViewport viewport{};
			viewport.x = 0.0f;
			viewport.y = 0.0f;
			viewport.width = (float) _swapChainExtent.width;
			viewport.height = (float) _swapChainExtent.height;
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;

			VkRect2D scissor{};
			scissor.offset = {0, 0};
			scissor.extent = _swapChainExtent;

			VkPipelineRasterizationStateCreateInfo rasterizer{};
			rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			rasterizer.depthClampEnable = VK_FALSE;
			rasterizer.rasterizerDiscardEnable = VK_FALSE;
			rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
			rasterizer.lineWidth = 1.0f;
			rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
			rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
			rasterizer.depthBiasEnable = VK_FALSE;
			rasterizer.depthBiasConstantFactor = 0.0f; // Optional
			rasterizer.depthBiasClamp = 0.0f; // Optional
			rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

			VkPipelineMultisampleStateCreateInfo multisampling{};
			multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
			multisampling.sampleShadingEnable = VK_FALSE;
			multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
			multisampling.minSampleShading = 1.0f; // Optional
			multisampling.pSampleMask = nullptr; // Optional
			multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
			multisampling.alphaToOneEnable = VK_FALSE; // Optional

			VkPipelineColorBlendAttachmentState colorBlendAttachment{};
			colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
			colorBlendAttachment.blendEnable = VK_FALSE;
			colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
			colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
			colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
			colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
			colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
			colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

			VkPipelineColorBlendStateCreateInfo colorBlending{};
			colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
			colorBlending.logicOpEnable = VK_FALSE;
			colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
			colorBlending.attachmentCount = 1;
			colorBlending.pAttachments = &colorBlendAttachment;
			colorBlending.blendConstants[0] = 0.0f; // Optional
			colorBlending.blendConstants[1] = 0.0f; // Optional
			colorBlending.blendConstants[2] = 0.0f; // Optional
			colorBlending.blendConstants[3] = 0.0f; // Optional

			VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
			pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			pipelineLayoutInfo.setLayoutCount = 0; // Optional
			pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
			pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
			pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

			if (vkCreatePipelineLayout(_device, &pipelineLayoutInfo, nullptr, &_pipelineLayout) != VK_SUCCESS) {
				throw std::runtime_error("failed to create pipeline layout!");
			}

			std::vector<VkDynamicState> dynamicStates = {
				VK_DYNAMIC_STATE_VIEWPORT,
				VK_DYNAMIC_STATE_SCISSOR
			};

			VkPipelineDynamicStateCreateInfo dynamicState{};
			dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
			dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
			dynamicState.pDynamicStates = dynamicStates.data();
			VkPipelineViewportStateCreateInfo viewportState{};
			viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			viewportState.viewportCount = 1;
			viewportState.scissorCount = 1;

			VkGraphicsPipelineCreateInfo pipelineInfo{};
			pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			pipelineInfo.stageCount = 2;
			pipelineInfo.pStages = shaderStages;
			pipelineInfo.pVertexInputState = &vertexInputInfo;
			pipelineInfo.pInputAssemblyState = &inputAssembly;
			pipelineInfo.pViewportState = &viewportState;
			pipelineInfo.pRasterizationState = &rasterizer;
			pipelineInfo.pMultisampleState = &multisampling;
			pipelineInfo.pDepthStencilState = nullptr; // Optional
			pipelineInfo.pColorBlendState = &colorBlending;
			pipelineInfo.pDynamicState = &dynamicState;
			pipelineInfo.layout = _pipelineLayout;
			pipelineInfo.renderPass = _renderPass;
			pipelineInfo.subpass = 0;

			pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
			pipelineInfo.basePipelineIndex = -1; // Optional

			if (vkCreateGraphicsPipelines(_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &_graphicsPipeline) != VK_SUCCESS) {
				throw std::runtime_error("failed to create graphics pipeline!");
			}

			vkDestroyShaderModule(_device, vertShaderModule, nullptr);
			vkDestroyShaderModule(_device, fragShaderModule, nullptr);
		}

		void createRenderPass()
		{
			VkAttachmentDescription colorAttachment{};
			colorAttachment.format = _swapChainImageFormat;
			colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
			colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

			VkAttachmentReference colorAttachmentRef{};
			colorAttachmentRef.attachment = 0;
			colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			VkSubpassDescription subpass{};
			subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			subpass.colorAttachmentCount = 1;
			subpass.pColorAttachments = &colorAttachmentRef;

			VkSubpassDependency dependency{};
			dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
			dependency.dstSubpass = 0;
			dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependency.srcAccessMask = 0;
			dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

			VkRenderPassCreateInfo renderPassInfo{};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			renderPassInfo.attachmentCount = 1;
			renderPassInfo.pAttachments = &colorAttachment;
			renderPassInfo.subpassCount = 1;
			renderPassInfo.pSubpasses = &subpass;
			renderPassInfo.dependencyCount = 1;
			renderPassInfo.pDependencies = &dependency;

			if (vkCreateRenderPass(_device, &renderPassInfo, nullptr, &_renderPass) != VK_SUCCESS) {
				throw std::runtime_error("failed to create render pass!");
			}
		}

		void createFramebuffers()
		{
			_swapChainFramebuffers.resize(_swapChainImageViews.size());
			for (size_t i = 0; i < _swapChainImageViews.size(); i++) {
				VkImageView attachments[] = {
					_swapChainImageViews[i]
				};

				VkFramebufferCreateInfo framebufferInfo{};
				framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
				framebufferInfo.renderPass = _renderPass;
				framebufferInfo.attachmentCount = 1;
				framebufferInfo.pAttachments = attachments;
				framebufferInfo.width = _swapChainExtent.width;
				framebufferInfo.height = _swapChainExtent.height;
				framebufferInfo.layers = 1;

				if (vkCreateFramebuffer(_device, &framebufferInfo, nullptr, &_swapChainFramebuffers[i]) != VK_SUCCESS)
				{
					throw std::runtime_error("failed to create framebuffer!");
				}
			}
		}

		void createCommandPool()
		{
			QueueFamilyIndices queueFamilyIndices = findQueueFamilies(_physicalDevice);

			VkCommandPoolCreateInfo poolInfo{};
			poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
			if (vkCreateCommandPool(_device, &poolInfo, nullptr, &_commandPool) != VK_SUCCESS) {
				throw std::runtime_error("failed to create command pool!");
			}
		}

		void createCommandBuffer()
		{
			VkCommandBufferAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocInfo.commandPool = _commandPool;
			allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocInfo.commandBufferCount = 1;

			if (vkAllocateCommandBuffers(_device, &allocInfo, &_commandBuffer) != VK_SUCCESS) {
				throw std::runtime_error("failed to allocate command buffers!");
			}
		}

		void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = 0; // Optional
			beginInfo.pInheritanceInfo = nullptr; // Optional

			if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
				throw std::runtime_error("failed to begin recording command buffer!");
			}

			VkRenderPassBeginInfo renderPassInfo{};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = _renderPass;
			renderPassInfo.framebuffer = _swapChainFramebuffers[imageIndex];
			renderPassInfo.renderArea.offset = {0, 0};
			renderPassInfo.renderArea.extent = _swapChainExtent;
			VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
			renderPassInfo.clearValueCount = 1;
			renderPassInfo.pClearValues = &clearColor;
			vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _graphicsPipeline);
			VkViewport viewport{};
			viewport.x = 0.0f;
			viewport.y = 0.0f;
			viewport.width = static_cast<float>(_swapChainExtent.width);
			viewport.height = static_cast<float>(_swapChainExtent.height);
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;
			vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

			VkRect2D scissor{};
			scissor.offset = {0, 0};
			scissor.extent = _swapChainExtent;
			vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

			VkBuffer vertexBuffers[] = {_vertexBuffer};
			VkDeviceSize offsets[] = {0};
			vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

			vkCmdDraw(commandBuffer, static_cast<uint32_t>(vertices.size()), 1, 0, 0);
			vkCmdEndRenderPass(commandBuffer);
			if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
				throw std::runtime_error("failed to record command buffer!");
			}
		}

		void createSyncObjects() {
			VkSemaphoreCreateInfo semaphoreInfo{};
			semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			VkFenceCreateInfo fenceInfo{};
			fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

			if (vkCreateSemaphore(_device, &semaphoreInfo, nullptr, &_imageAvailableSemaphore) != VK_SUCCESS
					|| vkCreateSemaphore(_device, &semaphoreInfo, nullptr, &_renderFinishedSemaphore) != VK_SUCCESS
					|| vkCreateFence(_device, &fenceInfo, nullptr, &_inFlightFence) != VK_SUCCESS) {
				throw std::runtime_error("failed to create semaphores!");
			}
		}

		void cleanupSwapChain()
		{
			for (auto framebuffer : _swapChainFramebuffers)
			{
				vkDestroyFramebuffer(_device, framebuffer, nullptr);
			}
			for (auto imageView : _swapChainImageViews)
			{
				vkDestroyImageView(_device, imageView, nullptr);
			}
			vkDestroySwapchainKHR(_device, _swapChain, nullptr);
		}

		void recreateSwapChain()
		{
			int width = 0, height = 0;
			glfwGetFramebufferSize(_window, &width, &height);
			while (width == 0 || height == 0) {
				glfwGetFramebufferSize(_window, &width, &height);
				glfwWaitEvents();
			}
			vkDeviceWaitIdle(_device);
			cleanupSwapChain();
			createSwapChain();
			createImageView();
			createFramebuffers();
		}

		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
			VkPhysicalDeviceMemoryProperties memProperties;
			vkGetPhysicalDeviceMemoryProperties(_physicalDevice, &memProperties);

			for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
				if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
					return i;
				}
			}

			throw std::runtime_error("failed to find suitable memory type!");
		}

		void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {	
			VkBufferCreateInfo bufferInfo{};
			bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferInfo.size = size;
			bufferInfo.usage = usage;
			bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			if (vkCreateBuffer(_device, &bufferInfo, nullptr, &_vertexBuffer) != VK_SUCCESS) {
				throw std::runtime_error("failed to create vertex buffer!");
			}

			VkMemoryRequirements memRequirements;
			vkGetBufferMemoryRequirements(_device, _vertexBuffer, &memRequirements);

			VkMemoryAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocInfo.allocationSize = memRequirements.size;
			allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties); 

			if (vkAllocateMemory(_device, &allocInfo, nullptr, &_vertexBufferMemory) != VK_SUCCESS) {
				throw std::runtime_error("failed to allocate vertex buffer memory!");
			}

			vkBindBufferMemory(_device, _vertexBuffer, _vertexBufferMemory, 0);
		}

		void createVertexBuffer() {
			VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
			VkBuffer stagingBuffer;
			VkDeviceMemory stagingBufferMemory;
			createBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

			void* data;
			vkMapMemory(_device, stagingBufferMemory, 0, bufferSize, 0, &data);
			memcpy(data, vertices.data(), (size_t) bufferSize);
			vkUnmapMemory(_device, _vertexBufferMemory);
		}

		void	copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
			VkCommandBufferAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocInfo.commandPool = _commandPool;
			allocInfo.commandBufferCount = 1;

			VkCommandBuffer commandBuffer {};
			vkAllocateCommandBuffers(_device, &allocInfo, &commandBuffer);

			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

			vkBeginCommandBuffer(commandBuffer, &beginInfo);

			VkBufferCopy copyRegion{};
			copyRegion.srcOffset = 0;
			copyRegion.dstOffset = 0;
			copyRegion.size = size;
			vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
			vkEndCommandBuffer(commandBuffer);
		}	


		void initVulkan()
		{
			createInstance();
			createSurface();
			pickPhysicalDevice();
			createLogicalDevice();
			createSwapChain();
			createImageView();
			createRenderPass();
			createGraphicsPipeline();
			createFramebuffers();
			createCommandPool();
			createVertexBuffer();
			createCommandBuffer();
			createSyncObjects();
		}

		void drawFrame()
		{
			vkWaitForFences(_device, 1, &_inFlightFence, VK_TRUE, UINT64_MAX);
			vkResetFences(_device, 1, &_inFlightFence);
			uint32_t imageIndex;
			VkResult res = vkAcquireNextImageKHR(_device, _swapChain, UINT64_MAX, _imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
			if (res == VK_ERROR_OUT_OF_DATE_KHR) {
				recreateSwapChain();
				return ;
			} else if (res != VK_SUCCESS && res != VK_SUBOPTIMAL_KHR) {
				throw std::runtime_error("failed to acquire swapchain image");
			}
			vkResetCommandBuffer(_commandBuffer, 0);
			recordCommandBuffer(_commandBuffer, imageIndex);

			VkSubmitInfo submitInfo{};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			VkSemaphore waitSemaphores[] = {_imageAvailableSemaphore};
			VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitSemaphores = waitSemaphores;
			submitInfo.pWaitDstStageMask = waitStages;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &_commandBuffer;
			VkSemaphore signalSemaphores[] = {_renderFinishedSemaphore};
			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = signalSemaphores;

			if (vkQueueSubmit(_graphicsQueue, 1, &submitInfo, _inFlightFence) != VK_SUCCESS) {
				throw std::runtime_error("failed to submit draw command buffer!");
			}

			VkPresentInfoKHR presentInfo{};
			presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

			presentInfo.waitSemaphoreCount = 1;
			presentInfo.pWaitSemaphores = signalSemaphores;
			VkSwapchainKHR swapChains[] = {_swapChain};
			presentInfo.swapchainCount = 1;
			presentInfo.pSwapchains = swapChains;
			presentInfo.pImageIndices = &imageIndex;
			presentInfo.pResults = nullptr; // Optional
			res = vkQueuePresentKHR(_presentQueue, &presentInfo);
			if (res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR || framebufferResized) {
				framebufferResized = false;
				recreateSwapChain();
			} else if (res != VK_SUCCESS) {
				throw std::runtime_error("failed to present swap chain image!");
			}
		}

		void mainLoop()
		{
			while (!glfwWindowShouldClose(_window))
			{
				glfwPollEvents();
				drawFrame();
			}
			vkDeviceWaitIdle(_device);
		}

		void cleanup()
		{
			cleanupSwapChain();
			vkDestroyBuffer(_device, _vertexBuffer, nullptr);
			vkFreeMemory(_device, _vertexBufferMemory, nullptr);
			vkDestroyBuffer(_device, _vertexBuffer, nullptr);
			vkDestroyPipeline(_device, _graphicsPipeline, nullptr);
			vkDestroyPipelineLayout(_device, _pipelineLayout, nullptr);
			vkDestroyRenderPass(_device, _renderPass, nullptr);
			vkDestroySemaphore(_device, _imageAvailableSemaphore, nullptr);
			vkDestroySemaphore(_device, _renderFinishedSemaphore, nullptr);
			vkDestroyFence(_device, _inFlightFence, nullptr);
			vkDestroyCommandPool(_device, _commandPool, nullptr);
			vkDestroyDevice(_device, nullptr);
			vkDestroySurfaceKHR(_instance, _surface, nullptr);
			vkDestroyInstance(_instance, nullptr);
			glfwDestroyWindow(_window);
			glfwTerminate();
		}
};