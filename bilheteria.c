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
#include <stdbool.h>


#define POLTRONAS_TOTAIS 40
#define HORA_INICIO 7
#define HORA_FIM 21
#define HORAS_TOTAIS 15 // "Deve haver ônibus de hora em hora. Cada um com 40 lugares."


typedef struct{
	int onibus[HORAS_TOTAIS][POLTRONAS_TOTAIS];
	sem_t semaphore;
	pthread_mutex_t mutex;
	int quantidade_passageiros;
} shared_t;

shared_t shared_data;

typedef struct{
	int N; // Id passageiro
	int P; // Nº Poltrona
	int H; // Hora
} passageiro_t;


bool	  reservar_passagem( int, int );
int		* verificar_poltronas_disponiveis( int );
void	* GerarPassageiros( void * );
void	* EscolherHorarioPoltrona( passageiro_t * );
void	* ImprimeMensagem( passageiro_t, bool );


/* DEBUG*/
//int reservados = 0, nao_conseguiu = 0;


int main(int argc, char *argv[]) {
	
	if (argc < 2) {
        printf("Erro: Voce deve fornecer a quantidade de passageiros como argumento.\n");
        return 1; // Encerrar o programa com código de erro
    }

    int quantidade_passageiros = atoi(argv[1]);
    
    srand(time(0));
	
	sem_t semaphore;
	sem_init(&semaphore,0,0);
	
	// Inicializando struct compartilhada
	int poltrona = 0;
	int l,c;
	for(l = 0; l < HORAS_TOTAIS; l++){
		for(c = 0; c < POLTRONAS_TOTAIS; c++){
			shared_data.onibus[l][c] = ++poltrona; // "Poltronas devem ser numeradas de 1 a quantidade max de poltronas"
		}
		poltrona = 0;
	}
	
	shared_data.quantidade_passageiros = quantidade_passageiros;
	shared_data.semaphore = &semaphore;
	pthread_mutex_init(&shared_data.mutex, NULL);
	//$end
	
	
	
/* "Essa thread deverá gerar continuamente passageiros, os quais deverão escolher um
horário de ônibus e uma poltrona" */
	pthread_t GeradoraPassageiros;
	pthread_create(&GeradoraPassageiros,NULL,GerarPassageiros,NULL);
	
/* Encerrando programa */
	pthread_join(GeradoraPassageiros,NULL);
	sem_destroy(&semaphore);
	pthread_mutex_destroy(&shared_data.mutex);
	return 0;
}



/**************
	FUNÇÕES
***************/
void * GerarPassageiros(void * thread_data){
	int i;
    for(i = 0; i <= shared_data.quantidade_passageiros; i++){
    	passageiro_t passageiro_data;
		passageiro_data.N = i; // "Passageiros devem ser numerados de 0 à quantidade máxima de passageiros"
		//printf("\nVez de: %d",passageiro_data.N);
		
		EscolherHorarioPoltrona(&passageiro_data);
		ImprimeMensagem(passageiro_data,reservar_passagem(passageiro_data.H,passageiro_data.P));
		
		/*
		if(i == shared_data.quantidade_passageiros){
			printf("\nTotal: reservados - %d || nao conseguiu - %d\n",reservados,nao_conseguiu);
		}
		*/
	}
    
    return NULL;
}

void * EscolherHorarioPoltrona(passageiro_t * passageiro_data){
	
	passageiro_data->H = (rand() % HORAS_TOTAIS + 1) + HORA_INICIO;
	passageiro_data->P = (rand() % POLTRONAS_TOTAIS + 1) + 1;
	
	return NULL;
}

bool reservar_passagem(int horario, int poltrona){
	pthread_mutex_lock(&shared_data.mutex);
	//printf("\nPassageiro escolheu horario %d e poltrona %d",horario,poltrona);
	
	int *poltronas = verificar_poltronas_disponiveis(horario);
	
	int i;
	for(i = 0; i < POLTRONAS_TOTAIS; i++){
		if(poltronas[i] == poltrona && poltronas[i] != 0){
			shared_data.onibus[horario - HORA_INICIO][poltrona - 1] = 0;
			pthread_mutex_unlock(&shared_data.mutex);
			return true;
		}
	}
	
	free(poltronas);
	pthread_mutex_unlock(&shared_data.mutex);
	
	return false;
}

int *verificar_poltronas_disponiveis(int horario){
	int *poltronas = (int *)malloc(POLTRONAS_TOTAIS * sizeof(int));

	int pos = horario - HORA_INICIO;
	int i;
	for(i = 0; i < POLTRONAS_TOTAIS; i++){
		poltronas[i] = shared_data.onibus[pos][i];
	}
	
	return poltronas;
}

void *ImprimeMensagem(passageiro_t passageiro_data, bool conseguiu){
	
	if(conseguiu){
		printf("\nPASSAGEIRO %d RESERVOU A POLTRONA %d DO ONIBUS PARTINDO AS %d HORAS",passageiro_data.N,passageiro_data.P,passageiro_data.H);
		//reservados += 1;
		fflush(stdout);
	}
	/*
	else{
		nao_conseguiu += 1;
	}
	*/
	
	return NULL;
}