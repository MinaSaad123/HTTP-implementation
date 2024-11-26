//===========================<Include LIBs>
#include "Server_APIs.h"
//=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*==*=*=*=*=*=*=*=*=*=*=*=*=//
//                                         MACROS                                               //
//=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*==*=*=*=*=*=*=*=*=*=*=*=*=//
#define httpProtocol 8080
#define NumOfBacklog 10
//================================================================================================

//=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*==*=*=*=*=*=*=*=*=*=*=*=*=//
//                                       MAIN Function                                          //
//=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*==*=*=*=*=*=*=*=*=*=*=*=*=//

/*
||  My Server  ||

Supports:)
1- If the requested resource is a directory: list its contents 
2- If the requested resource is a regular file: cat its contents.
3- If the requested resource is a CGI file: run it.
4- If the requested resource does not exist: form the proper HTTP error message.
*/
void main(int argc, char** argv)
{
    int Sfd, Cfd;
    struct sockaddr_in Server;

    if ( argc < 2 )
    {
        fprintf(stderr, "[ERROR] Follow convention: (program name)  (IP Address)");
        exit(EXIT_FAILURE);
    }

    //1- Create Server socket.
    if ( ( Sfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP) ) == 0)
    {
        perror("[ERROR] Socket");
        exit(EXIT_FAILURE);

    }
    //=======================================================================================

    //2- Passing details (Ip address and port) to socket by bind system call.
    /* 1.*/
    Server.sin_family = AF_INET;
      
    /* 2.*/
    if ( inet_pton(Server.sin_family, argv[1], &Server.sin_addr) < 1 )
    {
        perror("[ERROR] inet_pton");
        exit(EXIT_FAILURE);
    }

    /* 3.*/
    Server.sin_port = htons(httpProtocol);

    /* 4.*/
    if ( bind(Sfd, (struct sockaddr*)&Server, sizeof(Server)) == -1 )
    {
        perror("[ERROR] bind");
        exit(EXIT_FAILURE);
    }
    //=======================================================================================       

    //3- Set socket to be in listen mode.
    if ( listen(Sfd, NumOfBacklog) == -1)
    {
        perror("[ERROR] listen");
        exit(EXIT_FAILURE);
    }
    //=======================================================================================       

    //4- Ready to communicate with clients, Now in waiting state until connection establish between Server and client.
    while (1)
    {
        int ID = 0;

        if ( ( Cfd = accept(Sfd, NULL, NULL) ) < 0 )
        {
            perror("[ERROR] accept");
            continue;
        } 

        ID = fork();

        if (ID == -1)
        {
            perror("[ERROR] fork");
            exit(EXIT_FAILURE);
        }

        if (ID == 0)
        {   

            while(1)
            {
                Handle_Client_Request(Cfd);
            }

        } else if ( ID > 0 )
        {
            close(Cfd);
        }
    }   
}
//===================================================================
//(_   _)( )                   ( )       ( )   ( )               ( )
//  | |  | |__    ____   ___   | |/ )     \ \_/ /  ___    _   _  | |
//  | |  |  _  \ / _  )/  _  \ | | <        \ /   /   \  ( ) ( ) | |
//  | |  | | | |( (_| || ( ) | | |\ \       | |  ( (_) ) | (_) | | |
//  (_)  (_) (_) \___ )(_) (_) (_) (_)      (_)   \___/   \___/  (_)