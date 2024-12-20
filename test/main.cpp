


#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <map>


std::vector<std::string> parseStartLine(std::string request) {
    std::string first_line = request.substr(0, request.find("\r\n"));
    std::istringstream ss(first_line);
    std::vector<std::string> start_line;

    while (ss) {
        std::string token;
        std::getline(ss, token, ' ');
        if (!token.empty())
            start_line.push_back(token);
    }
    return start_line;
}

std::map<std::string, std::string> parseHeader(std::string request) {
    std::map<std::string, std::string> header;
    size_t start = request.find("\r\n") + 2;
    size_t end = request.find("\r\n\r\n");
    std::string header_str = request.substr(start, end - start);
    std::istringstream ss(header_str);

    while (ss) {
        std::string line;
        std::getline(ss, line, '\n');
        if (!line.empty()) {
            std::string key = line.substr(0, line.find(":"));
            std::string value = line.substr(line.find(":") + 1, line.find("\r"));
            header[key] = value;
        }
    }
    return header;
}

std::string parseBody(std::string request) {
    std::string body = request.substr(request.find("\r\n\r\n") + 4);
    return body;
}

int main(void) {
    std::string httpRequest = 
        "GET /index.html HTTP/1.1\r\n \
        Host: localhost:8080\r\n \
        Connection: keep-alive\r\n \
        sec-ch-ua: \"Google Chrome\";v=\"131\", \"Chromium\";v=\"131\", \"Not_A Brand\";v=\"24\"\r\n \
        sec-ch-ua-mobile: ?0\r\n \
        sec-ch-ua-platform: \"macOS\"\r\n \
        Upgrade-Insecure-Requests: 1\r\n \
        User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.114 Safari/537.36\r\n \
        Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9\r\n \
        Sec-Fetch-Site: none\r\n \
        Sec-Fetch-Mode: navigate\r\n \
        Sec-Fetch-User: ?1\r\n \
        Sec-Fetch-Dest: document\r\n \
        Accept-Encoding: gzip, deflate, br\r\n \
        Accept-Language: ja,en-US;q=0.9,en;q=0.8\r\n \
        Cookie: _ga=GA1.1.123456789.1234567890\r\n \
        \r\n";
    std::string Request =
        "POST /submit/index.php?12345 HTTP/1.1\r\n"           // Start-Line
        "Host: localhost:8080\r\n"            // Header
        "Content-Type: application/x-www-form-urlencoded\r\n"
        "Content-Length: 27\r\n"              // Bodyの長さを示す
        "Connection: close\r\n"
        "\r\n"                                // ヘッダとボディの区切り（空行）
        "name=JohnDoe&age=30&city=Tokyo";
    std::vector<std::string> result =  parseStartLine(Request);
    std::cout << "Start-Line" << std::endl;
    for (std::vector<std::string>::iterator it = result.begin(); it != result.end(); ++it) {
        std::cout << *it << std::endl;
    }
    std::cout << "Header" << std::endl;
    std::map<std::string, std::string> header = parseHeader(Request);
    for (std::map<std::string, std::string>::iterator it = header.begin(); it != header.end(); ++it) {
        std::cout << it->first << ": " << it->second << std::endl;
    }
    std::cout << "Body" << std::endl;
    std::string body = parseBody(Request);
    std::cout << body << std::endl;
}

