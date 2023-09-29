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

void *reservar_passagem(void* thread_data);
void *verificar_poltronas_disponiveis(void* horario);
void *CheckIn(void* thread_data);

#define POLTRONAS_TOTAIS 40
#define HORA_INICIO 7
#define HORA_FIM 21
#define HORAS_TOTAIS_ONIBUS 15 // Deve haver ônibus de hora em hora. Cada um com 40 lugares.

typedef struct{
	int onibus[HORAS_TOTAIS_ONIBUS][POLTRONAS_TOTAIS];
	int horarios[HORAS_TOTAIS_ONIBUS];
} shared_t;

typedef struct{
	sem_t *semaphore;
	int N; // Id passageiro
	int P; // Nº Poltrona
	int H; // Hora
	shared_t *shared_data;
} passageiro_t;

int main(int argc, char *argv[]) {
	int quantidade_passageiros = (int) * argv[1];	
	//printf("%d",quantidade_passageiros); testar dps
	
	sem_t semaphore;
	sem_init(&semaphore,0,1);
	
	
	// Inicializando onibus e horarios
	shared_t shared_data;
	int count = HORA_INICIO;
	for(int i = 0; i <= HORAS_TOTAIS_ONIBUS; i++){
		shared_data.horarios[i] = count;
		count++;
	}
	count = 1;
	for(int l = 0; l < HORAS_TOTAIS_ONIBUS; l++){
		for(int c = 0; c < POLTRONAS_TOTAIS; c++){
			shared_data.onibus[l][c] = count;
			count++;
		}
		count = 1;
	}
	//$end
	
	
/* A thread deverá continuamente gerar passageiros */
	pthread_t GeradorPassageiros;
	passageiro_t passageiro_data[quantidade_passageiros];
	
	for(int i = 0; i < quantidade_passageiros; i++){
		passageiro_data[i].semaphore = &semaphore;	// O semáforo é compartilhado entre as threads
		passageiro_data[i].N = i; 					// Passageiros devem ser numerados de 0 à quantidade máxima de passageiros
		passageiro_data[i].shared_data = &shared_data; // A struct de dados compartilhados serão compartilhados entre todos
		pthread_create(&GeradorPassageiros,NULL,CheckIn,&passageiro_data[i]);
	}
	
	
/* Encerrando programa */
	pthread_join(GeradorPassageiros,NULL);
	sem_destroy(&semaphore);
	return 0;
}

void * CheckIn(void *thread_data){
	passageiro_t *data=(passageiro_t*)thread_data;
    sem_t *semaphore = data->semaphore;
    shared_t *shared = data->shared_data;
    
    // Simulação: assim que entrar na página, reservará um tempo para o usuário realizar as operações
    sem_wait(semaphore);
    int onibus[POLTRONAS_TOTAIS] = verificar_poltronas_disponiveis(horario_passageiro_atual); // 0 = poltrona ocupada
    bool reservado = reservar_passagem(&data);
    if(reservado){
    	printf("PASSAGEIRO %d RESERVOU A POLTRONA %d DO ÔNIBUS PARTINDO AS %d HORAS",data->N,data->P,data->H);
	}
    sem_post(semaphore);
    
    return NULL;
}

void* reservar_passagem(passageiro_t* thread_data){
	
}

void* verificar_poltronas_disponiveis(int horario){
	
}