#include "realization.h"

int PrimeCount(int A, int B) {
	int N = B - A - 1 + 2;
	int sieve[2][N];
	int temp[N];
	int min = 2;
	
	for (int index = 0; index < N; ++index) {
		sieve[0][index] = index + A;
		sieve[1][index] = 0;
		temp[index] = 0;
	}
	
	for (int indexExt = min; indexExt * indexExt < N; ++indexExt) {
		if (temp[indexExt] == 0) {
			for (int indexInt = indexExt * indexExt; indexInt < N; indexInt += indexExt) {
				temp[indexInt] = 1;
			}
		}
	}
	
	for (int index = A; index <= B; ++index) {
		sieve[1][index - A] = temp[index];
	}
	
	return sieve;
}

float Square(float A, float B) {
	return 0.5 * A * B; 
}
