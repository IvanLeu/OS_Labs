#include <windows.h>
#include <stdexcept>
#include <iostream>
#include <string>
#include <cstring>

#define SHARED_MEMORY_NAME "Local\\MySharedMemory"
#define SHARED_MEMORY_SIZE 1024

struct SharedMemory {
    char buffer[SHARED_MEMORY_SIZE];
    volatile int flag; //0 - idle, 1 - data ready, 2 - result ready
};

int main() {
    try {
        std::string file_name;
        std::cin >> file_name;

        HANDLE hFile = CreateFileA(file_name.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile == INVALID_HANDLE_VALUE) {
            throw std::runtime_error("Failed to open a file");
        }

        HANDLE hMapFile = CreateFileMappingA(
            INVALID_HANDLE_VALUE, // Use the system paging file
            NULL,                 // Default security
            PAGE_READWRITE,       // Read/write access
            0,                    // Maximum object size (high-order DWORD)
            sizeof(SharedMemory),   // Maximum object size (low-order DWORD)
            SHARED_MEMORY_NAME    // Name of the shared memory object
        );

        if (!hMapFile) {
            throw std::runtime_error("Failed to create file mapping");
        }

        SharedMemory* lpBase = (SharedMemory*)MapViewOfFile(
            hMapFile,            // Handle to map object
            FILE_MAP_WRITE,      // Read/write access
            0,                   // File offset high
            0,                   // File offset low
            SHARED_MEMORY_SIZE   // Number of bytes to map
        );

        if (!lpBase) {
            CloseHandle(hMapFile);
            throw std::runtime_error("Failed to map view of file");
        }

        lpBase->flag = 0;

        TCHAR szCmdLine[] = TEXT("child.exe");
        PROCESS_INFORMATION piProcInfo;
        STARTUPINFO siStartInfo;
        ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));
        ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
        siStartInfo.cb = sizeof(STARTUPINFO);

        if (!CreateProcess(
            NULL,
            szCmdLine,
            NULL,
            NULL,
            FALSE,
            0,
            NULL,
            NULL,
            &siStartInfo,
            &piProcInfo)) {
            throw std::runtime_error("Failed to create a child process");
        }

        char buffer[SHARED_MEMORY_SIZE];
        DWORD bytesRead;
        while (ReadFile(hFile, buffer, sizeof(buffer), &bytesRead, NULL) && bytesRead > 0) {
            memcpy(lpBase->buffer, buffer, bytesRead);
            lpBase->flag = 1;

            do {}
            while (lpBase->flag == 1);

            if (lpBase->flag == 2) {
                std::cout << "Result: " << static_cast<char*>(lpBase->buffer) << std::endl;

                memset(lpBase->buffer, 0, SHARED_MEMORY_SIZE);
                lpBase->flag = 0;
            }
        }

        CloseHandle(hFile);

        WaitForSingleObject(piProcInfo.hProcess, INFINITE);

        UnmapViewOfFile(lpBase);
        CloseHandle(hMapFile);
        CloseHandle(piProcInfo.hProcess);
        CloseHandle(piProcInfo.hThread);
    }
    catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Unknown error occurred" << std::endl;
    }

    return 0;
}