#pragma once

#define API _declspec(dllexport)

extern "C" {
	API float Derivative(float A, float deltaX);
	API int PrimeCount(int A, int B);
}