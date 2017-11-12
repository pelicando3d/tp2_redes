//Window controller

#include "janela.h"

void criaJanela(int port, int bufferSize, int windowSize)
{

    int i;
    packageID = 0;
    lastPackageID = 0;
    windowSize = windowSize;
    currentlyWindow = 0;
    displacement = 0;
    receivedAcks = 0;
    sendedAcks = 0;
    receivedPackages = 0;
    sendedPackages = 0;
    discardedPackages = 0;
    resendedPackages = 0;

    window = (char **)malloc(sizeof(char *) * windowSize);
    for (i = 0; i < windowSize; i++)
    {
        window[i] = (char *)malloc(sizeof(char) * (TAM_CABECALHO + bufferSize));
    }
}

char checksum(char *buffer, int bufferSize)
{

    int i;
    int sum = 0;
    for (i = 0; i < bufferSize; i++)
    {
        sum += buffer[i];
        sum = sum % 128;
    }
    return sum;
}

void criaPacote(char *pacote, int id_pacote, int tipo_pacote, char *buffer, int bufferSize)
{

    /* 	   	  Formato do pacote:
	| ID_DO_PACOTE | TIPO_DO_PACOTE | CHECKSUM | DADOS      | 
	| 4Bytes       | 1Byte          | 1 Byte   | bufferSize |*/

    memcpy(pacote, &id_pacote, 4);                       //Copia 4 bytes que representa o id do pacote para o inicio do bloco do pacote
    pacote[4] = tipo_pacote;                             //Copia para a posição 4 o tipo da mensagem
    pacote[5] = checksum(buffer, bufferSize);            //copia para a posição 5 o valor checksum (inteiro de 1 byte)
    strncpy(TAM_CABECALHO + pacote, buffer, bufferSize); //copia os dados para as posições seguintes
}

int fazAberturaPassiva(int port, int bufferSize, int windowSize)
{

    tp_init();

    if ((idsocket_ = tp_socket(port)) < 0)
    {
        printf("Erro ao criar socket servidor\n");
        return -1;
    }

    criaJanela(port, bufferSize, windowSize);

    return idsocket_;
}

int fazAberturaAtiva(char *host, int port)
{

    tp_init();

    if ((idsocket_ = tp_socket(0)) < 0)
    {
        printf("Erro ao criar socket cliente\n");
        return -1;
    }

    if (tp_build_addr(&endereco_, host, port) < 0)
    {
        printf("Erro ao vincular (host,port) do servidor\n");
        return -1;
    }

    return idsocket_;
}

int recebeNomeArquivo(char *buffer, int bufferSize)
{

    char pacote_nome_arquivo[TAM_CABECALHO + bufferSize]; //cria pacote com o nome do arquivo
    int pacotes_recebidos = 0;
    pacotes_recebidos = tp_recvfrom(idsocket_, pacote_nome_arquivo, TAM_CABECALHO + bufferSize, (so_addr *)&endereco_);
    receivedPackages++;
    strncpy(buffer, TAM_CABECALHO + pacote_nome_arquivo, bufferSize); //copia para o buffer
    temporizador(1);
    return pacotes_recebidos - TAM_CABECALHO;
}

int enviaNomeArquivo(char *buffer, int bufferSize)
{

    char pacote[bufferSize + TAM_CABECALHO];
    criaPacote(pacote, packageID, TIPO_DADOS, buffer, bufferSize);

    int enviados = 0;
    enviados = tp_sendto(idsocket_, pacote, bufferSize + TAM_CABECALHO, (so_addr *)&endereco_);
    sendedPackages++;

    return enviados;
}

int idPacote(char *pacote)
{

    int id_pacote;
    memcpy(&id_pacote, &pacote[0], 4);

    return id_pacote;
}

int enviaBuffer(char *buffer, int bufferSize)
{

    int recebeuack = -1, pctsenviados = 0, id_pacote = 0;
    char pacote[TAM_CABECALHO + bufferSize];
    char pacoteack[TAM_CABECALHO + bufferSize];
    char pacotereenvio[TAM_CABECALHO + bufferSize];

    criaPacote(pacote, packageID, TIPO_DADOS, buffer, bufferSize);

    //Caso a Janela esteja CHEIA
    if (currentlyWindow == windowSize)
    {

        recebeuack = tp_recvfrom(idsocket_, pacoteack, TAM_CABECALHO + bufferSize, (so_addr *)&endereco_);

        //Servidor não recebeu nenhum ACK. É preciso reenviar todos os pacotes do buffer antes de mover a janela deslizante
        while (recebeuack == -1)
        {
            int i;
            for (i = displacement; i < windowSize; i++)
            {
                id_pacote = idPacote(window[i % windowSize]);
                criaPacote(pacotereenvio, id_pacote, TIPO_DADOS, TAM_CABECALHO + window[i], bufferSize);
                tp_sendto(idsocket_, pacotereenvio, bufferSize + TAM_CABECALHO, (so_addr *)&endereco_);
                sendedPackages++;
                resendedPackages++;
            }
            recebeuack = tp_recvfrom(idsocket_, pacoteack, TAM_CABECALHO + bufferSize, (so_addr *)&endereco_);
        }
        receivedAcks++;

        //Servidor recebe o ACK de um pacote não esperado
        if (idPacote(pacoteack) != lastPackageID)
        {
            currentlyWindow -= idPacote(pacoteack) - lastPackageID - 1;
            lastPackageID = idPacote(pacoteack);
        }

        //Desliza Janela e copia pacote para a janela
        lastPackageID++;
        strcpy(window[displacement], pacote);
        displacement = (displacement + 1) % windowSize;
    }

    //Caso a janela ainda tenha ESPAÇO LIVRE
    else
    {
        currentlyWindow++;
        strcpy(window[(currentlyWindow - 1 + displacement) % windowSize], pacote); //Copia o pacote para a primeira posição vazia da janela
    }

    //Envia o pacote para o cliente
    pctsenviados = tp_sendto(idsocket_, pacote, TAM_CABECALHO + bufferSize, (so_addr *)&endereco_);
    sendedPackages++;
    packageID++;
    temporizador(1);

    return pctsenviados;
}

