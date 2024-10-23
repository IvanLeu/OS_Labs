#include <Windows.h>
#include <iostream>
#include <vector>

struct ThreadData {
	std::vector<std::vector<int>> matrix;
	size_t size;
	size_t row;
	size_t column;
	int nThreads;
	HANDLE semaphore;
};

int calculateDeterminant(const std::vector<std::vector<int>>& matrix, size_t size, size_t nThreads);

DWORD CALLBACK calculateSubDeterminant(LPVOID param) {
	ThreadData* data = static_cast<ThreadData*>(param);
	size_t newSize = data->size - 1;

	std::vector<std::vector<int>> subMatrix(newSize, std::vector<int>(newSize));

	for (int i = 1; i < data->size; i++) {
		int colIndex = 0;
		for (int j = 0; j < data->size; ++j) {
			if (j == data->column) {
				continue;
			}
			subMatrix[i - 1][colIndex] = data->matrix[i][j];
			colIndex++;
		}
	}

	int result = calculateDeterminant(subMatrix, newSize, data->nThreads);

	ReleaseSemaphore(data->semaphore, 1, NULL);

	return result;
}

int calculateDeterminant(const std::vector<std::vector<int>>& matrix, size_t size, size_t nThreads) {
	if (size == 1) {
		return matrix[0][0];
	}

	HANDLE semaphore = CreateSemaphore(NULL, nThreads, nThreads, NULL);

	int determinant = 0;
	HANDLE* threads = new HANDLE[size];
	ThreadData* data = new ThreadData[size];
	int* results = new int[size];

	for (int col = 0; col < size; ++col) {
		
		WaitForSingleObject(semaphore, INFINITE);

		data[col].matrix = matrix;
		data[col].size = size;
		data[col].row = 0;
		data[col].column = col;
		data[col].semaphore = semaphore;
		data[col].nThreads = nThreads;

		threads[col] = CreateThread(NULL, 0, calculateSubDeterminant, &data[col], 0, NULL);
	}

	WaitForMultipleObjects(size, threads, TRUE, INFINITE);

	for (int col = 0; col < size; ++col) {
		DWORD code;
		GetExitCodeThread(threads[col], &code);
		results[col] = code;
		determinant += ((col % 2 == 0 ? 1 : -1) * matrix[0][col] * results[col]);
		CloseHandle(threads[col]);
	}

	delete[] threads;
	delete[] data;
	delete[] results;

	CloseHandle(semaphore);

	return determinant;
}

int main() {
	size_t maxThreads;
	std::cout << "Enter max amount of threads: ";
	std::cin >> maxThreads;
	
	size_t size;
	std::cout << "Enter matrix size: ";
	std::cin >> size;

	std::vector<std::vector<int>> matrix(size, std::vector<int>(size));
	std::cout << "Enter matrix elements: ";
	for (int i = 0; i < size; ++i) {
		for (int j = 0; j < size; ++j) {
			std::cin >> matrix[i][j];
		}
	}

	int determinant = calculateDeterminant(matrix, size, maxThreads);

	std::cout << determinant << std::endl;

	return 0;
}