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
    int map_fd = shm_open(BACKING_FILE, O_RDWR, FILE_MODE);
    if (map_fd < 0) {
    	perror("SHM_OPEN");
	    exit(EXIT_FAILURE);
    }
  	
  	struct stat statbuf;
    fstat(map_fd, &statbuf);
    const size_t map_size = statbuf.st_size;
    caddr_t mem_ptr = mmap(NULL, map_size, PROT_READ | PROT_WRITE, MAP_SHARED, map_fd, 0);
    if (mem_ptr == MAP_FAILED) {
    	perror("MMAP");
	    exit(EXIT_FAILURE);
    }
  
  	sem_t *sem_ptr = sem_open(SEMAPHORE_NAME, O_CREAT, FILE_MODE, 2);
    if (sem_ptr == SEM_FAILED) {
	    perror("SEM_OPEN");
	    exit(EXIT_FAILURE);
    }
    if (sem_wait(sem_ptr) != 0) {
	    perror("SEM_WAIT");
	    exit(EXIT_FAILURE);
    }
    
    char *str_out = (char *) calloc(1, sizeof(char));
    size_t m_size = 0;
    for (int index = 0; index < map_size; ++index) {
		if (mem_ptr[index] == ' ') {
			str_out[index] = '_';
		}
		else {
			str_out[index] = mem_ptr[index];
		}
    }
    memset(mem_ptr, '\0', map_size);
    sprintf(mem_ptr, "%s", str_out);
    free(str_out);
    close(map_fd);
    usleep(00150000);
    sem_post(sem_ptr);
    sem_close(sem_ptr);
    return EXIT_SUCCESS;
}
