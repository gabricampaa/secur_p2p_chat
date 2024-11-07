#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "miaLibVarie.h"
#include "RSACommLib.h"

#define PORT 51810 // The port on which our connections happen

char pathToHostPrivateKey[256];
pthread_mutex_t buffer_mutex;
pthread_cond_t message_cond;

void sending(char *TargetIp, char *pathToPeerPubKey);
void receiving(int server_fd);
void *receive_thread(void *server_fd);

int secureP2Pchat_simone(char* TargetIp, char* peer_s_pubKeyPath, char* pathToHostPrivKey) {
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    char *workingDirectory = cwd;
    const char *fileChiave = concatenateStrings(workingDirectory, ".secrets/public_key.pem");

    snprintf(pathToHostPrivateKey, sizeof(pathToHostPrivateKey), "%s", pathToHostPrivKey);

    char* publicIP = getHostIp(); // This so you can tell this to your buddy
    printf("Public IPv4 address of this machine: %s\n\n", publicIP);
    free(publicIP); // Free memory allocated for publicIP

    int server_fd;
    struct sockaddr_in address;

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Attach socket to the port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Print server IP and port
    printf("IP address is: %s\n", inet_ntoa(address.sin_addr));
    printf("Port is: %d\n", (int)ntohs(address.sin_port));

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 5) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // Initialize mutex and condition variable
    pthread_mutex_init(&buffer_mutex, NULL);
    pthread_cond_init(&message_cond, NULL);

    pthread_t tid;
    pthread_create(&tid, NULL, &receive_thread, &server_fd); // Create thread to continuously receive messages

    printf("\n*****At any point in time press the following:*****\n1. Send message\n0. Quit\n");

    int ch;
    do {
        printf("\nEnter choice: ");
        scanf("%d", &ch);
        getchar(); // consume newline character after scanf
        switch (ch) {
            case 1:
                sending(TargetIp, peer_s_pubKeyPath);
                break;
            case 0:
                printf("\nLeaving\n");
                break;
            default:
                printf("\nWrong choice\n");
        }
    } while (ch);

    close(server_fd);
    pthread_mutex_destroy(&buffer_mutex);
    pthread_cond_destroy(&message_cond);

    return 0;
}

void sending(char* TargetIp, char *pathToPeerPubKey) {
    char buffer[2000] = {0};
    char hello[1024] = {0};
    int sock = 0;

    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(TargetIp);
    serv_addr.sin_port = htons(PORT);

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return;
    }

    fflush(stdout); // Ensure immediate output

    printf("Enter your message: ");
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

    send(sock, ciphertext, strlen(ciphertext), 0);
    printf("\nMessage sent.\n");

    close(sock);
    fflush(stdout); // Ensure immediate output
}

void *receive_thread(void *server_fd) {
    int s_fd = *((int *)server_fd);
    while (1) {
        sleep(2);
        receiving(s_fd);
    }
}

void receiving(int server_fd) {
    printf("\n");
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    int client_socket;

    if ((client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    fflush(stdout); // Ensure immediate output

    char buffer[2048] = {0};
    char testoDecifato[2048];

    int valread = recv(client_socket, buffer, sizeof(buffer), 0);
    if (valread == -1) {
        perror("Error receiving message");
        close(client_socket);
        return;
    }
    RSACommLib_Decrypt(pathToHostPrivateKey, buffer, testoDecifato);

    printf("\n\n\n-----START RECEIVING MESSAGE-----\n\n%s\n\n----END----", testoDecifato);

    close(client_socket);
    fflush(stdout); // Ensure immediate output
}
