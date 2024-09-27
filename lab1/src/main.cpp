#include <stdio.h>
#include <Windows.h>
#include <stdexcept>

int main() {
	try {
		HANDLE hFile = NULL;
		HANDLE hChild_STDOUT_RD = NULL;
		HANDLE hChild_STDOUT_WR = NULL;

		SECURITY_ATTRIBUTES sa;
		sa.nLength = sizeof(sa);
		sa.bInheritHandle = TRUE;
		sa.lpSecurityDescriptor = NULL;

		hFile = CreateFile("files\\file.txt", GENERIC_READ, 0, &sa, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);

		if (hFile == INVALID_HANDLE_VALUE) {
			throw std::runtime_error("Failed to open a file");
		}

		if (!CreatePipe(&hChild_STDOUT_RD, &hChild_STDOUT_WR, &sa, 0)) {
			throw std::runtime_error("Failed to create the pipe");
		}

		if (!SetHandleInformation(&hChild_STDOUT_RD, HANDLE_FLAG_INHERIT, 0)) {
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
		siStartInfo.hStdError = GetStdHandle(STD_ERROR_HANDLE);
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
			throw std::runtime_error("Filed to create a child process");
		}

		CloseHandle(hChild_STDOUT_WR);
		CloseHandle(hFile);

		DWORD bytesRead, bytesWritten;
		char buffer[4096];
		while (ReadFile(hChild_STDOUT_RD, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
			buffer[bytesRead] = 0;
			printf("%s", buffer);
		}

		CloseHandle(hChild_STDOUT_RD);
		CloseHandle(piProcInfo.hProcess);
		CloseHandle(piProcInfo.hThread);
	}
	catch (const std::runtime_error& e) {
		MessageBox(NULL, e.what(), "Runtime Error", MB_OK);
	}
	catch (...) {
		MessageBox(NULL, "Unknownd error ocurred", "Error", MB_OK);
	}

	return 0;
}