#include "contract.h"
#include <cmath>

extern "C" {
    float Derivative(float A, float deltaX)
    {
        return (std::cos(A + deltaX) - std::cos(A)) / deltaX;
    }

    int PrimeCount(int A, int B)
    {
        if (A > B) return 0;

        int count = 0;
        for (int i = A; i <= B; ++i) {
            if (i < 2) {
                continue;
            }

            bool isPrime = true;;
            for (int j = 2; j * j <= i; ++j) {
                if (i % j == 0) {
                    isPrime = false;
                    break;
                }
            }
            if (isPrime) ++count;
        }

        return count;
    }
}
