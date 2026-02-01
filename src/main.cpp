#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <cstdlib>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#include "httplib.h"

class FileServer {
private:
    std::vector<std::string> serverLog;
    std::string uploadDir;
    
    void logMessage(const std::string& message) {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::tm tm_buf;
        localtime_r(&time, &tm_buf);
        
        std::stringstream ss;
        ss << "[" << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S") << "] " << message;
        std::string logEntry = ss.str();
        
        serverLog.push_back(logEntry);
        std::cout << logEntry << std::endl;
    }
    
    bool saveUploadedFile(const std::string& filename, const std::string& content) {
        if (filename.empty()) {
            logMessage("ошибка! имя файла пустое");
            return false;
        }
        
        std::string safe_filename = filename;
        std::string filepath = uploadDir + "/" + safe_filename;
        
        int counter = 1;
        while (std::ifstream(filepath)) {
            size_t dot_pos = safe_filename.find_last_of('.');
            std::string name, ext;
            if (dot_pos != std::string::npos) {
                name = safe_filename.substr(0, dot_pos);
                ext = safe_filename.substr(dot_pos);
            } else {
                name = safe_filename;
                ext = "";
            }
            safe_filename = name + "_" + std::to_string(counter++) + ext;
            filepath = uploadDir + "/" + safe_filename;
        }
        
        try {
            std::ofstream outfile(filepath, std::ios::binary);
            if (!outfile) {
                logMessage("ошибка создания файла: " + filepath);
                return false;
            }
            
            outfile.write(content.c_str(), content.size());
            outfile.close();
            
            chmod(filepath.c_str(), 0644);
            
            logMessage("файл сохранён: " + safe_filename + 
                      " (" + std::to_string(content.size()) + " байт)");
            return true;
            
        } catch (const std::exception& e) {
            logMessage("исключение при сохранении: " + std::string(e.what()));
            return false;
        }
    }
    
    std::string getFilenameFromRequest(const httplib::Request& req) {
        std::string contentType = req.get_header_value("Content-Type");
        std::string disposition = req.get_header_value("Content-Disposition");
        
        if (!disposition.empty() && disposition.find("filename=") != std::string::npos) {
            size_t start = disposition.find("filename=") + 9;
            size_t end = disposition.find(";", start);
            if (end == std::string::npos) end = disposition.length();
            
            std::string filename = disposition.substr(start, end - start);
            
            if (filename.length() >= 2 && filename[0] == '"' && filename[filename.length()-1] == '"') {
                filename = filename.substr(1, filename.length() - 2);
            }
            
            if (!filename.empty()) {
                return filename;
            }
        }
        
        return "uploaded_file_" + std::to_string(time(nullptr));
    }
    
public:
    FileServer() : uploadDir("/tmp") {
        if (mkdir(uploadDir.c_str(), 0777) != 0 && errno != EEXIST) {
            logMessage("ошибка создания директории " + uploadDir);
        } else {
            logMessage("директория загрузок: " + uploadDir);
        }
        logMessage("сервер инициализирован");
    }
    
    void run(int port = 1616) {
        httplib::Server svr;
        
        svr.Get("/info", [](const httplib::Request&, httplib::Response& res) {
            res.set_content("все ок\n", "text/plain");
        });
        
        svr.Get("/log", [this](const httplib::Request&, httplib::Response& res) {
            std::string logContent;
            for (const auto& entry : serverLog) {
                logContent += entry + "\n";
            }
            res.set_content(logContent, "text/plain");
        });
        
        svr.Post("/upload", [this](const httplib::Request& req, httplib::Response& res) {
            if (req.body.empty()) {
                logMessage("ошибка! пустое тело запроса");
                res.set_content("ошибка! пустое тело запроса\n", "text/plain");
                res.status = 400;
                return;
            }
            
            std::string filename = getFilenameFromRequest(req);
            
            if (saveUploadedFile(filename, req.body)) {
                res.set_content("все ок! файл загружен\n", "text/plain");
                logMessage("успешная загрузка: " + filename);
            } else {
                res.set_content("ошибка! не удалось сохранить файл\n", "text/plain");
                res.status = 500;
            }
        });
        
        svr.Get("/", [](const httplib::Request&, httplib::Response& res) {
            std::string response = 
                "HTTP File Server\n"
                "порт: 1616\n"
                "методы:\n"
                "  GET  /info  - проверка работы\n"
                "  GET  /log   - логи сервера\n"
                "  POST /upload - загрузить файл\n"
                "\n"
                "использование wget:\n"
                "  wget --post-file=файл.txt http://адрес:1616/upload\n";
            res.set_content(response, "text/plain");
        });
        
        logMessage("запуск сервера на порту " + std::to_string(port));
        logMessage("сервер доступен: http://0.0.0.0:" + std::to_string(port));
        
        if (!svr.listen("0.0.0.0", port)) {
            logMessage("ошибка запуска сервера");
            std::cerr << "ошибка! не удалось запустить сервер\n";
        }
    }
};

int main() {
    if (geteuid() != 0) {
        std::cerr << "ошибка! программа должна запускаться от root\n";
        std::cerr << "используйте sudo ./build/http-server\n";
        return 1;
    }
    
    std::cout << "запуск http сервера...\n";
    
    try {
        FileServer server;
        server.run(1616);
    } catch (const std::exception& e) {
        std::cerr << "ошибка: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}