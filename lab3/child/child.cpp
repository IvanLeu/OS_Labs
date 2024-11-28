#include <windows.h>
#include <string>
#include <sstream>
#include <iostream>
#include <cstring>

#define SHARED_MEMORY_NAME "Local\\MySharedMemory"
#define SHARED_MEMORY_SIZE 1024

struct SharedMemory {
    char buffer[SHARED_MEMORY_SIZE];
    volatile int flag; //0 - idle, 1 - data ready, 2 - result ready
};

int main() {
    try {
        HANDLE hMapFile = OpenFileMappingA(
            FILE_MAP_WRITE, // Read/write access
            FALSE,          // Do not inherit the handle
            SHARED_MEMORY_NAME
        );

        if (!hMapFile) {
            throw std::runtime_error("Child: Failed to open file mapping");
        }

        SharedMemory* lpBase = (SharedMemory*)MapViewOfFile(
            hMapFile,       // Handle to map object
            FILE_MAP_WRITE, // Read/write access
            0,              // File offset high
            0,              // File offset low
            sizeof(SharedMemory)
        );

        if (!lpBase) {
            CloseHandle(hMapFile);
            throw std::runtime_error("Child: Failed to map view of file");
        }

        while (true) {
            if (lpBase->flag == 1) {
                char buffer[SHARED_MEMORY_SIZE];
                memcpy(buffer, lpBase->buffer, SHARED_MEMORY_SIZE);
                std::string input(buffer);

                if (input.empty()) {
                    break;
                }

                std::istringstream iss(input);
                std::string line;
                std::ostringstream resultBuffer;

                while (std::getline(iss, line)) {
                    std::istringstream line_ss(line);

                    float result;
                    line_ss >> result;

                    float y = 1.0f;
                    while (line_ss >> y) {
                        if (y == 0.0f) {
                            std::string resultStr = resultBuffer.str();
                            strncpy(static_cast<char*>(lpBase->buffer), resultStr.c_str(), SHARED_MEMORY_SIZE);
                            lpBase->flag = 2;
                            throw std::runtime_error("Child: Division by zero");
                        }

                        result /= y;
                    }

                    resultBuffer << result << " ";
                }

                std::string resultStr = resultBuffer.str();
                strncpy(static_cast<char*>(lpBase->buffer), resultStr.c_str(), SHARED_MEMORY_SIZE);
                lpBase->flag = 2;
            }
            else if (lpBase->flag == 0) {
                break;
            }
        }

        UnmapViewOfFile(lpBase);
        CloseHandle(hMapFile);
    }
    catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    catch (...) {
        std::cerr << "Unknown error occurred" << std::endl;
        return 1;
    }

    return 0;
}