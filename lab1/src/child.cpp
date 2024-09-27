#include <stdlib.h>
#include <stdio.h>

int main() {
	char line[256];

	while (1) {
		if (fgets(line, sizeof(line), stdin) == NULL) {
			break;
		}
		
		char* ptr = line;

		float result;
		if (sscanf(ptr, "%f", &result) != 1) {
			return 0;
		}

		float val;
		while (sscanf(ptr, "%f", &val) == 1) {
			if (val == 0.0f) {
				return 0;
			}

			result /= val;
		}

		char buffer[256];
		sprintf(buffer, "%f\n", result);
		fwrite(buffer, 1, sizeof(buffer), stdout);
	}

	return 0;
}