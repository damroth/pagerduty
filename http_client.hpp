#pragma once
#include <string>
#include <curl/curl.h>

size_t write_callback(char* ptr, size_t size, size_t nmemb, std::string *data);

class HttpClient {
    CURL* curl;
public:
    HttpClient();
    ~HttpClient();
    std::string get(const std::string &url, const std::string &header = "");
};