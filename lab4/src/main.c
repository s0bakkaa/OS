#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdbool.h>
#include <string.h>

#include "memwork.h"

int main() {
	size_t map_size = 0;
	char *str_in = (char *) calloc(1, sizeof(char));
    char symbol;
    while ((symbol = getchar()) != EOF) {
    	str_in[map_size] = symbol;
		str_in = (char *) realloc(str_in, (++map_size + 1) * sizeof(char));
    }
    str_in[map_size++] = '\0';
    
    //  определяем обственно создаваемый объект разделяемой памяти для создания или открытия
    int fd = shm_open(BACKING_FILE, O_RDWR | O_CREAT, FILE_MODE);
    if (fd == -1) { //  в случае успеха: возврат неотрицательного дескриптора
    	perror("SHM_OPEN_main");
    	exit(EXIT_FAILURE);
    }

    sem_t *sem_ptr;
    //  создаем новый семафор
    //  O_CREAT - флаг, управляющий работой вызова (здесь - создание сема, если тот еще не существует)
    if ((sem_ptr = sem_open(SEMAPHORE, O_CREAT, FILE_MODE, 2)) == SEM_FAILED) {  
    	perror ("SEM_OPEN_main");
    	exit(EXIT_FAILURE);
    }
    
    //  устанавливаем длину обычного файла с файловым дескриптором fd (файл должен быть открыт для записи)
    if (ftruncate(fd, (off_t)map_size) == -1) { //  в случае успеха: возвращает 0
    	perror("FTRUNCATE_main");
    	exit(EXIT_FAILURE);
    }

    caddr_t mem_ptr;
    //  отображение map_size байтов, начиная с позиции 0 файла, определяемого fd, в память, начиная с адреса 0
    //  prot описывает желаемый режим работы памяти
    // 	map_shared - тип отражаемого объекта, в данном случае - 
    //    разделение использования отображения с другими процессами
    if ((mem_ptr = mmap(NULL, map_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED) {
    	perror("MMAP_main"); //  в случае успеха: указатель отображаемой области
    	exit(EXIT_FAILURE);    	
    }
    
    int value;
    memset(mem_ptr, '\0', map_size);
    sprintf(mem_ptr, "%s", str_in);
    free(str_in);
    if (sem_getvalue(sem_ptr, &value) != 0) {
    	perror("SEM_GETVALUE_main_1");
      	exit(EXIT_FAILURE);    	
    }
    
    while (value++ < 2) {
    	if (sem_post(sem_ptr) != 0) { // разлочивает семафор 
    		perror("SEM_POST_main");
       		exit(EXIT_FAILURE);       		
    	}
    }
    
    while (value-- > 3) {
    	if (sem_wait(sem_ptr) != 0) { //  блокировка семафора
    		perror("SEM_WAIT_main_1");
    		exit(EXIT_FAILURE);
    	}
    }
    
    int pid_0 = 0;
    if ((pid_0 = fork()) == 0) { //  если потомок
        //  освобождение отображения в адресном пространстве процесса
    	/*if (munmap(mem_ptr, map_size) == -1) {
    		perror("MUNMAP_main");
    		exit(EXIT_FAILURE);    	
    	}
    	close(fd);
    	if (sem_close(sem_ptr) != 0) {
    		perror("SEM_CLOSE_main");
    		exit(EXIT_FAILURE);       		
    	}
    	shm_unlink(BACKING_FILE);*/
    	execl("child0", "child0", NULL);
    	perror("EXECL");
        exit(EXIT_FAILURE);  	
    }
    else if (pid_0 < 0) {
    	perror("FORK_main");
    	exit(EXIT_FAILURE);  	
    }
    
    while (true) {
    	if (sem_getvalue(sem_ptr, &value) != 0) {
    		perror("SEM_GETVALUE_main_2");
			exit(EXIT_FAILURE);  	    		
    	}
    	if (value == 0) {
    		if (sem_wait(sem_ptr) == -1) {
    			perror("SEM_WAIT_main_2");
				exit(EXIT_FAILURE);  	      			
    		}
    		return EXIT_SUCCESS;
    	}
    }
}

    /*
    //  удаление объекта разделяемой памяти
    fd = shm_unlink(BACKING_FILE);
    if (fd == -1) { //  в случае успеха: возвращает 0
    	perror("MEM_UNLOCK");
    	exit(EXIT_FAILURE);
    }
    */
