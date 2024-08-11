#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <time.h>

FILE* init_log();

void log_attempt(FILE* log_file, long unsigned middle, long unsigned pk);

void log_total_time(FILE* log_file, clock_t start_time);

#endif