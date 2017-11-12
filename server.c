// Server side

#include "io.h"
#include "janela.h"

int main(int argc, char *argv[]){
    
    //Declaração das variaveis
    int port,bufferSize,window,readedBytes;
    char *buffer;
    FILE *file;
   
    //Processa argumentos da linha de comando
    readArgsFromCommandLineToServer(argc,argv,&port,&bufferSize,&window);
	  
    //Faz abertura passiva e aguarda conexao
    fazAberturaPassiva(port,bufferSize,window);
    printf("Waiting for a client connection...\n");
    	
    //Recebe a string com o nome do arquivo
	buffer=(char*)malloc(sizeof(char)*bufferSize);   
    if(recebeNomeArquivo(buffer,bufferSize)<0){
        printf("Error: Can't read the file.\n");
        return 0;
    }
    printf("FileName received: %s\n",buffer);

    //Abre o arquivo que vai ser lido, se der erro, fecha conexao e termina
    if((file=fopen(buffer,"r"))==NULL){
        printf("Error: File %s don't exists.\n",buffer);
        fechaConexaoServidor();     
        return 0;
    }

    //Le o arquivo, um buffer por vez ate fread retornar zero
    readedBytes=0;
    printf("Sending a file: %s\n",buffer);
    while((readedBytes=fread(buffer,1,bufferSize,file))!=0) 
        enviaBuffer(buffer,readedBytes);
        
    //Fecha arquivo e conexao
    if(enviaFimArquivo(bufferSize)==1){
    	printf("File transfered with success.\n");
    }

    free(buffer);
    fclose(file);  
    fechaConexaoServidor();
    printf("\nFinish server connection\n");

    return 0;
}
