//===========================<Include LIBs>
#include "Server_APIs.h"
//=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*==*=*=*=*=*=*=*=*=*=*=*=*=//
//                                      Enumeration                                             //
//=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*==*=*=*=*=*=*=*=*=*=*=*=*=//
enum ServicesType
{
    CatRegularFile,
    ShowDirectoryFiles,
    CGI
};

typedef enum 
{
    NoErorr, Error404 = -1

}Error_Type;
//================================================================================================

//=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*==*=*=*=*=*=*=*=*=*=*=*=*=//
//                                    Global variables                                          //
//=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*==*=*=*=*=*=*=*=*=*=*=*=*=//
char Message[4070];
char Response[4070];
char* state = NULL; 
char* Type = NULL;
//================================================================================================

//=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*==*=*=*=*=*=*=*=*=*=*=*=*=//
//                                    Service Function                                          //
//=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*==*=*=*=*=*=*=*=*=*=*=*=*=//
static enum ServicesType Get_Service(char * PATH)
{
    struct stat File_Info;

    //Ensure if it is not /favicon.ico because i dont have PIC right now.
    if (strcmp("/favicon.ico", PATH) == 0)
    {
        return Error404;

    } else if (stat(PATH, &File_Info) == -1)
    {
        return Error404;
    }

    if ( S_ISREG(File_Info.st_mode))
    {
        return CatRegularFile;

    } else if ( S_ISDIR(File_Info.st_mode) )
    {
        return ShowDirectoryFiles;

    } else
    {
        return Error404;
    }
}

static char* ExecuteTheService(char* Command, char* PATH)
{
    int ID = 0;
    int pipe_fd[2];

    if (pipe(pipe_fd) == -1)
    {
        perror("[ERROR] pipe");
        exit(EXIT_FAILURE);
    }

    ID = fork();

    if (ID > 0) /* in parent*/
    {
        int state;
        int PID;
        int MessageSize = 0;

        MessageSize = read(pipe_fd[0], Message, sizeof(Message) - 1);
        if (MessageSize == -1)
        {
            perror("[ERROR] read:");
            exit(EXIT_FAILURE);
        }

        //Add NULL to message
        Message[MessageSize] = '\0';

        PID = wait(&state);
        if (PID == -1)
        {
            perror("[ERROR] wait");
            exit(EXIT_FAILURE);
        }        

        close(pipe_fd[0]);
        close(pipe_fd[1]);

    } else if (ID == 0) /*in child*/
    {
        dup2(pipe_fd[1], STDOUT_FILENO);

        char CommandPath[50] = "/usr/bin/";
        strcat(CommandPath, Command);

        if (execl(CommandPath, Command, PATH, NULL) == -1)
        {
            perror("[ERROR] execl");
            exit(EXIT_FAILURE);
        }

        close(pipe_fd[0]);
        close(pipe_fd[1]);
    }
} 

static void Request_Handling(char* PATH, enum ServicesType Service)
{
    //Prepare the message
    if ( Service == CatRegularFile )
    {
        ExecuteTheService("cat",PATH);

    } else if ( Service == ShowDirectoryFiles )
    {
        ExecuteTheService("ls", PATH);

    } else if ( Service == Error404 )
    {
        strcpy(Message, "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n    <meta charset=\"UTF-8\">\n    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n    <title>Error 404</title>\n    <style>\n        body {\n            font-family: Arial, sans-serif;\n            text-align: center;\n            background-color: #f4f4f4;\n            margin: 0;\n            padding: 0;\n        }\n        .container {\n            margin-top: 100px;\n        }\n        .error-image {\n            max-width: 300px;\n            margin: 20px auto;\n        }\n        h1 {\n            font-size: 48px;\n            color: #d9534f;\n        }\n        p {\n            font-size: 18px;\n            color: #555;\n        }\n        a {\n            text-decoration: none;\n            color: #0275d8;\n            font-weight: bold;\n        }\n        a:hover {\n            text-decoration: underline;\n        }\n    </style>\n</head>\n<body>\n    <div class=\"container\">\n        <img src=\"https://via.placeholder.com/300?text=Error+404\" alt=\"Error\" class=\"error-image\">\n        <h1>Error 404</h1>\n        <p>The requested page could not be found.</p>\n        <p>Please check the URL or go back to the <a href=\"/\">homepage</a>.</p>\n    </div>\n</body>\n</html>");
    }

    //Prepare the state (404- 200)
    if ( Service == Error404 )
    {
        state = "404 Not Found";

    } else
    {
        state = "200 OK";
    }

    //Prepare the Type
    if ( Service == CGI || Service == Error404 )
    {
        Type = "html";

    } else
    {
        Type = "plain";
    }
}

static void Response_Handling()
{
    snprintf(Response, sizeof(Response),
    "HTTP/1.1 %s\r\n"
    "Content-Type: text/%s; charset=UTF-8\r\n" 
    "Content-Length: %d\r\n"
    "\r\n"
    "%s", state, Type, strlen(Message), Message);
}

//================================================================================================

//=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*==*=*=*=*=*=*=*=*=*=*=*=*=//
//                               Function implementation                                        //
//=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*==*=*=*=*=*=*=*=*=*=*=*=*=//
void Handle_Client_Request(int Cfd)
{
    /*
     * i dont care about method because i will handle one method and assume user will not wrong
       also version is not in my pirority so i will ignore
    */
    char buf[1024];
    char* PATH = NULL, *query = NULL;
    int ReadSize = 0;

    if ( ( ReadSize = read(Cfd, buf, sizeof(buf) - 1) ) == -1 )
    {
        perror("[ERROR] read");
        exit(EXIT_FAILURE);
    }

    //Put '\0' in buf
    if ( ReadSize != 0 )
    {
        buf[ReadSize] = '\0';
    }

    //Extract the path
    strtok(buf, " ");
    PATH = strtok(NULL, " ");

    //TODO: Case there is ? for CGI
    if ( (query = strchr(PATH, '?') ) != NULL )
    {
       //It is CGI 
    }

    Request_Handling(PATH, Get_Service(PATH) ); /* Handle the request */

    Response_Handling(); /* Handle the response */


    if ( write(Cfd, Response, strlen(Response)) != strlen(Response) ) /* Send message to client */
    {
        perror("[ERROR] Write");
        exit(EXIT_FAILURE);
    }

    memset(Message, 0, 4070);
    memset(Response, 0, 4070);
    state = NULL; 
    Type = NULL;

}