#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "miaLibVarie.h"
#include  "RSACommLib.h"

//gcc -o main_bb DEF_p2p_Chat_simo_cript.c librerie/RSACommLib.c librerie/miaLibVarie.c  -lssl -lcrypto -w -lcurl  -g

// gcc -o main_bb main.c librerie/RSACommLib.c librerie/miaLibVarie.c  -lssl -lcrypto -w -lcurl  -g
//#define LISTENING_PORT_RECIVING_FILES 51811
//#define PORT_FOR_SENDING_FILES 51812


int PORT = 51810;

void sending(char *TargetIp, char *pathToPeerPubKey);
void receiving(int server_fd);
void *receive_thread(void *server_fd);


//char *pathToHostPrivateKey= NULL;
char *pathToHostPrivateKey[256];


int secureP2Pchat_simone(char* TargetIp, char* peer_s_pubKeyPath, char* pathToHostPrivKey)
{

        char cwd[1024];
        getcwd(cwd, sizeof(cwd));
    	char *workingDirectory = cwd;
        const char *fileChiave = concatenateStrings(workingDirectory, ".secrets/public_key.pem");

        //pathToHostPrivateKey = pathToHostPrivKey;
        snprintf(pathToHostPrivateKey, sizeof(pathToHostPrivateKey), "%s", pathToHostPrivKey);

        char name[25];

        char* publicIP = getHostIp();
        printf("Public IPv4 address of this machine: %s\n\n", publicIP);
        free(publicIP); // Free memory allocated for publicIP



    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int k = 0;

    // Creazione del socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    // Attach del socket alla porta

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Stampa dell'indirizzo IP e della porta del server
    printf("IP address is: %s\n", inet_ntoa(address.sin_addr));
    printf("Port is: %d\n", (int)ntohs(address.sin_port));

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 5) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    int ch;
    pthread_t tid;
    pthread_create(&tid, NULL, &receive_thread, &server_fd); // Creazione del thread per continuare a ricevere i messaggi in tempo reale
    printf("\n*****At any point in time press the following:*****\n1. Send message\n0. Quit\n");
    printf("\nEnter choice:");
    do
    {
        scanf("%d", &ch);
        switch (ch)
        {
        case 1:
            sending(TargetIp,peer_s_pubKeyPath);
            break;
        case 0:
            printf("\nLeaving\n");
            break;
        default:
            printf("\nWrong choice\n");
        }
    } while (ch);

    close(server_fd);

    return 0;
}

void sending(char* TargetIp, char *pathToPeerPubKey) {
    char buffer[2000] = {0};
    int PORT_server = PORT;
    char dummy;

    int sock = 0;
    struct sockaddr_in serv_addr;
    char hello[1024] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(TargetIp);
    serv_addr.sin_port = htons(PORT_server);

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return;
    }


    printf("Enter your message: ");
    //scanf("%c", &dummy); // Clear the newline character from the buffer

    if (fgets(hello, sizeof(hello), stdin) == NULL) {
            perror("Error reading input");
            close(sock);
            return;
        }

    char ciphertext[300];
    int ciphertext_len = RSACommLib_Encrypt(pathToPeerPubKey, hello, ciphertext);
    if (ciphertext_len == -1) {
        fprintf(stderr, "Error in encrypting plaintext.\n");
        close(sock);
        return;
    }


    send(sock,ciphertext,strlen(ciphertext), 0);
    printf("\nMessage sent.\n");

    close(sock);
}



//Calling receiving every 2 seconds
void *receive_thread(void *server_fd)
{
    int s_fd = *((int *)server_fd);
    while (1)
    {
        sleep(2);
        receiving(s_fd);
    }
}


void receiving(int server_fd) {
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    int client_socket;

    if ((client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    char buffer[2048] = {0};
    char testoDecifato[2048];

    int valread = recv(client_socket, buffer, sizeof(buffer), 0);
    if (valread == -1) {
        perror("Error receiving message");
        close(client_socket);
        return;
    }
    RSACommLib_Decrypt(pathToHostPrivateKey, buffer, testoDecifato);

    printf("\n\n\n-----START RECIVING MESSASGEE-----\n\n%s \n\n\n%s\n\n----END----", buffer, testoDecifato);
    int buf_len = strlen(buffer) / 2;

    char *decrDef = malloc(2048); // Assicurati di allocare memoria per decrDef
    if (decrDef == NULL) {
        fprintf(stderr, "Error allocating memory for decrypted message.\n");
        close(client_socket);
        return;
    }

    free(decrDef); // Libera la memoria allocata per decrDef
    close(client_socket);
}
