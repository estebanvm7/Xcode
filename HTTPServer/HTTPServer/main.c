//
//  main.c
//  HTTPServer
//
//  Created by Esteban Vargas Mora on 31/3/15.
//  Copyright (c) 2015 Esteban Vargas Mora. All rights reserved.
//

//#include <sys/types.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
//#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "Cola.h"
#include "threadpool.h"

#define Buffer_Size 100

const char* hola = "Esteban Vargas Mora :)"; //texto a imprimir ############### creado por el hilo

//char method[largoBuffer];
//char url[largoBuffer];
//buffer[n] = '\0';
//sscanf(buffer, "%s %s", method, url);
//int escritos;
//if(strcmp(method, "GET") == 0){
//    escritos = write(connectFD, respuesta, strlen(respuesta));
//} else{
//    escritos = write(connectFD, error501, strlen(error501)); }


struct s_request{
    int CFileDescriptor;
    char* dirrecion;
    char* metodo;
};
typedef struct s_request* t_request;

void catch_ctrlc() {
    
    //liberar la memoria
    /*
     El pop de la cola libera la memoria
     */
    //close(SocketFD); se repite con cada elemento de la cola
}

void doWork(void* info){

    t_request rTemp = (void*)info;
    
    if(strcmp(rTemp->metodo, "GET") == 0){
        write(rTemp->CFileDescriptor, hola, strlen(hola));
    } else {
        write(rTemp->CFileDescriptor, "Error 501", 9);
    }
    
}

pthread_mutex_t mutex_lock = PTHREAD_MUTEX_INITIALIZER;

int main(void) {
    
    //t_cola cola = nuevaCola();                                        // Cola
    char buffer [100];                                                  //Buffer
    struct threadpool *pool;
    pool = threadpool_init(5);
    
    
    struct sockaddr_in stSockAddr;
    int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    
    if(SocketFD == -1) {
        perror("can not create socket");
        exit(EXIT_FAILURE);
    }
    
    memset(&stSockAddr, 0, sizeof(stSockAddr));
    
    stSockAddr.sin_family = AF_INET;
    stSockAddr.sin_port = htons(8080);
    stSockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    if(bind(SocketFD,(struct sockaddr *)&stSockAddr, sizeof(stSockAddr)) == -1) {
        perror("error bind failed");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }
    
    if(listen(SocketFD, 10) == -1) {                                    //listen
        perror("error listen failed");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }
    
//    if (signal(SIGINT, catch_ctrlc) == SIG_ERR) {
//        t_request temp = (t_request)pop(cola);
//        int tempFD = temp->CFileDescriptor;
//        close(tempFD);
//        
//        //fputs("An error occurred while setting a signal handler.\n", stderr);
//        return EXIT_FAILURE;
//    }
    
//    if(signal(SIGINT, catch_ctrlc)){ //############## activarlo cuando se tenga la lista implementada
//    
//        printf("se activó\n");
//    
//    }
    
    for(;;) {
        
        pthread_mutex_lock(&mutex_lock);
        
        int ConnectFD = accept(SocketFD, NULL, NULL);
        
        if(ConnectFD < 0) {
            perror("error accept failed");
            close(SocketFD);
            exit(EXIT_FAILURE);
        }
        
        read(ConnectFD, buffer, sizeof(buffer)-1);                       //lee el request
        
        printf("buffer: %s\n", buffer);
        
        //# parsea la primer linea del request #
        char method[5];
        char url[100];
        buffer[99] = '\0';
        sscanf(buffer, "%s %s", method, url);
        //# parsea la primer linea del request #
        
        //# crea el struct s_request #
        t_request temp = (t_request) malloc(sizeof(struct s_request));
        temp->CFileDescriptor = ConnectFD;
        temp->dirrecion = url;
        temp->metodo = method;
        //# crea el struct s_request #
        
        if(threadpool_add_task(pool, doWork, temp, 1)){
            printf("Error in Pool_Task\n");
        } else {
            printf("Pool Task Added\n");
        }
        
//        //# push a la cola #
//        push((void*) temp, cola);
//        printf("Push correcto\n");
//        //# push a la cola #
//        
//        //# prueba de la cola #
//        t_request rTemp = (void*)pop(cola);
//        printf("direccion: %s\nMetodo: %s\n", rTemp->dirrecion, rTemp->metodo);
//        //# prueba de la cola #
        
        /*
         hilos
         */
        
//        const char* hola = "Esteban Vargas Mora :)"; //texto a imprimir ############### creado por el hilo
//        
//        write(ConnectFD, hola, strlen(hola)); //envía a buscador - creado por el hilo
        
        if (shutdown(ConnectFD, SHUT_RDWR) == 1) {
            perror("can not shutdown socket");
            close(ConnectFD);
            close(SocketFD);
            exit(EXIT_FAILURE);
        }
        pthread_mutex_unlock(&mutex_lock);
        close(ConnectFD);
    }
    
//    close(SocketFD);
//    return EXIT_SUCCESS;
}
