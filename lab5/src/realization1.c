#include "realization.h"

int PrimeCount(int A, int B) {
	int N = B - A - 1 + 2;
	int sieve[2][N];
	for (int index = 0; index < N; ++index) {
		sieve[0][index] = index + A;
		sieve[1][index] = 0;
	}
	
	for (int indexExt = 1; indexExt < N; ++indexExt) {
		for (int indexInt = 0; indexInt < indexExt; ++indexInt) {
			if (sieve[0][indexExt] % sieve[0][indexInt] == 0) {
				sieve[1][indexExt] = 1;
			}
		}
	}	
	
	return sieve;
}

float Square(float A, float B) {
	return A * B;
}
