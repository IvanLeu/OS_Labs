#include <Windows.h>
#include <string>
#include <iostream>

typedef float (*Derivative)(float, float);
typedef int (*PrimeCount)(int, int);

int main() {
	HINSTANCE libHandle = nullptr;
	Derivative derFunc = nullptr;
	PrimeCount primeCountFunc = nullptr;

	libHandle = LoadLibraryA("contract_native.dll");

	if (!libHandle) {
		std::cerr << "Failed to include lib file" << std::endl;
		return 1;
	}

	derFunc = (Derivative)GetProcAddress(libHandle, "Derivative");
	primeCountFunc = (PrimeCount)GetProcAddress(libHandle, "PrimeCount");

	if (!derFunc || !primeCountFunc) {
		std::cerr << "Failed to load the functions" << std::endl;
		FreeLibrary(libHandle);
		return 1;
	}

	std::cout << derFunc(3.1415f / 2.0f, 0.0001f) << std::endl;

	std::cout << primeCountFunc(3, 100) << std::endl;

	return 0;
}