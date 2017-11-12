// Client side
#include "io.h"
#include "time.h"
#include "janela.h"

int main(int argc, char * argv[]) {
    
    //Declaração das variaveis
    int port,bufferSize,window;
    char *buffer,*host,*fileName;
    double startTime,totalTime,kbps;
    unsigned int totalBytesReceived = 0;
    int receivedBytesz = 0;
    FILE *file;
  
    //Processa argumentos da linha de comando
    readArgsFromCommandLineToClient(argc,argv,&host,&port,&fileName,&bufferSize,&window);
     
    //Faz abertura ativa da conexão
    fazAberturaAtiva(host,port);
    printf("Connecting with server %s port %d.\n\n",host,port);

    //Chama gettimeofday para tempo inicial
    startTime=obtemTempo();

    //Envia string com nome do arquivo    
    buffer=(char*)malloc(sizeof(char)*bufferSize);
    strcpy(buffer,fileName);
    if(enviaNomeArquivo(buffer,bufferSize)<0){
		printf("Error: Wrong file name.\n");
		return 0;
    }
    printf("Send the file name.\n\n");
 
    //Abre arquivo que vai ser gravado
    strcat(fileName," (copy)"); //para nao dar conflito no diretorio
    file=fopen(fileName,"w");
   
    //Recebe buffer até que perceba que arquivo acabou
    printf("Receiving file...\n\n"); 
    while((receivedBytesz=recebeBuffer(buffer,bufferSize))>0)
        totalBytesReceived+=fwrite(buffer,1,receivedBytesz,file);

    //Fecha arquivo
    printf("File %s saved\n",fileName);
    fclose(file);
    free(buffer);
    free(host);
    free(fileName);

    //Chama gettimeofday para o tempo final e calcula tempo gasto
    totalTime=obtemTempo()-startTime;
 
    //Imprime resultado
    kbps=(totalBytesReceived/totalTime);    
    printf("\nBuffer = \%5u byte(s), \%10.2f kbps (\%u bytes em \%3.06f s)\n",bufferSize,kbps,totalBytesReceived,totalTime);

    //Fecha conexão
    fechaConexaoCliente();
    
    printf("\nFinish client connection\n");
    
    return 1;
}
