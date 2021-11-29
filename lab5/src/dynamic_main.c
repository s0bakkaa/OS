#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

typedef enum {
	first,
	second,
} Contract;

Contract contract = first;

const char* lib1 = "libFirst.so";
const char* lib2 = "libSecond.so";

int (*PrimeCount)(int, int) = NULL;
float (*Square)(float, float) = NULL;

void *libHandle = NULL;

void libLoad(Contract cont) {
	const char* name;
	switch(cont) {
		case first:
			name = lib1;
			break;
		case second:
			name = lib2;
			break;
	}
	libHandle = dlopen(name, RTLD_LAZY);
	if (libHandle == NULL) {
		perror("dlopen");
		exit(EXIT_FAILURE);
	}
}

void contractLoad() {
	libLoad(contract);
	PrimeCount = dlsym(libHandle, "PrimeCount");
	Square = dlsym(libHandle, "Square");
}

void contractChange() {
	dlclose(libHandle);
	switch(contract) {
		case first:
			contract = second;
			break;
		case second:
			contract = first;
			break;
	}
	contractLoad;
}

int main() {
	int cmd = 0;
	contractLoad();
	while(scanf("%d", &cmd) != EOF) {
		switch(cmd) {
			case 0: {
					contractChange();
					printf("The contract is changed\n");
					switch(contract) {
						case first:
							printf("Number of contract: 1\n");
							break;
						case second:
							printf("Number of contract: 2\n");
							break;
					}
				}
				break;
			case 1: {
					int A, B;
					if (scanf("%d %d", &A, &B) == 2) {
						if (A < 2 || B < 2) {
							printf("The number A and the number B must be 2 or larger\n");
						}
						else if (A > B) {
							printf("The number B must be larger than number A or equal\n");
						}
						else {
							printf("Hello\n");
						}
					}
				}
				break;
			case 2: {
					float numA, numB;
					if (scanf("%f %f", &numA, &numB) == 2) {
						if (numA < 0 || numB < 0) {
							printf("The number A and the number B must be non-negative\n");
						}
						else {
							printf("The square is %.2f\n", Square(numA, numB));
						}
					}
				}
				break;
			default: {
				printf("Wrong answer\n");
			}
		}
	}
	return 0;
}
