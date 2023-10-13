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

void *reservar_passagem(void*);
int *verificar_poltronas_disponiveis(void *, int); /* NÚMERO DA POLTRONA 0 = POLTRONA INDISPONÍVEL */
void *GerarPassageiros(void*); 

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
	
	if (argc < 2) {
        printf("Erro: Voce deve fornecer a quantidade de passageiros como argumento.\n");
        return 1; // Encerrar o programa com código de erro
    }

    int quantidade_passageiros = atoi(argv[1]);

    printf("Iniciando o programa com %d passageiros...\n", quantidade_passageiros);
    
    srand(time(0));
	
	sem_t semaphore;
	sem_init(&semaphore,0,1);
	
	// Inicializando struct compartilhada
	shared_t shared_data;
	int poltrona = 0;
	int l,c;
	for(l = 0; l < HORAS_TOTAIS; l++){
		for(c = 0; c < POLTRONAS_TOTAIS; c++){
			shared_data.onibus[l][c] = ++poltrona; // Poltronas devem ser numeradas de 1 a quantidade max de poltronas
		}
		poltrona = 0;
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



/**************
	FUNÇÕES
***************/
void * GerarPassageiros(void * thread_data){
	shared_t *shared = (shared_t*) thread_data;
	
	passageiro_t passageiro_data[shared->quantidade_passageiros];
	
	int i;
    for(i = 0; i < shared->quantidade_passageiros; i++){
    	passageiro_data[i].shared_data = shared;	// A struct de dados compartilhados serão compartilhados entre todos
		passageiro_data[i].N = i;	// Passageiros devem ser numerados de 0 à quantidade máxima de passageiros
		passageiro_data[i].reserva_feita = 0;
		
		reservar_passagem(&passageiro_data[i]);
	    if(passageiro_data[i].reserva_feita){
	    	printf("\nPASSAGEIRO %d RESERVOU A POLTRONA %d DO ONIBUS PARTINDO AS %d HORAS",passageiro_data[i].N,passageiro_data[i].P,passageiro_data[i].H);
		}
		else{
			printf("\nPASSAGEIRO %d NAO CONSEGUIU RESERVAR",passageiro_data[i].N);
		}
	}
    
    return NULL;
}

void* reservar_passagem(void* thread_data){
	passageiro_t *passageiro_data = (passageiro_t*) thread_data;
	sem_t semaphore = passageiro_data->shared_data->semaphore;
	
	// Gerando escolha aleatória de horário e poltrona
	
	passageiro_data->H = (rand() % HORAS_TOTAIS) + HORA_INICIO;
	passageiro_data->P = (rand() % POLTRONAS_TOTAIS) + 1;
	
	sem_wait(semaphore);
	
	int * poltronas = verificar_poltronas_disponiveis(&passageiro_data->shared_data,passageiro_data->H - HORA_INICIO);
	
	int i;
	for(i = 0; i < POLTRONAS_TOTAIS; i++){
		int poltrona = poltronas[i];
		if(poltrona == passageiro_data->P && poltrona != 0){
			printf("\nConseguiu poltrona");
			passageiro_data->reserva_feita = 1;
			passageiro_data->shared_data->onibus[passageiro_data->H - HORA_INICIO][passageiro_data->P] = 0;
			break;
		}
	}
	
	free(poltronas);
	
	sem_post(semaphore);
	
	return NULL;
}

int *verificar_poltronas_disponiveis(void * thread_data, int pos){
	shared_t *shared_data = (shared_t*) thread_data;
	
	int *poltronas = (int *)malloc(POLTRONAS_TOTAIS * sizeof(int));
	
	printf("\nVerificando poltronas disponíveis\n");

	int i;
	for(i = 0; i < POLTRONAS_TOTAIS; i++){
		poltronas[i] = shared_data->onibus[pos][i];
		printf("%d ",poltronas[i]);
	}
	return poltronas;
}