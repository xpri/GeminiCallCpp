#include <iostream>

#include "helper.h"
#include "config.cpp"

int main() 
{
	std::cout << "Gemini C++ Client Starting...\n" << std::endl;

	GeminiClient client(GEMINI_API_KEY);

	if (!client.isConfigured()) {
		std::cerr << "Error: Failed to configure Gemini client!" << std::endl;
		return EXIT_FAILURE;
	}

	std::cout << "Gemini Configuration Successful!\n" << std::endl;


	// Demo 1: Simple test
	std::cout << "Testing basic API call..." << std::endl;
	std::string response = client.generateContent("Hello! Please introduce yourself briefly.");
	std::cout << "Response: " << response << std::endl;
	std::cout << "----------------------------------------\n" << std::endl;

	return EXIT_SUCCESS;
}