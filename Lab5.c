#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<time.h>

int N;
int* vetor;
int bloqueadas = 0;
pthread_mutex_t x_mutex;
pthread_cond_t x_cond;

void barreira() {
    pthread_mutex_lock(&x_mutex); //inicio secao critica
    if (bloqueadas == (N-1)) {
        //ultima thread a chegar na barreira
        pthread_cond_broadcast(&x_cond);
        bloqueadas=0;
    } else {
        bloqueadas++;
        pthread_cond_wait(&x_cond, &x_mutex);
    }
    pthread_mutex_unlock(&x_mutex); //fim secao critica
}

void *thread (void *arg){
    int id = (int) arg;

    int somaLocal = 0;  

    for (int i = 0; i < N; i++){
        for (int i = 0; i < N; i++){
            somaLocal += vetor[i];
        }

        barreira();

        vetor[id] = rand()*(id+1) % 10;

        barreira();
    }

    //Imprimindo resultado
    printf("Thread: %d;\tsomaLocal: %d\n", id, somaLocal);

    pthread_exit((void*)somaLocal);
}

int main(int argc, char *argv[]){
    pthread_t *tid;
    int *retorno;
    srand(time(NULL));

    pthread_mutex_init(&x_mutex, NULL);
    pthread_cond_init (&x_cond, NULL);
    
    //Check de input
    if(argc<2){
        fprintf(stderr,"A entrada deve ser: ./'nome do programa' <n de threads>\n");
        return 1;
    }

    //Armazenando input
    N = atoi(argv[1]);

    //Alocação da memória para ponteiros do programa com checks de erro
    vetor  = (int *) malloc(sizeof(int)*N);
    if (vetor == NULL){
        fprintf(stderr,"Erro-malloc\n");
        return 2;
    }

    tid = (pthread_t*) malloc(sizeof(pthread_t)*N);
    if(tid == NULL){
        fprintf(stderr,"Erro-malloc\n");
        return 2;
    }

    retorno = (int*) malloc(sizeof(int));
    if (retorno == NULL){
        fprintf(stderr,"Erro-malloc\n");
        return 2;
    }

    //Preenchendo o vetor
    for(int i = 0; i < N; i++){
        vetor[i] = rand()*(i+1) % 10; //Varia entre 0 e 10 (excluindo o 10)
    }

    //Criando as threads
    for(int i = 0; i < N; i++){
        
        if(pthread_create(tid+i,NULL,thread,(void*)i)){
            fprintf(stderr,"Erro--pthread_create\n");
            return 3;
        }
    }

    //Aguardando término das threads
    for(int i = 0; i < N; i++){
        pthread_join(*(tid+i),(void**) &retorno);

        vetor[i] = (int) retorno;
    }

    //Verificando resultados
    for(int i = 1; i < N; i++){
        if (vetor[0] != vetor[i]){
            puts("Falta de igualdade entre os resultados de cada thread.");
            return 4;
        }
    }

    puts("Confirmada igualdade entre os resultados de cada thread!");

    //Liberando variáveis
    free(vetor);
    free(tid);
    free(retorno);

    pthread_mutex_destroy(&x_mutex);
    pthread_cond_destroy(&x_cond);

    return 0;
}
