// Serveur -  TP M1-AII 2017 //


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>
using namespace std;
#include<thread>
#include<time.h>

#include <QString>
#include <QFile>
#include <QVector>
#include <QTextStream>

#include "compatible_socket.h"




//  --- fin spécifique au TP -----------------------------------------------------------------


#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"
void threadclient(SOCKET ClientSocket, int compteclient, int A);
char recvbuf[DEFAULT_BUFLEN];
char sendbuf[DEFAULT_BUFLEN];
int iResult, iSendResult;
int recvbuflen = DEFAULT_BUFLEN;
bool test=false;
// programme principal avec fonctions de communication (extrait du tutorial MSDN)
int main(void)
{
SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;

    // Intialisation de la console
    ChangeConsoleTitle("TP Serveur");

    InitialiseSocketLib();

    // Resolve the server address and port
    addrinfo *result = NULL;
    addrinfo hints;
    memset(&hints,0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0)
        ErreurFatale("Echec getaddrinfo", iResult);


    // Create a SOCKET for connecting to server
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET)
        ErreurFatale("Echec socket");


    // Setup the TCP listening socket
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR)
        freeaddrinfo(result);           // result n'est plus utilisé

    iResult = listen(ListenSocket, SOMAXCONN);

    if (iResult == SOCKET_ERROR)
    {
        closesocket(ListenSocket);
        ErreurFatale("Echec listen");
    }
    int compteclient=0;//initialisation du compteur de nombre de clients connectées au serveur;
    // Accept a client socket
    sockaddr addrClient;
    int lenAddr = sizeof(addrClient);
    srand(time(NULL));
    int minimum=rand() % 101;// on genere un nombre min aleatoire entre 0 et 100
    int maximum=(rand() % 101 )+minimum;//on genere un nombre max aleatoire entre min et 100+min
    int A=rand() % (maximum-minimum) +minimum;//on genere le nombre à deviné entre min et max
    cout<<"nombre choisi :"<<A<<endl;
    while (true){
        ClientSocket = accept(ListenSocket, &addrClient, (socklen_t*)&lenAddr);
        if (ClientSocket == INVALID_SOCKET)
            ErreurFatale("Echec accept");
        else
        {
            char buf1[256];
            char buf2[50];
            if (!getnameinfo(&addrClient,lenAddr,buf1,sizeof(buf1),buf2,sizeof(buf2),NI_NAMEREQD| NI_NUMERICHOST))
                printf("Connection de %s:%s\n", buf1,buf2);
            else{
                compteclient++;
            cout<<"client "<<compteclient<<" connected "<<endl;
            }
        }

        iSendResult=send(ClientSocket,(const char *)&minimum,sizeof(minimum), 0);//envoyer le nombre minimum au client
        this_thread::sleep_for(chrono::milliseconds(2000));
        iSendResult=send(ClientSocket,(const char *)&maximum,sizeof(maximum), 0);//envoyer le nombre maximum au client
        // No longer need server socket
        //closesocket(ListenSocket);
        test=false;// si un client demarre apres qu'un des clients a deja trouvé il peut demmarer
                    //sa recherche comme si c'est une nouvelle partie sinon il ne peut pas demarrer sa recherche
        std::thread t(threadclient, ClientSocket,compteclient,A);// on lance le thread :threadclient

        t.detach();// on detache le thread

}

        // Receive until the peer shuts down the connection



    closesocket(ListenSocket);
    // shutdown the connection since we're done
    printf("Liberation du socket\n");
    iResult = shutdown(ClientSocket, SHUT_RDWR );
    if (iResult == SOCKET_ERROR)
        ErreurFatale("Echec shutdown");


    return 0;
}

void threadclient(SOCKET ClientSocket,int compteclient,int A)
{


      int compteur=1;//initialiser le nombre de tentative  pour chaque client
        char  msg[1]; //message à envoyer
        msg[0]='s';// initialiser à 's' par exemple pour commencer
        iSendResult=send(ClientSocket,msg,strlen(msg) + 1,0);// envoi du premier message pour demararer la recherche
        if (iSendResult == SOCKET_ERROR)
            ErreurFatale("Echec send");
        while(1){
            iResult=recv(ClientSocket,recvbuf, recvbuflen, 0); // recevoir les nombres a chaque itteration
            //if (iResult > 0)
                //printf("Reponse recue: '%s' (%d octets)\n");
             if (iResult == 0)
                printf("Connection closed\n");
            int * B=(int * )recvbuf;// casting de char* vers int*

            //printf("le nombre envoye %d",*B);
            cout<<"tentative numero :"<<compteur<<" du client :"<<compteclient<<endl;


            //printf("le nombre A %d",A);
            if (test==true){ // le test est initialement false donc ils peuvent effectué les recherches
                msg[0]='#'; //on envoi ce msg qui va etre traité de l 'autre coté et qui va interompre la recherche
                iSendResult=send(ClientSocket,msg,strlen(msg) + 1,0);
                cout<<"perdu, client:"<<compteclient<<endl;
                break;//on sort de la boucle parce qu'un des clients a deja trouvé
            }
            if (*B>A){
                msg[0]='-';
                cout<<"moins"<<endl;
            }
            else if (*B<A)
            {
                msg[0]='+';
                cout<<"plus"<<endl;}
            else {
                msg[0]='=';
                cout<<"egaux, bravo client:"<<compteclient<<"  reussi au bout de : "<<compteur<<" tentatives et le nombre choisi etait bien : "<<A<<endl;

                iSendResult=send(ClientSocket,msg,strlen(msg) + 1,0);//envoyer le caractére = pour dire qu'il a trouvé

                if (iSendResult == SOCKET_ERROR)
                    ErreurFatale("Echec send");
                test=true;//le premier client qui trouve la reponse va inverser l'etat de ce boolean donc les autres clients rentreront dans le if (test==true)
                break;
            }
            iSendResult=send(ClientSocket,msg,strlen(msg) + 1,0);//envoyer le caractére pour dire plus ou moins
            compteur++;//incrementer le compteur de nombre de tentative
            if (iSendResult == SOCKET_ERROR)
                ErreurFatale("Echec send");

        if  (iResult == 0){
            printf("Connection closed\n");
            break;
    }
         this_thread::sleep_for(chrono::milliseconds(2000));
        }
    // cleanup
    closesocket(ClientSocket);
    CloseLibrary();
    cout << endl;
}
