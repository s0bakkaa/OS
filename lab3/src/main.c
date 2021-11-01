#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <math.h>
//  #include <unistd.h>
//  #include <ctype.h>

struct Arguments {
	int id; //  ID of thread
	int num_of_throws;
	int sum_of_points_1;
	int sum_of_points_2;
	bool win_1;
	bool win_2;
};

typedef struct Arguments Args;



void* thread_func(void *args) {
	Args *arg = (Args*) args;
	int id = arg->id;
	int num_of_throws = arg->num_of_throws;
	int sum_of_points_1 = arg->sum_of_points_1;
	int sum_of_points_2 = arg->sum_of_points_2;
	int player_1_num_1;
	int player_1_num_2;
	int player_2_num_1;
	int player_2_num_2;
	
	for (int index = 0; index < num_of_throws; index++) {
		player_1_num_1 = rand() % 6 + 1;
		player_1_num_2 = rand() % 6 + 1;
		player_2_num_1 = rand() % 6 + 1;
		player_2_num_2 = rand() % 6 + 1;
		printf("ID of thread: %d\n", id);
		printf("For the 1st player: %d and %d\n", player_1_num_1, player_1_num_2);
		printf("For the 2nd player: %d and %d\n", player_2_num_1, player_2_num_2);
		sum_of_points_1 += (player_1_num_1 + player_1_num_2);
		sum_of_points_2 += (player_2_num_1 + player_2_num_2);	
		printf("\n");	
	}
	
	if (sum_of_points_1 > sum_of_points_2) {
		arg->win_1 = true;
		arg->win_2 = false;		
	}
	else {
		arg->win_1 = false;
		arg->win_2 = true;
	}
	
	return NULL;
}

int main (int argc, char *argv[]) {
	if (argc != 7) {
		fprintf(stderr, "Usage: %s, Num_of_throws (K), Num_of_tour, Points_1, Points_2, Num_of_experiments Max_num_of_threads\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	if (atoi(argv[1]) < 0 || atoi(argv[2]) < 0 || atoi(argv[3]) < 0 || atoi(argv[4]) < 0 || atoi(argv[5]) < 0 || atoi(argv[6]) < 0) {
		fprintf(stderr, "Arguments %d, %d, %d, %d, %d, %d must be non negative\n", atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), atoi(argv[5]), atoi(argv[6]));
		exit(EXIT_FAILURE);
	}
	
	int status;
	int status_addr;
	int num_of_throws = atoi(argv[1]) - atoi(argv[2]);
	int sum_of_points_1 = atoi(argv[3]);
	int sum_of_points_2 = atoi(argv[4]);
	int num_of_experiments = atoi(argv[5]);
	size_t num_of_threads = (size_t)atoi(argv[6]);	
	bool need_continue = true;
	
	int num_of_plays = num_of_experiments;
	int wins_1 = 0;
	int wins_2 = 0;
	float result_for_the_1st;
	float result_for_the_2nd;

	srand(time(NULL));
		
	while(need_continue) {
		if (num_of_threads < num_of_experiments) {
			pthread_t *threads = (pthread_t *) calloc(num_of_threads, sizeof(pthread_t));
			if (threads == NULL) {
				fprintf(stderr, "in main: Can't allocate memory for threads\n");
				exit(EXIT_FAILURE);
			}
			//  заполняем значения аргументов
			Args args[num_of_threads];
			for (int index = 0; index < num_of_threads; index++) {
				args[index].id = index;
				args[index].num_of_throws = num_of_throws;
				args[index].sum_of_points_1 = sum_of_points_1;
				args[index].sum_of_points_2 = sum_of_points_2;	
			}
			
			//  создаем новые потоки
			for (int index = 0; index < num_of_threads; index++) {
				status = pthread_create(&threads[index], NULL, thread_func, (void *) &args[index]);
				if (status != 0) {
					fprintf(stderr, "main error: Can't create thread, status = %d\n", status);
					exit(EXIT_FAILURE);		
				}
			}
			
			//  ждем завершения
			for (int index = 0; index < num_of_threads; index++) {
				status = pthread_join(threads[index], (void **) &status_addr);
				if (status != 0) {
					fprintf(stderr, "main error: Can't join thread, status = %d\n", status);
					exit(EXIT_FAILURE);
				}
				//printf("Joined with address: %d\n", status_addr);
			}

			//  расчет количества побед у каждого игрока
			for (int index = 0; index < num_of_threads; index++) {
//				wins_1 = (args[index].win_1 == true)? (wins_1 + 1) : (wins_2 + 1);
				if (args[index].win_1) {
					++wins_1;
				}
				else {
					++wins_2;
				}
			}			

			num_of_experiments -= num_of_threads;
			free(threads);
		}
		else {
			pthread_t *threads = (pthread_t *) calloc(num_of_experiments, sizeof(pthread_t));
			if (threads == NULL) {
				fprintf(stderr, "in main: Can't allocate memory for threads\n");
				exit(EXIT_FAILURE);
			}
			//  заполняем значения аргументов
			Args args[num_of_experiments];
			for (int index = 0; index < num_of_experiments; index++) {
				args[index].id = index;
				args[index].num_of_throws = num_of_throws;
				args[index].sum_of_points_1 = sum_of_points_1;
				args[index].sum_of_points_2 = sum_of_points_2;	
			}
			
			//  создаем новые потоки
			for (int index = 0; index < num_of_experiments; index++) {
				status = pthread_create(&threads[index], NULL, thread_func, (void *) &args[index]);
				if (status != 0) {
					fprintf(stderr, "main error: Can't create thread, status = %d\n", status);
					exit(EXIT_FAILURE);		
				}
			}
			
			//  ждем завершения
			for (int index = 0; index < num_of_experiments; index++) {
				status = pthread_join(threads[index], (void **) &status_addr);
				if (status != 0) {
					fprintf(stderr, "main error: Can't join thread, status = %d\n", status);
					exit(EXIT_FAILURE);
				}
				//printf("Joined with address: %d\n", status_addr);
			}

			//  расчет количества побед у каждого игрока
			for (int index = 0; index < num_of_experiments; index++) {
				//wins_1 = (args[index].win_1 == true)? (wins_1 + 1) : (wins_2 + 1);
				if (args[index].win_1) {
					++wins_1;
				}
				else {
					++wins_2;
				}
			}	

			free(threads);
			need_continue = false;
		}
	}

	//  подсчет шансов
	result_for_the_1st = (float)wins_1 / (float)num_of_plays;
//	result_for_the_2nd = (float)wins_2 / (float)num_of_plays;
	result_for_the_2nd = 1.0 - result_for_the_1st;
	printf("Chances of the 1st player: %.2f\n", result_for_the_1st);
	printf("Chances of the 2nd player: %.2f\n", result_for_the_2nd);	

	return 0;
}
