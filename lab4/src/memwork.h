#ifndef SRC__MEMWORK_H_
#define SRC__MEMWORK_H_

#include <fcntl.h>

const char *BACKING_FILE = "lab4.back";
const char *SEMAPHORE_NAME = "lab4.semaphore";
unsigned FILE_MODE = S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH; //  всегда должен быть указан для O_CREAT

#endif //SRC__MEMWORK_H_
