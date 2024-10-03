#include <stdlib.h>
#include <stdio.h>
#include <Windows.h>
#include <string>
#include <sstream>

int main() {
	HANDLE readHandle = GetStdHandle(STD_INPUT_HANDLE);
	HANDLE writeHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD readBytes, writtenBytes;

	if (readHandle == INVALID_HANDLE_VALUE) {
		fprintf(stderr, "Child: Failed to get stdin handle");
	}

	if (writeHandle == INVALID_HANDLE_VALUE) {
		fprintf(stderr, "Child: Failed to get stdout handle");
	}

	char buffer[256];
	while (true) {
		float result;

		if (ReadFile(readHandle, buffer, 255, &readBytes, NULL)) {

			if (readBytes == 0) {
				exit(1);
			}

			buffer[readBytes] = 0;

			std::string input(buffer);
			std::istringstream iss(input);

			std::string line;

			while (std::getline(iss, line)) {
				std::istringstream line_ss(line);

				float y;
				
				line_ss >> result;

				while (line_ss >> y) {
					if (y == 0.0f) {
						exit(1);
					}

					result /= y;
				}

				WriteFile(writeHandle, &result, sizeof(float), &writtenBytes, NULL);
			}
		}
		else {
			exit(1);
		}

	}

	return 0;
}