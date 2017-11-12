// It controls the input/output of the file which will be transfered
#include "io.h"

void readArgsFromCommandLineToServer(int argc, char *argv[], int *port, int *buffer, int *window){

    if(argc<4){
        printf("Error: Wrong arguments!\n");
        printf("./server <port> <buffer> <window>\n");
        exit(1);
    }

    else{
	    *port=atoi(argv[1]);
	    *buffer=atoi(argv[2]);
	    *window=atoi(argv[3]);
	
		//Clear the screen and print some info
    	printf("\e[2J\e[H"); 
    	printf("## Server ##\n");
    	printf("port: %d buffer: %d window: %d\n\n", *port, *buffer, *window);
    }
}

void readArgsFromCommandLineToClient(int argc, char *argv[], char **host, int *port, char **fileName, int *buffer, int *window){

   if(argc<6){
    	printf("Erro: Digite os parâmetros corretos!\n");
        printf("./cliente <host_do_servidor> <porto_servidor> <nome_arquivo> <tam_buffer> <tam_janela>\n");
        exit(1);
    }    

    else{
    	*host=(char*)malloc(sizeof(char)*strlen(argv[1]));
    	strcpy(*host, argv[1]);
   		*port=atoi(argv[2]);
   		*fileName=(char*)malloc(sizeof(char)*(strlen(argv[3])+8));
   		strcpy(*fileName, argv[3]);
    	*buffer=atoi(argv[4]);
    	*window=atoi(argv[5]);
    
    	//Clear the screen and print some info
    	printf("\e[2J\e[H"); 
    	printf("## Client ##\n");
    	printf("host: %s port: %d fileName: %s buffer: %d window: %d\n\n", *host, *port, *fileName, *buffer, *window);
    }
}
