#include <iostream>

long long SumOfProducts(int arr_num, int* arr_size, int** arrays); // How can I make my two-dimensional array const?
bool nextPermutation(int arr_num, const int* arr_size, int* indices);
bool next(int arr_num, const int* arr_size, int* indices);
bool pairwiseDifferent(int n, const int* lst);
long long product(int arr_num, int** arrays, const int* indices);
int str_to_int(const char* number);

void check_nextPermutation(int arr_num, const int* arr_size);

int main(int argc, char* argv[]) {
    int arr_num = argc - 1;
    int* arr_size = new int[arr_num];
    for (int i = 0; i < arr_num; i++)
        arr_size[i] = str_to_int(argv[i + 1]);
    int** arrays = new int*[arr_num];
    for (int i = 0; i < arr_num; i++) {
        arrays[i] = new int[arr_size[i]];
        for (int j = 0; j < arr_size[i]; j++)
            std::cin >> arrays[i][j];
    }

    std::cout << SumOfProducts(arr_num, arr_size, arrays);
    //check_nextPermutation(arr_num, arr_size);

    for (int i = 0; i < arr_num; i++)
        delete[] arrays[i];
    delete[] arrays;
    delete[] arr_size;
    return 0;
}

int str_to_int(const char* number) {
    int ans = 0;
    for (int i = 0; number[i] != '\0'; i++){
        ans *= 10;
        ans += number[i] - '0';
    }
    return ans;
}

bool pairwiseDifferent(int n, const int* lst) {
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++)
            if (lst[i] == lst[j])
                return false;
    return true;
}

bool next(int arr_num, const int* arr_size, int* indices) {
    for (int i = 0; i < arr_num; i++) {
        indices[i] += 1;
        if (indices[i] < arr_size[i])
            return true;
        indices[i] = 0;
    }
    return false;
}

bool nextPermutation(int arr_num, const int* arr_size, int* indices) {
    do {
        if (!next(arr_num, arr_size, indices))
            return false;
    } while (!pairwiseDifferent(arr_num, indices));
    return true;
}

void check_nextPermutation(int arr_num, const int* arr_size) {
    std::cout << "sizes of arrays:\n";
    for (int i = 0; i < arr_num; i++)
        std::cout << arr_size[i] << " ";
    std::cout << "\nPermutations:\n";
    int* indices = new int[arr_num];
    do {
        for (int i = 0; i < arr_num; i++)
            std::cout << indices[i] << " ";
        std::cout << std::endl;
    } while (nextPermutation(arr_num, arr_size, indices));
    delete[] indices;
}

long long product(int arr_num, int** arrays, const int* indices) {
    long long ans = 1;
    for (int i = 0; i < arr_num; i++)
        ans *= arrays[i][indices[i]];
    return ans;
}

long long SumOfProducts(int arr_num, int* arr_size, int** arrays) {
    long long sum = 0;
    int* indices = new int[arr_num];
    while (nextPermutation(arr_num, arr_size, indices)) {
        sum += product(arr_num, arrays, indices);
    }
    delete[] indices;
    return sum;
	//12334
}
