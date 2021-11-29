#include <stdio.h>
#include "realization.h"

int main() {
	int cmd = 0;
	while(scanf("%d", &cmd) != EOF) {
		switch(cmd) {
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
