#include <stdio.h>
#include <Windows.h>
#include <stdexcept>
#include <filesystem>
#include <iostream>
#include <iterator>

int main() {
	try {
		std::string file_name;
		std::cin >> file_name;
		std::wstring wfile_name(file_name.begin(), file_name.end());

		HANDLE hFile = NULL;
		HANDLE hChild_STDOUT_RD = NULL;
		HANDLE hChild_STDOUT_WR = NULL;

		SECURITY_ATTRIBUTES sa;
		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		sa.bInheritHandle = TRUE;
		sa.lpSecurityDescriptor = NULL;
		
		hFile = CreateFile(wfile_name.c_str(), GENERIC_READ, 0, &sa, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		if (hFile == INVALID_HANDLE_VALUE) {
			throw std::runtime_error("Failed to open a file");
		}

		if (!CreatePipe(&hChild_STDOUT_RD, &hChild_STDOUT_WR, &sa, 0)) {
			throw std::runtime_error("Failed to create the pipe");
		}

		if (!SetHandleInformation(hChild_STDOUT_RD, HANDLE_FLAG_INHERIT, 0)) {
			throw std::runtime_error("Failed set handle information");
		}

		//Process creation
		TCHAR szCmdLine[] = TEXT("child.exe");
		PROCESS_INFORMATION piProcInfo;
		STARTUPINFO siStartInfo;
		BOOL bSuccess = FALSE;

		ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));

		ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
		siStartInfo.cb = sizeof(STARTUPINFO);
		siStartInfo.hStdInput = hFile;
		siStartInfo.hStdOutput = hChild_STDOUT_WR;
		siStartInfo.hStdError = hChild_STDOUT_WR;
		siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

		bSuccess = CreateProcess(
			NULL,
			szCmdLine,
			NULL,
			NULL,
			TRUE,
			0,
			NULL,
			NULL,
			&siStartInfo,
			&piProcInfo);

		if (!bSuccess) {
			throw std::runtime_error("Failed to create a child process");
		}
		
		CloseHandle(hChild_STDOUT_WR);
		CloseHandle(hFile);

		DWORD bytesRead;
		float result;
		while(ReadFile(hChild_STDOUT_RD, &result, sizeof(float), &bytesRead, NULL))
		{
			std::cout << result << std::endl;
		}

		WaitForSingleObject(piProcInfo.hProcess, INFINITE);

		CloseHandle(hChild_STDOUT_RD);
		CloseHandle(piProcInfo.hProcess);
		CloseHandle(piProcInfo.hThread);
	}
	catch (const std::runtime_error& e) {
		std::string error = e.what();
		std::wstring werror(error.begin(), error.end());

		MessageBox(NULL, werror.c_str(), L"Runtime Error", MB_OK);
	}
	catch (...) {
		MessageBox(NULL, L"Unknownd error ocurred", L"Error", MB_OK);
	}

	return 0;
}