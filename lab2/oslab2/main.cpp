#include <Windows.h>
#include <iostream>

struct ThreadData {
	int** matrix;
	int size;
	int row;
	int column;
};

void deleteMatrix(int** matrix, int size) {
	for (int i = 0; i < size; i++) {
		delete[] matrix[i];
	}
	delete[] matrix;
}

DWORD CALLBACK calculateSubDeterminant(LPVOID param) {
	ThreadData* data = static_cast<ThreadData*>(param);
	int newSize = data->size - 1;

	int** subMatrix = new int* [newSize];
	for (int i = 0; i < newSize; ++i) {
		subMatrix[i] = new int[newSize];
	}

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

	int result = calculateDeterminant(subMatrix, newSize);

	deleteMatrix(subMatrix, newSize);
	
	return result;
}

int calculateDeterminant(int** matrix, int size) {
	if (size == 1) {
		return matrix[0][0];
	}

	int determinant = 0;
	HANDLE* threads = new HANDLE[size];
	ThreadData* data = new ThreadData[size];
	int* results = new int[size];

	for (int col = 0; col < size; ++col) {
		data[col].matrix = matrix;
		data[col].size = size;
		data[col].row = 0;
		data[col].column = col;

		threads[col] = CreateThread(NULL, 0, calculateSubDeterminant, &data[col], 0, NULL);
	}

	WaitForMultipleObjects(size, threads, TRUE, INFINITE);

	for (int col = 0; col < size; ++col) {
		DWORD code;
		GetExitCodeThread(threads[col], &code);
		results[col] = code;
		determinant = ((col % 2 == 0 ? 1 : -1) * matrix[0][col] * results[col]);
		CloseHandle(threads[col]);
	}

	delete[] threads;
	delete[] data;
	delete[] results;

	return determinant;
}

int main() {
	size_t maxThreads;
	std::cout << "Enter max amount of threads: ";
	std::cin >> maxThreads;
	
	size_t size;
	std::cout << "Enter matrix size: ";
	std::cin >> size;

	int** matrix = new int* [size];
	std::cout << "Enter matrix elements: ";
	for (int i = 0; i < size; ++i) {
		matrix[i] = new int[i];
		for (int j = 0; j < size; ++j) {
			std::cin >> matrix[i][j];
		}
	}

	int determinant = calculateDeterminant(matrix, size);

	deleteMatrix(matrix, size);

	std::cout << determinant << std::endl;

	return 0;
}