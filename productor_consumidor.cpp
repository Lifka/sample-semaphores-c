/*
	* Javier Izquierdo Vera
	* javierizquierdovera@gmail.com
	* UGR
*/

#include <iostream>
#include <pthread.h>
#include <semaphore.h>

using namespace std;

const int NUM_ITEMS = 100; // Número de elementos que se van a producir
const int TAM_VEC = 20; // Tamaño del buffer
int buffer[TAM_VEC]; // Buffer
int primer_libre = 0; // Variable para LIFO, para leer y escribir
sem_t mutex, puede_producir, puede_consumir; // Necesita,os 3 semáforos
pthread_t h_productor, h_consumidor; // Necesitamos 3 hebras

int producir_dato(){
	static int contador = 1;
	return contador ++;
}

void * productor(void *){
	for(unsigned i = 0; i < NUM_ITEMS; i++){

		// Cada vez que produzcamos un valor, le restamos 1 a los que se pueden producir (para no saturar el buffer)
		sem_wait(&puede_producir);
		int dato = producir_dato();

		sem_wait(&mutex); // Comienza sección crítica
		buffer[primer_libre] = dato;
		primer_libre++;
		sem_post(&mutex); // Acaba sección crítica

		// Cada vez que producimos un valor, le indicamos al consumidor que tiene otro para consumir en el buffer
		sem_post(&puede_consumir);
	}

	return NULL;
}

void consumir_dato(int dato){
	sem_wait(&mutex); // Comienza sección crítica
	cout << "dato recibido: " << dato << endl;
	sem_post(&mutex); // Acaba sección crítica
}


void * consumidor(void *){
	for(unsigned i = 0; i < NUM_ITEMS; i++){
		// Cada vez que se recorre la hebra consumir, hay que restar uno de los valores que tenemos disponibles para consumir
		sem_wait(&puede_consumir);

		sem_wait(&mutex); // Comienza sección crítica
		// Sacamos el valor
		int dato = buffer[primer_libre-1];
		primer_libre--;
		sem_post(&mutex); // Acaba sección crítica

		// Consumimos el valor sacado, y por lo tanto indicamos al productor que tiene otro espacio para producir
		consumir_dato(dato);
		sem_post(&puede_producir);	
	}

	return NULL ;
}

/* ACTIVIDAD 1 */

void Espacio(){
	cout << "Están ocupadas desde la posición 0 hasta " << primer_libre << 
		" y libre desde " << primer_libre << " hasta " << TAM_VEC << endl;
}

/* ACTIVIDAD 2 y 3 */

void Comienza(){
	// Iniciamos semáforos
	sem_init(&mutex, 0, 1); // Exclusión mútua 1
	sem_init(&puede_producir, 0, TAM_VEC); // Producir TAM_VEC => Indica que tenemos todo ese espacio para producir
	sem_init(&puede_consumir, 0, 0); // Consumir 0

	pthread_create(&h_productor,NULL,productor,NULL);
	pthread_create(&h_consumidor,NULL,consumidor,NULL);
	
	pthread_join(h_consumidor,NULL);
	pthread_join(h_productor,NULL);
	cout << "Fin" << endl;

	// Cerramos
	sem_destroy(&mutex);
	sem_destroy(&puede_producir);
	sem_destroy(&puede_consumir);
}

int main(){
	Espacio();
	Comienza();
	//pthread_exit(NULL);
}
