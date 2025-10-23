#include "helper.h"
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include <sstream>

using json = nlohmann::json;

// Callback function for writing data
size_t GeminiClient::curlWriteCallback(void* contents, size_t size, size_t nmemb, std::string* response) {
    size_t total_size = size * nmemb;
    response->append((char*)contents, total_size);
    return total_size;
}

GeminiClient::GeminiClient(const std::string& api_key) : api_key_(api_key) {}

bool GeminiClient::isConfigured() const
{
    return !api_key_.empty();
}

std::string GeminiClient::generateContent(const std::string& prompt) {
    return generateContent(prompt, {});
}

std::string GeminiClient::generateContent(const std::string& prompt, const std::map<std::string, std::string>& parameters) {
    if (!isConfigured()) {
        return "Error: API key not configured";
    }

    // Use Gemini Pro model
    std::string model = "gemini-pro:generateContent";
    std::string url = base_url_ + model + "?key=" + api_key_;

    // Create JSON payload
    json payload;

    // Create contents array
    json contents;
    json parts;
    parts["text"] = prompt;
    contents["parts"] = json::array({ parts });
    payload["contents"] = json::array({ contents });

    // Add generation config if parameters are provided
    if (!parameters.empty()) {
        json generationConfig;
        for (const auto& param : parameters) {
            if (param.first == "temperature") {
                generationConfig["temperature"] = std::stod(param.second);
            }
            else if (param.first == "topP") {
                generationConfig["topP"] = std::stod(param.second);
            }
            else if (param.first == "topK") {
                generationConfig["topK"] = std::stoi(param.second);
            }
            else if (param.first == "maxOutputTokens") {
                generationConfig["maxOutputTokens"] = std::stoi(param.second);
            }
        }
        payload["generationConfig"] = generationConfig;
    }

    std::string post_data = payload.dump();

    std::cout << "Sending request to Gemini API..." << std::endl;
    std::string response = makeHttpRequest(url, post_data);

    // Parse the response to extract the actual text
    return parseAPIResponse(response);
}

std::string GeminiClient::makeHttpRequest(const std::string& url, const std::string& post_data) {
    CURL* curl;
    CURLcode res;
    std::string response;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlWriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "Gemini-CPP-Client/1.0");
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L); // 30 second timeout

        // Perform the request
        res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK) {
            response = "Error: " + std::string(curl_easy_strerror(res));
        }

        // Cleanup
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
    else {
        response = "Error: Failed to initialize CURL";
    }

    curl_global_cleanup();
    return response;
}

std::string GeminiClient::parseAPIResponse(const std::string& response) {
    try {
        // Check if it's an error message first
        if (response.find("Error:") == 0) {
            return response;
        }

        auto json_response = json::parse(response);

        // Check for API errors
        if (json_response.contains("error")) {
            return "API Error: " + json_response["error"]["message"].get<std::string>();
        }

        // Extract the generated text
        if (json_response.contains("candidates") && !json_response["candidates"].empty()) {
            auto& candidate = json_response["candidates"][0];
            if (candidate.contains("content") && candidate["content"].contains("parts")) {
                auto& parts = candidate["content"]["parts"];
                if (!parts.empty() && parts[0].contains("text")) {
                    return parts[0]["text"].get<std::string>();
                }
            }
        }

        return "Error: Unexpected response format from API";

    }
    catch (const json::parse_error& e) {
        return "Error: Failed to parse API response - " + std::string(e.what());
    }
    catch (const std::exception& e) {
        return "Error: " + std::string(e.what());
    }
}