int recebeBuffer(char *buffer, int bufferSize)
{

    int bytesRecebidos = 0;
    char pacote[TAM_CABECALHO + bufferSize];
    char pacoteack[TAM_CABECALHO + bufferSize];

    bytesRecebidos = tp_recvfrom(idsocket_, pacote, TAM_CABECALHO + bufferSize, (so_addr *)&endereco_);
    receivedPackages++;

    //Cliente recebeu um pacote que já foi recebido (servidor não recebeu o ACK daquele pacote)
    while (idPacote(pacote) != packageID)
    {
        criaPacote(pacoteack, idPacote(pacote), TIPO_ACK, TAM_CABECALHO + pacote, bufferSize);
        tp_sendto(idsocket_, pacote, TAM_CABECALHO + bufferSize, (so_addr *)&endereco_);
        sendedAcks++;
        //o pacote é descartado
        bytesRecebidos = tp_recvfrom(idsocket_, pacote, TAM_CABECALHO + bufferSize, (so_addr *)&endereco_);
        discardedPackages++;
    }

    //Caso o pacote recebido seja do TIPO_FINAL
    if (pacote[TAM_CABECALHO - 2] == TIPO_FINAL)
    {
        criaPacote(pacoteack, idPacote(pacote), TIPO_FINAL, TAM_CABECALHO + pacote, bufferSize);
        tp_sendto(idsocket_, pacoteack, TAM_CABECALHO + bufferSize, (so_addr *)&endereco_);
    }

    //Cliente recebe pacote esperado e envia um pacote ACK para o servidor
    else
    {
        /*	char verificaChecksum=checksum(pacote,sizeof(pacote));
    	if(pacote[TAM_CABECALHO-1]!=verificaChecksum){
			//Aqui deveria ser o tratamento de erro checksum
  		}
	*/
        criaPacote(pacoteack, idPacote(pacote), TIPO_ACK, TAM_CABECALHO + pacote, bufferSize);
        tp_sendto(idsocket_, pacoteack, TAM_CABECALHO + bufferSize, (so_addr *)&endereco_);
    }

    sendedAcks++;
    packageID++;

    //Copia a mensagem para o buffer
    strncpy(buffer, TAM_CABECALHO + pacote, bufferSize);

    return bytesRecebidos - TAM_CABECALHO;
}

int enviaFimArquivo(int bufferSize)
{

    char pacote[TAM_CABECALHO + bufferSize];

    criaPacote(pacote, packageID, TIPO_FINAL, NULL, 0);

    tp_sendto(idsocket_, pacote, TAM_CABECALHO, (so_addr *)&endereco_);
    sendedPackages++;

    do
    {
        tp_recvfrom(idsocket_, pacote, TAM_CABECALHO + bufferSize, (so_addr *)&endereco_);
        receivedAcks++;
    } while (pacote[TAM_CABECALHO - 2] != TIPO_FINAL); //Espera a confirmação do ack final

    temporizador(1);
    return 1;
}

void fechaConexaoServidor()
{

    printf("\n## INFORMACOES ##\n");
    printf("Reenvios: %d\nPacotes enviados: %d\nPacotes recebidos: %d\nPacotes descartados: %d\nACKs enviados: %d\nACKs recebidos: %d\n", resendedPackages, sendedPackages, receivedPackages, discardedPackages, sendedAcks, receivedAcks);
    close(idsocket_);
}

void fechaConexaoCliente()
{

    printf("\n## INFORMACOES ##\n");
    printf("Reenvios: %d\nPacotes enviados: %d\nPacotes recebidos: %d\nACKs enviados: %d\nACKs recebidos: %d\n", resendedPackages, sendedPackages, receivedPackages, sendedAcks, receivedAcks);
    close(idsocket_);
}
