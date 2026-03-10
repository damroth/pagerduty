#include <iostream>
#include <string>
#include "http_client.hpp"

size_t write_callback(char* ptr, size_t size, size_t nmemb, std::string *data) {
    data->append(ptr, size * nmemb);
    return size * nmemb;
}


HttpClient::HttpClient() {
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
}

HttpClient::~HttpClient() {
    if (curl) curl_easy_cleanup(curl);
    curl_global_cleanup();
}

   
std::string HttpClient::get(const std::string &url, const std::string &header) {
    std::string response;

    struct curl_slist *headers = nullptr;
    if (!header.empty())
        headers = curl_slist_append(headers, header.c_str());
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode result = curl_easy_perform(curl);
    if (result != CURLE_OK)
        std::cerr << "curl failed: " << curl_easy_strerror(result) << "\n";

    curl_slist_free_all(headers);
    return response;
}