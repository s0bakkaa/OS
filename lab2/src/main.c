#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

int main() {
    int pipefd[2];  // child1->child2
    int pipefd_1[2];  // parent->child1
    int pipefd_2[2];  // child2->parent
    int errno = 0;
    
    pid_t pid_child1 = 0;
    pid_t pid_child2 = 0;

	// pipes: child1->child2, parent->child1, child2->parent    
	if (pipe(pipefd) == -1 || pipe(pipefd_1) == -1 || pipe(pipefd_2) == -1) {
		perror("pipe error");
		exit(EXIT_FAILURE);
	}
    if ((pid_child1 = fork()) > 0) {  // create the 1st process
        if ((pid_child2 = fork()) > 0) {  // create the 2nd process
            // parent
            char *str_in = (char *) malloc(sizeof(char) * 2);
            str_in[0] = 0;
            char c;
            while ((c = getchar()) != EOF) {
                str_in[0] += 1;
                str_in[str_in[0]] = c;
                str_in = (char *) realloc(str_in, (str_in[0] + 2) * sizeof(char));
            }
            str_in[str_in[0]] = '\0';
            errno = write(pipefd_1[1], str_in, (str_in[0] + 2) * sizeof(char));
			if (errno == -1) {
				perror ("write error");
				exit(EXIT_FAILURE);
			}
            char *str_out = (char *) malloc(sizeof(char));
            errno = read(pipefd_2[0], &str_out[0], sizeof(char));
			if (errno == -1) {
				perror("read error");
				exit(EXIT_FAILURE);				
			}
            str_out = (char *) realloc(str_out, (str_out[0] + 2) * sizeof(char));
            for (int i = 1; i < str_out[0] + 1; ++i) {
                errno = read(pipefd_2[0], &str_out[i], sizeof(char));
				if (errno == -1) {
					perror("read error");
					exit(EXIT_FAILURE);				
				}
                printf("%c", str_out[i]);
            }
            printf("\n");
            close(pipefd_2[0]);
            close(pipefd_1[1]);
            free(str_in);
            free(str_out);
        } 
        else if (pid_child2 == 0) { // child2
            fflush(stdin);
            fflush(stdout);
            char *str_in = (char *) malloc(sizeof(char));
            errno = read(pipefd[0], &str_in[0], sizeof(char));
			if (errno == -1) {
				perror("read error");
				exit(EXIT_FAILURE);					
			}
            str_in = (char *) realloc(str_in, (str_in[0] + 2) * sizeof(char));
            for (int i = 1; i < str_in[0] + 1; i++) {
                errno = read(pipefd[0], &str_in[i], sizeof(char));
				if (errno == -1) {
					perror("read error");
					exit(EXIT_FAILURE);					
				}
            }
            char *str_out = (char *) malloc(2 * sizeof(char));
            str_out[0] = str_in[0];;
            for (int i = 1; i < str_in[0]; i++) {  // ' '->'_'
                if (str_in[i] == ' ') {
                    str_out[i] = '_';
                }
                else {
                	str_out[i] = str_in[i];
                }
                str_out = (char *) realloc(str_out, (str_out[0] + 2) * sizeof(str_out));
            }
            str_out[0]++;
            str_out[str_out[0]] = '\0';
            errno = write(pipefd_2[1], str_out, (str_out[0] + 2) * (sizeof(char)));
			if (errno == -1) {
				perror ("write error");
				exit(EXIT_FAILURE);
			}
            fflush(stdout);
            close(pipefd_2[1]);
            close(pipefd[0]);
            free(str_in);
            free(str_out);
        } 
		else {
			perror("fork #2 error");
			exit(EXIT_FAILURE);
		}
    } 
    else if (pid_child1 == 0) {  // child1
        char *str_in = (char *) malloc(sizeof(char));
        errno = read(pipefd_1[0], &str_in[0], sizeof(char));
		if (errno == -1) {
			perror("read error");
			exit(EXIT_FAILURE);					
		}
        str_in = (char *) realloc(str_in, (str_in[0] + 2) * sizeof(char));
        char *str_out = (char *) malloc((str_in[0] + 2) * sizeof(char));
        str_out[0] = str_in[0];
        for (int i = 1; i < str_in[0] + 1; i++) {  // UPPER->lower
            errno = read(pipefd_1[0], &str_in[i], sizeof(char));
			if (errno == -1) {
				perror("read error");
				exit(EXIT_FAILURE);					
			}
            str_out[i] = tolower(str_in[i]);
        }
        errno = write(pipefd[1], str_out, (str_out[0] + 2) * sizeof(char));
		if (errno == -1) {
			perror("write error");
			exit(EXIT_FAILURE);
		}
        close(pipefd_1[0]);
        close(pipefd[1]);
        free(str_in);
        free(str_out);
    } 
    else {
		perror("fork #1 error");
		exit(EXIT_FAILURE);	
    }
	return EXIT_SUCCESS;
}