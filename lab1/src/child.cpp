#include <stdio.h>
#include <Windows.h>
#include <exception>

HANDLE g_hChild_STDIN_RD = NULL;
HANDLE g_hChild_STDIN_WR = NULL;
HANDLE g_hChild_STDOUT_RD = NULL;
HANDLE g_hChild_STDOUT_WR = NULL;

int main() {
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(sa);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = NULL;

	if (!CreatePipe(g_hChild_STDOUT_RD, g_hChild_STDOUT_WR, &sa, 0)) {
		
	}
}