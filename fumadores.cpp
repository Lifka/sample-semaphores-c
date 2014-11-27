/*
	* Javier Izquierdo Vera
	* javierizquierdovera@gmail.com
	* UGR
*/

/*
Se crean una serie de hebras (fumadores) las cuales requieren un elemento que el estanquero produce aleatoriamente y de forma paralela,
el estanquero ha de esperar a que los fumadores cojan el ingrediente.
*/

#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h> // incluye ”usleep(...)”
#include <stdlib.h> // incluye ”rand(...)” y ”srand”

using namespace std;

const int FUMADORES = 3; // Es la cantidad de hebras, fumadores, e ingredientes, ya que lo haremos en función a la ID

pthread_t fumador_h[FUMADORES], estanquero_h;
sem_t fumador_s[FUMADORES], estanquero_s; // Array de fumadores y el estanquero, cada id del fumador se corresponderá con la hebra y con el ingrediente

// como un retardo aleatorio de la hebra
void Fumar(){ 
	const unsigned miliseg = 100U + (rand() % 1900U); // Calcular un numero aleatorio de milisegundos (entre 1/10 y 2 segundos)
	usleep( 1000U*miliseg ); // Blooqueado durante miliseg milisegundos
}

int SacaIngrediente(){
	return rand()%FUMADORES; // Saca un ingrediente aleatorio entre los disponibles (FUMADORES)
}

void * Estanquero(void *){
	while(true){

		cout << "\nEstanquero comienza a producir..." << endl; 

		//Sacar ingrediente:
		sem_wait(&estanquero_s); // Espera a que el ingrediente se consuma, de este modo espera a que la hebra que lo requiera mantenga a la espera al estanquero hasta que vuelva a empezar
		int ing = SacaIngrediente();
		cout << "Producido ingredite -> " << ing << endl;

		sem_post(&fumador_s[ing]); // La hebra a la que le corresponde el ingrediente (id) puede continuar
	}
	return NULL;
}

void * Fumador(void * ing){
	while(true){
	
		sem_wait(&fumador_s[(int)ing]); // La hebra comienza, pero espera a que le llegue su ingrediente
		cout << "Fumador " << (int)ing << " comienza a fumar." << endl;

		sem_post(&estanquero_s); // Indica al estanquero que el ingrediente se ha consumido
		Fumar();
		
		// Termina de fumar
		cout << "Fumador " << (int)ing << " ha terminado de fumar." << endl;
	}
}

void Comienza(){
	// Iniciar semaforos
	for(int i = 0; i < FUMADORES; i++)
		sem_init(&fumador_s[i],0,0);
	sem_init(&estanquero_s,0,1);

	// Iniciar hebras
	for(int i = 0; i < FUMADORES; i++)
		pthread_create(&fumador_h[i],NULL,Fumador,(void *)(i));

	pthread_create(&estanquero_h,NULL,Estanquero,NULL);

	// Esperamos a las hebras
	for(int i = 0; i < FUMADORES; i++)
		pthread_join(fumador_h[i],NULL);
	pthread_join(estanquero_h,NULL);

	// Destruimos los semaforos
	for(int i = 0; i < FUMADORES;i++)
		sem_destroy(&fumador_s[i]);
	sem_destroy(&estanquero_s);
	
	
}


int main(){
	srand( time(NULL) ); // inicializa la semilla aleatoria
	Comienza();
	pthread_exit(NULL);
}
