#include <contract.h>
#include <iostream>

int main() {
	std::cout << Derivative(3.1415f / 2.0f, 0.0001f) << std::endl;

	std::cout << PrimeCount(3, 100) << std::endl;

	return 0;
}
