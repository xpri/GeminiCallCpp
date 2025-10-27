#ifndef HELPER_H
#define HELPER_H

#include <string>
#include <map>

class GeminiClient
{
public:
    // Constructor - takes API key
    GeminiClient(const std::string& api_key);

    // Check if API key is configured
    bool isConfigured() const;

    // Main function to generate content from Gemini
    std::string generateContent(const std::string& prompt);

    // Advanced version with parameters
    std::string generateContent(const std::string& prompt,
        const std::map<std::string, std::string>& parameters);

private:
    std::string api_key_;
    std::string base_url_ = "https://generativelanguage.googleapis.com/v1beta/models/";

    // Makes the actual HTTP request
    std::string makeHttpRequest(const std::string& url, const std::string& post_data);

    // Parses the JSON response from Gemini
    std::string parseAPIResponse(const std::string& response);

    // Callback function for CURL to write response data
    static size_t curlWriteCallback(void* contents, size_t size, size_t nmemb, std::string* response);
};

// Utility function to get API key from environment or user
//std::string getGeminiApiKey();

void getPromptAndPushAPI(GeminiClient& client);

void demo1(GeminiClient& client);

std::string displayMenu();

#endif