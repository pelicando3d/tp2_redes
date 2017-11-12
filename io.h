#ifndef IO_H
#define IO_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// Read arguments from client command line and print info
void readArgsFromCommandLineToServer(int argc, char *argv[], int *port, int *buffer, int *window);

// Read arguments from server command line and print info
void readArgsFromCommandLineToClient(int argc, char *argv[], char **host, int *port, char **fileName, int *buffer, int *window);

#endif
