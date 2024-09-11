# include <stdio.h>
# include <unistd.h>
# include <stdlib.h>
# include <pthread.h>
# include <semaphore.h>
int m = 3; //Cant de mozos
int n = 4; //Cant de invitados

//Semaforos a utilizar
sem_t manuchoSentado;
sem_t manuchoComiendo;
sem_t comenzalesSentados;
sem_t manuchoLanzoPregunta;
sem_t comenzalLanzaRespuesta;
sem_t mozosDisponibles;
sem_t unComensalLibre;

void servirComida(int id){ //id de invitado

        //METER A MANUCHO COMIENDO POR ACA;

        int invitado_id = 5;
        // Espera hasta que un mozo estÃ© disponible (disminuye el semÃ¡foro)
        sem_wait(&mozosDisponibles);
        // Simula el tiempo que el mozo tarda en servir al invitado
        printf("\n Mozo esta sirviendo al invitado \n");
        sleep(2); // El mozo sirve al invitado
         // El invitado comienza a comer
        printf("\n Invitado esta comiendo.\n");
        sleep(5); // Simulamos que el invitado estÃ¡ comiendo
        // El mozo ha terminado de servir al invitado y queda disponible
        printf("\n Invitado ha terminado de comer.\n");
        sem_post(&unComensalLibre); //lo libero para que este disponible para responder
        sem_post(&mozosDisponibles); // El mozo queda disponible nuevamente
}

void* levantarse(int id) {
        int invitado_id=4;
        sem_wait(&manuchoSentado);
        printf("\n Comensal se levanto de la mesa");
        sem_post(&comenzalesSentados);
        sem_post(&manuchoSentado);
}

void* sentarse(int id) {
        int invitado_id=3;
        sem_post(&comenzalesSentados);
        printf("\nInvitado se sento");
}

void* lanzar_pregunta(){
        //Manucho es el unico que lanza pregunta
        sem_wait(&manuchoComiendo);//Cuando termine de comer manucho
        printf("\nManucho: Quien consideran que es el proximo campeon del mundo???");
        sem_post(&manuchoLanzoPregunta);
}

void* lanzar_respuesta(int comenzal_id){
        sem_wait(&manuchoLanzoPregunta);//Si ya se hizo la pregunta
        sem_wait(&unComensalLibre);
        //int comenzal= comenzal_id; //ATENCION:::::Esto me da violacion de segmento revisar despues de dormir
        printf("\nComenzal: Francia tiene potencial"); //Se deberia decir el id del comenzal que responde? quedaria mas bonito? aporta algo?
        sem_post(&comenzalLanzaRespuesta);
        sem_post(&unComensalLibre);
}

void* sentarse_manucho(){
        sem_wait(&comenzalesSentados);//Necesito saber si todos los comenzales se sentaron //Quiero ese recurso //Bueno se entiende
        printf("\nManucho se sento");
        sem_post(&manuchoSentado);
}

void* enojarse(){
        //El unico que se enoja en nuestro problema es manucho Manucho se enoja cuando se lanza la respuesta
        sem_wait(&comenzalLanzaRespuesta);
        printf("\nManucho: -Se enoja por la respuesta!-");
        printf("\nManucho: -Se levanta de la mesa!-");
        sem_post(&manuchoSentado);//Manucho se levanto de mesa!
}

void* comer_manucho() { //ESTO esta mal debe comer cuando ya este atendido por un mozo, es solo para probar
        sem_wait(&manuchoSentado);
        printf("\nManucho esta comiendo");
        sem_post(&manuchoComiendo);
}


void* invitados(void* id) {
        int invitado_id = 3; //Busco el ID de cada thread
        sentarse(invitado_id);
        servirComida(invitado_id);
        lanzar_respuesta(invitado_id);
        levantarse(invitado_id);
        pthread_exit(NULL); //Thread termina su laburo
}

//Comportamiento de Manucho.
void* manucho(void* par) {
        sentarse_manucho();
        comer_manucho(); //ESte no esta terminado porque aun no esta mozos ni cuando le sirven
        lanzar_pregunta();
        enojarse();
        pthread_exit(NULL);
}

int main () {
        //Defino los threads
        pthread_t hilos_invitados[n];
        int id_invitados[n];
        pthread_t manuchot;
        //Inicializo semaforos y les defino su primer estado
        sem_init(&manuchoSentado, 0, 0); //Este semaforo se va a usar para que los mozos esperen a que marucho se siente para servir
        sem_init(&comenzalesSentados, 0 ,n); //
        sem_init(&manuchoComiendo,0,0); //No come hasta que no se sienta!
        sem_init(&manuchoLanzoPregunta,0,0);
        sem_init(&comenzalLanzaRespuesta,0,0);
        sem_init(&mozosDisponibles, 0, m);
        sem_init(&unComensalLibre,0,0);

        //Hilo para manucho
        pthread_create(&manuchot, NULL, *manucho, NULL);

        //Hilos para los invitados
        for (int i = 0; i < n; i++)
        {
                id_invitados[i] = i + 1;
                pthread_create(&hilos_invitados[i], NULL, *invitados, &id_invitados[i]);
        }

        //Esperamos a que todos los hilos terminen
        for (int i = 0; i < n; i++)
        {
                pthread_join(hilos_invitados[i], NULL);
        }

        pthread_join(manuchot,NULL);
        printf("Todos los threads deberian haber acabado.\n");
        return 0;
}
