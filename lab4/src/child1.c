#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <string.h>

#include "memwork.h"

int main(int argc, char **argv) {
	//открываем от потомка 1
	int fd = shm_open(BACKING_FILE_0, O_RDWR, FILE_MODE);
    if (fd < 0) { //  в случае успеха: возврат неотрицательного дескриптора
    	perror("SHM_OPEN_child1_1");
    	exit(EXIT_FAILURE);
    }

	//открываем для потомка 2
    int map_fd = shm_open(BACKING_FILE_1, O_RDWR | O_CREAT, FILE_MODE);
    if (map_fd < 0) {
    	perror("SHM_OPEN_child1_2");
	    exit(EXIT_FAILURE);
    }
  	
  	struct stat statbuf;
    fstat(fd, &statbuf);
    const size_t map_size = statbuf.st_size;
    caddr_t mem_ptr = mmap(NULL, map_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mem_ptr == MAP_FAILED) {
    	perror("MMAP");
	    exit(EXIT_FAILURE);
    }
  
  	sem_t *sem_ptr = sem_open(SEMAPHORE, FILE_MODE, 2);
    if (sem_ptr == SEM_FAILED) {
	    perror("SEM_OPEN_child1");
	    exit(EXIT_FAILURE);
    }
    if (sem_wait(sem_ptr) != 0) {
	    perror("SEM_WAIT_child1");
	    exit(EXIT_FAILURE);
    }

    char *str_in = (char *) malloc(map_size * sizeof(char));
	for (int index = 0; index < map_size; ++index) {
		str_in[index] = mem_ptr[index];
	}
	
	if (ftruncate(map_fd, (off_t)map_size) == -1) {
    	perror("FTRUNCATE_child1");
    	exit(EXIT_FAILURE);
    }
	
	caddr_t mem_ptr_1 = mmap(NULL, map_size, PROT_READ | PROT_WRITE, MAP_SHARED, map_fd, 0);
	if (mem_ptr == MAP_FAILED) {
		perror("MMAP");
		exit(EXIT_FAILURE);		
	}

	char *str_out = (char *) malloc(map_size * sizeof(char));
    for (int index = 0; index < map_size; ++index) {
		if (str_in[index] == ' ') {
			str_out[index] = '_';
		}
		else {
			str_out[index] = str_in[index];
		}
    }
    memset(mem_ptr_1, '\0', map_size);
    sprintf(mem_ptr_1, "%s", str_out);
    printf("%s", mem_ptr_1);
    free(str_in);
    free(str_out);
    close(fd);
    close(map_fd);
    munmap(mem_ptr, map_size);
    munmap(mem_ptr_1, map_size);
    shm_unlink(BACKING_FILE);
    shm_unlink(BACKING_FILE_0);
    shm_unlink(BACKING_FILE_1);
    sem_post(sem_ptr);
    sem_close(sem_ptr);
    return EXIT_SUCCESS;
}
