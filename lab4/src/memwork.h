#ifndef SRC__MEMWORK_H_
#define SRC__MEMWORK_H_

#include <fcntl.h>

const char *BACKING_FILE = "lab4.back";
const char *BACKING_FILE_0 = "lab4_0.back";
const char *BACKING_FILE_1 = "lab4_1.back";
const char *SEMAPHORE = "lab4.semaphore";
const char *SEMAPHORE_0 = "lab4_0.semaphore";
const char *SEMAPHORE_1 = "lab4_1.semaphore";
unsigned FILE_MODE = S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH; //  всегда должен быть указан для O_CREAT

#endif //SRC__MEMWORK_H_
