#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <iostream>
#include <iomanip>
#include <vector>
#include <thread>
#include <chrono>
using namespace std;

#include<time.h>
#include <QString>
#include <QStringList>
#include <QVector>
#include <QTextStream>

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"			// numéro de port d'écoute sur le serveur
#define SERVER_NAME	 "127.0.0.1"		// N.B. numéros IP sont indiqués sur les machines


#include <compatible_socket.h>
int minii;//variable globale pour stocker la valeur de minimum reçu du serveur
int maxii;//variable globale pour stocker la valeur de maximum reçu du serveur


int main(void)
{
    SOCKET ConnectSocket= INVALID_SOCKET;
    srand(time(NULL));

    char recvbuf[DEFAULT_BUFLEN];
    int iResult;
    int recvbuflen = DEFAULT_BUFLEN;
    int portLocal = 0;

  // Initialisation de la bibliothèque de sockets – fournie pour compatibilité W/linux
    InitialiseSocketLib();

// Resolve the server address and port
    addrinfo *result = NULL;
    addrinfo hints;
    memset(&hints,0, sizeof(hints));
    iResult = getaddrinfo(SERVER_NAME, DEFAULT_PORT, &hints, &result);
    if (iResult != 0)
        ErreurFatale("Echec getaddrinfo", iResult);
    this_thread::sleep_for(chrono::milliseconds(100));

 // Create a SOCKET for connecting to server to the first address
ConnectSocket= socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ConnectSocket == INVALID_SOCKET)
         ErreurFatale("Echec creation socket");
//boucle pour essayer de ce connecter au serveur
    while (1) {


// Connect to server.
    iResult = connect(ConnectSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR){
         //ErreurFatale("Echec connect");
         cout << "server down"<<endl;
        this_thread::sleep_for(chrono::milliseconds(1000));
        continue;}
    else{
        cout << "Connexion effectuee" << endl;
        break;}
    }
     //une boucle pour attendre l'envoi de la valeur min par le serveur
    while (1) {
        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
       if (iResult > 0)
           break;
    }
    int * minimum=(int * )recvbuf;//casting de char* à int*
    minii = *minimum;
    cout<<"le nombre min choisi par le serveur : "<<minii<<endl;
     //une boucle pour attendre l'envoi de la valeur max par le serveur
    while (1) {
        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
           if (iResult > 0)
           break;
    }
    int * maximum=(int * )recvbuf;//casting de char* à int*
    maxii = *maximum;
    cout<<"le nombre max choisi par le serveur : "<<maxii<<endl;
    int nbrsend=0;//initialisation de la variable (le nombre envoyé au serveur)


    while(1){
// Send an initial buffer



// Wait for response
    iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);

    if (iResult > 0){
        //printf("Reponse recue: '%s' (%d octets)\n", recvbuf, iResult);
        if (recvbuf[0] =='s') //on commence si le message reçu =s ,du coté du serveur on a envoyer au debut le caractére s
            {
            nbrsend= rand() % (maxii - minii + 1 ) ; //choisir un nombre aléatoire entre max et min

            iResult=send(ConnectSocket,(const char *)&nbrsend,sizeof(nbrsend), 0);
            if (iResult == SOCKET_ERROR)
                 ErreurFatale("Echec send");
            }
        //si la reponse reçu = + on doit réaffecter la valeur de min et on envoi une valeur entre  max et le nouveau min
        else if (recvbuf[0]=='+'){
            minii=nbrsend;
            nbrsend=rand() % (maxii - minii + 1 ) + minii;
            cout<<nbrsend<<endl;
            iResult=send(ConnectSocket,(const char *)&nbrsend,sizeof(nbrsend) , 0);}
            if (iResult == SOCKET_ERROR)
                 ErreurFatale("Echec send");
            //si la reponse reçu = - on doit réaffecter la valeur de max et on envoi une valeur entre  min et le nouveau max
        else if (recvbuf[0]=='-'){

                maxii=nbrsend;
                nbrsend=rand() % (maxii - minii + 1 ) + minii;
                cout<<nbrsend<<endl;
                iResult=send(ConnectSocket,(const char *)&nbrsend,sizeof(nbrsend), 0);
                if (iResult == SOCKET_ERROR)
                     ErreurFatale("Echec send");
            }
            //si la reponse reçu = '=' le nombre est trouvé et on sort de la boucle
            else if(recvbuf[0]=='='){
                cout<<"nombre trouve : "<<nbrsend<<endl;
                break;
            }
            //si la reponse reçu = '#' le client à perdu et on sort de la boucle
            else if(recvbuf[0]=='#'){
                cout<<"game over : perdu"<<endl;
                break;
            }
    }
    else if (iResult == 0)
    {
        printf("Connection closed\n");
            break;}
    else
ErreurFatale("Echec recv");

    //iResult = send(ConnectSocket, sendTexte, (int)strlen(sendTexte) + 1, 0);

    //printf("Message envoye : '% s' (%i octets)\n", sendTexte, iResult);
}
// shutdown the connection since no more data will be sent
    iResult = shutdown(ConnectSocket, SHUT_RDWR);
    if (iResult == SOCKET_ERROR)
         ErreurFatale("Echec shutdown");

// cleanup
closesocket(ConnectSocket);
    CloseLibrary();
    cout << endl;
    return 0;
}
