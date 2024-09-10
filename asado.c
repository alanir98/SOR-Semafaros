# include <stdio.h>
# include <pthread.h>
# include <semaphore.h>

int m = 3;              //Cant de mozos
int n = 4;              //Cant de invitados
int platosServidos = 0; //No seria mas util un semafaro?

//Semaforos a utilizar
sem_t manuchoSentado;
sem_t manuchoComiendo;
sem_t comenzalesSentados;
sem_t manuchoLanzoPregunta;
sem_t comenzalLanzaRespuesta;

void* lanzar_pregunta()
{
        //Manucho es el unico que lanza pregunta
        sem_wait(&manuchoComiendo);//Cuando termine de comer manucho
        printf("Manucho: Quien consideran que es el proximo campeon del mundo???");
        sem_post(&manuchoLanzoPregunta);
}

void* lanzar_respuesta(int comenzal_id)
{
        sem_wait(&manuchoLanzoPregunta);//Si ya se hizo la pregunta
        //int comenzal= *(int*)comenzal_id;     //ATENCION:::::Esto me da violacion de segmento revisar despues de dormir
        printf("Comenzal: Francia tiene potencial");  //Se deberia decir el id del comenzal que responde? quedaria mas bonito? aporta algo?
        sem_post(&comenzalLanzaRespuesta);
}

void* sentarse_manucho()
{
        sem_wait(&comenzalesSentados);//Necesito saber si todos los comenzales se sentaron //Quiero ese recurso //Bueno se entiende

        printf("Manucho se sento");

        sem_post(&manuchoSentado);
}

void* enojarse()
{
        //El unico que se enoja en nuestro problema es manucho
        //Manucho se enoja cuando se lanza la respuesta
        sem_wait(&comenzalLanzaRespuesta);
        printf("Manucho: -Se enoja por la respuesta!-");
        printf("Manucho: -Se levanta de la mesa!-");
        sem_post(&manuchoSentado);//Manucho se levanto de mesa!
}

void* comer_manucho()
{//ESTO esta mal debe comer cuando ya este atendido por un mozo, es solo para probar
        sem_wait(&manuchoSentado);
        printf("Manucho esta comiendo");
        sem_post(&manuchoComiendo);
}

//Comportamiento de Manucho.
void* manucho(void* par)
{
                sentarse_manucho();
                comer_manucho();                //ESte no esta terminado porque aun no esta mozos ni cuando le sirven
                lanzar_pregunta();
                int idfalsa=4;
                lanzar_respuesta(idfalsa);      //SOLO PARA FINGIR UNA RESPUESTA!!!!!!!!! esto no va aca, la respuesta la lanza un comenzal que haya terminado de
                enojarse();

        pthread_exit(NULL);
}

int main()
{
        //Threads
        pthread_t manuchot;
        //Semaforos
        sem_init(&manuchoSentado, 0, 1);
        sem_init(&comenzalesSentados, 0 ,1);    //debe iniciar en 0 y cuando todos los comenzales esten sentados debe quedar en N
        sem_init(&manuchoComiendo,0,0);         //No come hasta que no se sienta!
        sem_init(&manuchoLanzoPregunta,0,0);
        sem_init(&comenzalLanzaRespuesta,0,0);

        pthread_create(&manuchot, NULL, *manucho, NULL);
        pthread_join(manuchot,NULL);

         return 0;
}
