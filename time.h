
#ifndef TIME_H
#define TIME_H

#include "janela.h"
#include <sys/time.h>
	
//Obtem um certo tempo do relogio do computador e retorna em segundos
double obtemTempo();

//Define o tempo timeout
void temporizador(int segundos);

#endif
