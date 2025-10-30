#include "helper.h"
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include <sstream>
#include <time.h>
#include <limits>

using json = nlohmann::json;

// Callback function for writing data
size_t GeminiClient::curlWriteCallback(void* contents, size_t size, size_t nmemb, std::string* response)
{
    size_t total_size = size * nmemb;
    response->append((char*)contents, total_size);
    return total_size;
}

GeminiClient::GeminiClient(const std::string& api_key) : api_key_(api_key) {}

bool GeminiClient::isConfigured() const
{
    if (api_key_.empty())
    {
        return false;
    }
    else
    {
        return true;        // Has API configured
    }
}

std::string GeminiClient::generateContent(const std::string& prompt)
{
    return generateContent(prompt, {});     // Allows the user to add more parameters if needed (i.e. tempurature/max output tokens)s
}

std::string GeminiClient::generateContent(const std::string& prompt, const std::map<std::string, std::string>& parameters)
{
    if (!isConfigured())
    {
        return "Error: API key not configured";
    }

    // Use Gemini 2.5 flash
    std::string model = "gemini-2.5-flash:generateContent";
    std::string url = base_url_ + model + "?key=" + api_key_;
    //std::string url = base_url_ + "?key=" + api_key_;

    // Create JSON payload
    json payload;

    // Create contents array
    json contents;
    json parts;
    parts["text"] = prompt;
    contents["parts"] = json::array({ parts });
    payload["contents"] = json::array({ contents });

    // Add generation config if parameters are provided
    if (!parameters.empty())
    {
        json generationConfig;         // Makes json object to hold generation configuration
        
        // Parameter types
        // temperature - creativity (0.0 - 1.0)
        // topP - response diversity
        // topK - vocabulary section
        // maxOutputTokens - response length limit

        for (const auto& param : parameters)
        {
            if (param.first == "temperature")
            {
                generationConfig["temperature"] = std::stod(param.second);          // stod converts string to double. And stoi converts string to integer.
            }
            else if (param.first == "topP")
            {
                generationConfig["topP"] = std::stod(param.second);
            }
            else if (param.first == "topK")
            {
                generationConfig["topK"] = std::stoi(param.second);
            }
            else if (param.first == "maxOutputTokens")
            {
                generationConfig["maxOutputTokens"] = std::stoi(param.second);
            }
        }
        payload["generationConfig"] = generationConfig;
    }

    std::string post_data = payload.dump();

    std::cout << "\nSending request to Gemini API...\n" << std::endl;
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
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);       // 30 second timeout

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
    else
    {
        response = "Error: Failed to initialize cURL";
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

void getPromptAndPushAPI(GeminiClient& client)
{
    std::string prompt;
    std::cout << "\nHello! Please input your prompt for the API call:" << std::endl;
    //std::cin >> prompt;
    std::getline(std::cin, prompt);
    std::string response = client.generateContent(prompt);
    std::cout << response << std::endl;
    std::cout << "----------------------------------------\n" << std::endl;
}

void demo1(GeminiClient& client)
{
    std::cout << '\n' << std::endl;
    std::cout << "Testing basic API call...\n" << std::endl;
    std::map<std::string, std::string> params;
    params["temperature"] = "0.8";
    params["maxOutputTokens"] = "500";
    std::string prompt = "Hello! Please introduce yourself briefly.";
    std::cout << "Prompt: " << prompt << '\n' << std::endl;
    std::string response = client.generateContent(prompt);
    std::cout << "Response:\n" << response << std::endl;
    std::cout << "----------------------------------------\n" << std::endl;
}

std::string displayMenu()
{
    std::string response;

    std::cout << "Please choose from one of the following menu items:" << std::endl;
    std::cout << "(1) Demo 1 test" << std::endl;
    std::cout << "(2) Free Response prompt to gemini" << std::endl;
    std::cout << "(0) Exit program" << std::endl;
    std::cout << "Please enter your desired option: ";
    //std::getline(std::cin, response);
    //std::cin >> response;
    std::getline(std::cin >> std::ws, response);        // Clears out the leading whitespace in the stream.
    return response;
}

void wait(int milliseconds)      //in milliseconds
{
    clock_t endwait;
    endwait = clock() + milliseconds * CLOCKS_PER_SEC / 1000;
    while (clock() < endwait) {}
}

void cinFailSafe() // Clears the cin input if it is an incorrect input. e.g. string if it only accepts ints.
{
    if (std::cin.fail())
    {
        // Clear the error flag and discard invalid input
        std::cin.clear();
        std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n'); // Added <limits> in the header
        /* Also 'std::numeric_limits<std::streamsize>::max()' works good on every compiler BUT MSVC.
        * Like it works fine on MinGW-w64, Clang, and WSL with GCC.
        * BUT this doesn't work with MSVC. So if you're using this on visual studio 2022 then it is okay and you don't
        * need to change anything. But if you are working on visual stidio code then you just take out the parenthesis outisde of 
        * std::numeric_limits<std::streamsize>::max
        */ 
    }
}