#ifndef COMPATIBLE_SOCKET_H
#define COMPATIBLE_SOCKET_H



#ifdef __WIN32__                // entetes de la bibliothèque WinSock

    //#define _WIN32_WINNT  0x601
    #include <winsock2.h>
    #include <ws2tcpip.h>

    #define SHUT_RDWR 2
#else                           // entetes de socket linux
    # include <sys/socket.h>
  //  #include <arpa/inet>
    #include <unistd.h>
    #include <sys/types.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <netdb.h> /* gethostbyname, getnameinfo */
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1

typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;


    #define closesocket(s) close(s)
#endif


void InitialiseSocketLib()
{
#ifdef __WIN32__
    // Initialize Winsock
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0)
    {
        printf("WSAStartup failed: %d\n", iResult);
        exit(1);
    }

#endif

}

void CloseLibrary()
{
#ifdef __WIN32
    // Ferme Winsock
    WSACleanup();
#endif
}

void ErreurFatale(const char * MessageContexte, int err =0)
{

#ifdef __WIN32__
    if (err <=0)
        printf("%s : %d\n\n",MessageContexte,WSAGetLastError());
    else
        printf("%s : %d\n\n",MessageContexte,err);
    WSACleanup();
    exit(-1);
#else
    perror(MessageContexte);
    exit(1);
#endif
}

int numLastError()
{
#ifdef __WIN32__
    return WSAGetLastError();
#else
    return errno;
#endif
}

#endif // COMPATIBLE_SOCKET_H

void ResizeTerminal(int nligne, int nCol)
{

#ifdef __WIN32__
    // pour forcer la taille de la fenetre console (efface le contenu)
    char buf[256];
    sprintf(buf,"mode con LINES=%i COLS=%i",nligne,nCol);
            system(buf);

#else
    printf("\e[8;%i;%it",nligne,nCol);
#endif
}

void EffaceConsole()
{
#ifdef __WIN32__
    system("cls");
#else
    system("clear");
#endif
}

void ChangeConsoleTitle(const char * titre)
{
#ifdef __WIN32__
  SetConsoleTitleA(titre);

#else
    printf("%c]0;%s%c", '\033', titre, '\007');
#endif
}
