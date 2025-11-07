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
	//std::cout << "Testing basic API call..." << std::endl;
	//std::map<std::string, std::string> params;
	//params["temperature"] = "0.8";
	//params["maxOutputTokens"] = "500";
	//std::string response = client.generateContent("Hello! Please introduce yourself briefly.");
	//std::cout << "Response: " << response << std::endl;
	//std::cout << "----------------------------------------\n" << std::endl;
	//demo1(client);

	// Doing where one can input their own prompt.
	//std::cout << "Hello! Please input your prompt for the API call:" << std::endl;
	//std::string prompt;
	//std::cin >> prompt;
	
	//while (exit != 1)
	//{
	//	// Place code here.
	//}
	
	std::string userMenuResponse;
	while (userMenuResponse != "0")
	{
		userMenuResponse = displayMenu();
		//std::getline(std::cin, userMenuResponse);
		if (userMenuResponse == "0")
		{
			std::cout << "program terminated." << std::endl;
			break;
		}
		else if (userMenuResponse == "1")
		{
			demo1(client);
		}
		else if (userMenuResponse == "2")
		{
			demo2(client);
		}
		else if (userMenuResponse == "9")
		{
			getPromptAndPushAPI(client);
		}
		else
		{
			std::cout << "That is an incorrect response. Please try again." << std::endl;
		}
	}

	//std::cout << "Do you read me?" << std::endl;

	//displayMenu();
	//getPromptAndPushAPI(client);
	return EXIT_SUCCESS;
}