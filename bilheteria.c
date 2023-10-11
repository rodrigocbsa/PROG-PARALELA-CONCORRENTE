/*
Instruções =>

entrega:
apenas arquivo de código

nome:
bilheteria.c

comandos:
gcc bilheteria.c -o bilheteria -pthread
chmod +x bilheteria
./bilheteria 1000 (para uma simulação com 1000 passageiros)



2023 @ 2022105GSIS
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

void *reservar_passagem(void* thread_data);
int *verificar_poltronas_disponiveis(void* thread_data); /* VALOR 0 = POLTRONA INDISPONÍVEL */
void *GerarPassageiros(void* thread_data); 
int DeMatrizParaHorario(int pos);
int DeHorarioParaMatriz(int horario);

#define POLTRONAS_TOTAIS 40
#define HORA_INICIO 7
#define HORA_FIM 21
#define HORAS_TOTAIS 15 // Deve haver ônibus de hora em hora. Cada um com 40 lugares.

typedef struct{
	int onibus[HORAS_TOTAIS][POLTRONAS_TOTAIS];
	sem_t *semaphore;
	int quantidade_passageiros;
} shared_t;

typedef struct{
	int N; // Id passageiro
	int P; // Nº Poltrona
	int H; // Hora
	shared_t *shared_data;
	int reserva_feita; // 1 = true 0 = false
} passageiro_t;

int main(int argc, char *argv[]) {
	int quantidade_passageiros = (int) * argv[1];	
	printf("%d",quantidade_passageiros);
	
	sem_t semaphore;
	sem_init(&semaphore,0,1);
	
	// Inicializando struct compartilhada
	shared_t shared_data;
	int poltrona = 0;
	for(int l = 0; l < HORAS_TOTAIS; l++){
		for(int c = 0; c < POLTRONAS_TOTAIS; c++){
			shared_data.onibus[l][c] = poltrona++; // Poltronas devem ser numeradas de 1 a quantidade max de poltronas
		}
	}
	shared_data.quantidade_passageiros = quantidade_passageiros;
	shared_data.semaphore = &semaphore;
	
/* Essa thread deverá gerar continuamente passageiros, os quais deverão escolher um
horário de ônibus e uma poltrona */
	pthread_t GeradorPassageiros;
	pthread_create(&GeradorPassageiros,NULL,GerarPassageiros,&shared_data);
	
/* Encerrando programa */
	pthread_join(GeradorPassageiros,NULL);
	sem_destroy(&semaphore);
	return 0;
}

void * GerarPassageiros(void * thread_data){
	shared_t *shared = (shared_t*) thread_data;
	
	passageiro_t passageiro_data[shared->quantidade_passageiros];
	
    for(int i = 0; i < shared->quantidade_passageiros; i++){
    	passageiro_data[i].shared_data = shared;	// A struct de dados compartilhados serão compartilhados entre todos
		passageiro_data[i].N = i;	// Passageiros devem ser numerados de 0 à quantidade máxima de passageiros
		passageiro_data[i].reserva_feita = 0;
		
		reservar_passagem(&passageiro_data[i]);
	    if(passageiro_data[i].reserva_feita)
	    	printf("PASSAGEIRO %d RESERVOU A POLTRONA %d DO ÔNIBUS PARTINDO ÀS %d HORAS",passageiro_data->N,passageiro_data->P,passageiro_data->H);
	}
    
    return NULL;
}

void* reservar_passagem(void* thread_data){
	passageiro_t *passageiro_data = (passageiro_t*) thread_data;
	sem_t semaphore = passageiro_data->shared_data->semaphore;
	
	// Gerando escolha aleatória de horário e poltrona
	srand(time(0));
	passageiro_data->H = (rand() % (HORAS_TOTAIS + 1)) + HORA_INICIO;
	passageiro_data->P = (rand() % (POLTRONAS_TOTAIS + 1)) + 1;
	
	sem_wait(semaphore);
	
	int * poltronas_disponiveis = verificar_poltronas_disponiveis(&passageiro_data);
	int pos = 0;
	
	for(int i = 0; i < POLTRONAS_TOTAIS; i++){
		int poltrona = poltronas_disponiveis[i];
		if(poltrona == passageiro_data->P && poltrona != 0){
			passageiro_data->reserva_feita = 1;
			passageiro_data->shared_data->onibus[DeHorarioParaMatriz(passageiro_data->H)][passageiro_data->P] = 0;
			break;
		}
	}
	
	sem_post(semaphore);
}

int *verificar_poltronas_disponiveis(void * thread_data){
	passageiro_t *passageiro_data = (passageiro_t*) thread_data;
	
	int *poltronas_disponiveis = malloc(POLTRONAS_TOTAIS);
	int pos = 0;
	for(int i = 0; i < POLTRONAS_TOTAIS; i++){
		poltronas_disponiveis[i] = passageiro_data->shared_data->onibus[DeHorarioParaMatriz(passageiro_data->H)][i];
	}
	return poltronas_disponiveis;
}

int DeHorarioParaMatriz(int horario){
	return horario - HORA_INICIO;
}

int DeMatrizParaHorario(int pos){
	return pos + HORA_INICIO;
}