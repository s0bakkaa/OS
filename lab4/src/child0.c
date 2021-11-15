#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "memwork.h"

int main(int argc, char **argv) {
	// открываем родительское
	int fd = shm_open(BACKING_FILE, O_RDWR, FILE_MODE);
    if (fd < 0) { //  в случае успеха: возврат неотрицательного дескриптора
    	perror("SHM_OPEN_child0_1");
    	exit(EXIT_FAILURE);
    }

	// открываем для потомка 0
	int map_fd = shm_open(BACKING_FILE_0, O_RDWR | O_CREAT, FILE_MODE);
	if (map_fd < 0) {
		perror("SHM_OPEN_child0_2");
		exit(EXIT_FAILURE);
	}
	
	struct stat statbuf;
	//fstat(map_fd, &statbuf);
	fstat(fd, &statbuf);
	const size_t map_size = statbuf.st_size;
	caddr_t mem_ptr = mmap(NULL, map_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (mem_ptr == MAP_FAILED) {
		perror("MMAP");
		exit(EXIT_FAILURE);		
	}
	
	sem_t *sem_ptr = sem_open(SEMAPHORE, FILE_MODE, 2);
	if (sem_ptr == SEM_FAILED) {
		perror("SEM_OPEN");
		exit(EXIT_FAILURE);
	}
	if (sem_wait(sem_ptr) != 0) {
		perror("SEM_WAIT");
		exit(EXIT_FAILURE);
	}
	
	char *str_in = (char *) malloc(map_size * sizeof(char));
	for (int index = 0; index < map_size; ++index) {
		str_in[index] = mem_ptr[index];
	}
	
	if (ftruncate(map_fd, (off_t)map_size) == -1) {
    	perror("FTRUNCATE_child0");
    	exit(EXIT_FAILURE);
    }
	
	caddr_t mem_ptr_0 = mmap(NULL, map_size, PROT_READ | PROT_WRITE, MAP_SHARED, map_fd, 0);
	if (mem_ptr == MAP_FAILED) {
		perror("MMAP");
		exit(EXIT_FAILURE);		
	}

	char *str_out = (char *) malloc(map_size * sizeof(char));
	for (int index = 0; index < map_size; ++index) {
		str_out[index] = tolower(str_in[index]);
	}
	
	memset(mem_ptr_0, '\0', map_size);
	sprintf(mem_ptr_0, "%s", str_out);
	free(str_in);
	free(str_out);
	pid_t pid = fork();
	if (pid == 0) {
		munmap(mem_ptr, map_size);
		close(fd);
		//close(map_fd);
		shm_unlink(BACKING_FILE);
		//shm_unlink(BACKING_FILE_0);
		//sem_close(sem_ptr);
		execl("child1", "child1", NULL);
		perror("EXECL");
		exit(EXIT_FAILURE);
	}
	else if (pid == -1) {
		perror("FORK");
		exit(EXIT_FAILURE);
	}
	return EXIT_SUCCESS;
}
