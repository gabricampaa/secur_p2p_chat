#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>

#include <curl/curl.h>

#include "librerie/configuringVpn.h"
#include "librerie/readFile.h"
#include "librerie/file_sender_new.h"
#include "librerie/miaLibVarie.h"
#include "librerie/RSACommLib.h"
#include "librerie/p2p_chat.h"


#define MAX_IP_LENGTH 18
#define PORT 51810
#define MAX_BUF_SIZE 1024

#define PATH_TRACKER_DIRECTORY "http://13.53.40.109:8080/home/ubuntu/apple/"
#define TRACKER_SERVER_IP "13.53.40.109"
#define TRACKER_LISTENING_PORT 8080


#ifdef DEBUG
    #define DEBUG_PRINT(fmt, args...)    fprintf(stderr, "\033[0;31mDEBUG: " fmt "\033[0m", ## args)
#else
    #define DEBUG_PRINT(fmt, args...)    /* Don't do anything in release builds */
#endif


char* retrieve_assigend_private_ip(const char* host, int port);  // -> from the tracker server
void download_file(const char *base_url, const char *ip_folder, const char *filename); // -> from the tracker server
void signal_handler(int signal);


int main(){

   	DEBUG_PRINT("Run with sudo!\n", cwd);
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

	char cwd[1024];
	getcwd(cwd, sizeof(cwd));
	DEBUG_PRINT("Current working directory: %s\n", cwd);

	char *pathVPNconfiguration = concatenateStrings(cwd, "/.vpn-secrets/wg0_vpn.conf");
	char *pathWireguardPrivateKey = concatenateStrings(cwd, "/.vpn-secrets/privatekey");


	//connecting to the tracker so it can assign this machine's private ip address (for the vpn connection)
	char static_assigned_private_ip[MAX_IP_LENGTH];
	char *assigned_private_ip = retrieve_assigend_private_ip(TRACKER_SERVER_IP, TRACKER_LISTENING_PORT);

 	if (assigned_private_ip == NULL) {
        fprintf(stderr, "Failed to retrieve the assigned private IP address. An error occurred, the tracker server might be down. Quitting...\n");
		free(assigned_private_ip);
			//qui assegni manualmente l'ip e vaffanculo
        return 1;
    	}

    strncpy(static_assigned_private_ip, assigned_private_ip, MAX_IP_LENGTH - 1);
    static_assigned_private_ip[MAX_IP_LENGTH - 1] = '\0'; // Ensure null-termination
	free(assigned_private_ip);
    DEBUG_PRINT("\nPrivate ip assigned by the server: %s\n", static_assigned_private_ip);


    //PRIVATE IP has been assigned from tracker, now we proceed with the configuration of the VPN and crytpo shit

	int vpnConf_errorHandler = vpnConf(cwd);
	if(vpnConf_errorHandler != 0){
	fprintf(stderr, "Error while configuring the VPN services. \n");
	return 1;
	}

	int crittografiaSetup_errorHandler = crittografiaSetup(cwd);
	if(crittografiaSetup_errorHandler != 0){
	fprintf(stderr, "Error while configuring the OPENSSL services. \n");
	return 1;
	}


	//if all goes right, we now are ready to write to the config files
	char *wireguardPrivateKey = readFile(pathWireguardPrivateKey);
	writeConfFile(pathVPNconfiguration, wireguardPrivateKey, PORT, static_assigned_private_ip);	//only this machine's interface, no peer


	char *pathHostPubKey=concatenateStrings(cwd,"/.secrets/public_key.pem");
	char *hostPubKey= readFile(pathHostPubKey);
	char *path_HostConfForPeer=concatenateStrings(cwd,"/.vpn-secrets/HostConfForPeer");

	//writing the wireguard configuration that other peer's will append in their wg config file to connect to this ip
	configurazione_Peer_per_questo_host(cwd, static_assigned_private_ip);


	//sending the files needed for the communication with the other host to the tracker.
	int send_one = sender_nuovo(path_HostConfForPeer);
	int send_two= sender_nuovo(pathHostPubKey);

	if (send_one != 0 || send_two !=0 ) {
	fprintf(stderr,"\nProblems with sending the files to the tracker\n");
	return 1;
	}

	//We have now reached THE FIRST CHECKPOINT: the tracker has our data and we've been assigned a private ip
    char peerIp[MAX_IP_LENGTH];
	printf("\nNow let's get the peer stuff! Insert the other dude's PUBLIC ipv4: ");

    if (fgets(peerIp, sizeof(peerIp), stdin) != NULL) {
            size_t len = strlen(peerIp);
            if (len > 0 && peerIp[len - 1] == '\n') {
                peerIp[len - 1] = '\0';
            }

    DEBUG_PRINT("\nConnecting to: %s\n",peerIp);


    //ON the tracker, files are stored like this: "pub_ip"-files. Let's create this path'
	char long_query[30];
	snprintf(long_query, sizeof(long_query), "%s-files", peerIp);


   /*We now start downloading the HostConfForPeer, the public_key.pem for ssl encryption and also THE PRIVATE IP TO PUT IN THE SECUREP2PCHAT FUNCTION to enable the connection */
   DEBUG_PRINT("\nStarting to get the peer stuff...\n");
   download_file(PATH_TRACKER_DIRECTORY, long_query, "HostConfForPeer");
   download_file(PATH_TRACKER_DIRECTORY, long_query, "public_key.pem");
   download_file(PATH_TRACKER_DIRECTORY, long_query, "private_address");
   DEBUG_PRINT("\n%s stuff gotten!", ip_query);


   //moving all in the folder for that ip
	char peer_folder_on_host[200];
	snprintf(peer_folder_on_host, sizeof(peer_folder_on_host), ".%s-info", long_query);
	mkdir(peer_folder_on_host, 0777);

	//moving files into folder
	char moving_files_comand[300];
	snprintf(moving_files_comand, sizeof(moving_files_comand), "mv private_address public_key.pem HostConfForPeer %s", peer_folder_on_host);
	system(moving_files_comand);


	//reading from the downloaded the file the peeer's private ip
	char path_private_ip_address_peer[200];
	snprintf(path_private_ip_address_peer, sizeof(path_private_ip_address_peer), "%s/%s/private_address",cwd,peer_folder_on_host);
	char *private_ip_of_peer= readFile(path_private_ip_address_peer);

	DEBUG_PRINT("\nPrivate ip of peer: %s\n",private_ip_of_peer);


	// appending the peer's info to our wg config file
	char sourceFile[200];
	snprintf(sourceFile, sizeof(sourceFile), "%s/%s/HostConfForPeer", cwd, peer_folder_on_host);
	char destFile[200];
	snprintf(destFile, sizeof(destFile), "%s/.vpn-secrets/wg0_vpn.conf", cwd);

	FILE *src = fopen(sourceFile, "r");
    if (src == NULL) {
        perror("Error opening source file");
        exit(EXIT_FAILURE);
    }

    FILE *dest = fopen(destFile, "a");
    if (dest == NULL) {
        perror("Error opening destination file");
        fclose(src);
        exit(EXIT_FAILURE);
    }

    char ch;
    while ((ch = fgetc(src)) != EOF) {
        fputc(ch, dest);
    }

    fclose(src);
    fclose(dest);


	//retriving data for initalzing our chat
	char peerPubKey_due[200];
	snprintf(peerPubKey_due, sizeof(peerPubKey_due), "%s/%s/public_key.pem", cwd, peer_folder_on_host); //quessto è wrong, deve essere il path della folder del peer

    char hostPrivateKey[250];
	snprintf(hostPrivateKey, sizeof(hostPrivateKey), "%s/.secrets/private_key.pem", cwd);

	//CHECKPOINT 2 REACHED. If all was correctly executed, now we should start the wg tunnel so that the encr chat can start.
    printf("\nAll good. Activating vpn...");

    char attiva_vpn[150];
    snprintf(attiva_vpn, sizeof attiva_vpn,"sudo wg-quick up %s", destFile);
    system(attiva_vpn);



    //now wee should erease the config file and the folder
	free(wireguardPrivateKey);
	free(pathVPNconfiguration);
	free(pathWireguardPrivateKey);
	free(pathHostPubKey);
	free(hostPubKey);
	free(path_HostConfForPeer);

 secureP2Pchat_simone(private_ip_of_peer, peerPubKey_due, hostPrivateKey);


    return 0;
}
}

char* retrieve_assigend_private_ip(const char* host, int port) {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[MAX_BUF_SIZE];
    char* result = NULL;

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        return NULL;
    }

    // Initialize server address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(host);

    // Connect to server
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection failed");
        close(sockfd);
        return NULL;
    }

    // Receive data
    ssize_t bytes_received = recv(sockfd, buffer, MAX_BUF_SIZE - 1, 0);
    if (bytes_received == -1) {
        perror("Receive failed");
        close(sockfd);
        return NULL;
    }

    // Null-terminate received data
    buffer[bytes_received] = '\0';

    // Copy received data to a dynamically allocated string
    result = strdup(buffer);
    if (result == NULL) {
        perror("Memory allocation failed");
        close(sockfd);
        return NULL;
    }

    // Close socket
    close(sockfd);

    return result;
}





void download_file(const char *base_url, const char *ip_folder, const char *filename) {
    char command[300];
    snprintf(command, sizeof(command), "wget %s/%s/%s", base_url, ip_folder, filename);
    system(command);
}



void signal_handler(int signal) {
    //HANDLING OF THE quit request, disabling the wg conf and deleting file from host and peer
    printf("\nSignal %d received.\n", signal);
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("getcwd error");
        return;
    }

    char wg_conf_path[1024];
    snprintf(wg_conf_path, sizeof(wg_conf_path), "%s/.vpn-secrets/wg0_vpn.conf", cwd);

    char comando[300];
    snprintf(comando, sizeof(comando), "sudo wg-quick down %s", wg_conf_path);
    if (system(comando) == -1) {
        perror("Failed to execute wg-quick down");
    }


    // now let's delete this host files' from the peer
    int sock = 0;
    struct sockaddr_in serv_addr;
    char hello[5] = "DEL";

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(TRACKER_SERVER_IP); // Ensure TRACKER_SERVER_IP is defined
    serv_addr.sin_port = htons(TRACKER_LISTENING_PORT);

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        close(sock);
        return;
    }

    if (send(sock, hello, strlen(hello), 0) < 0) {
        perror("Send failed");
    } else {
        printf("\nMessage sent.\n");
    }

    close(sock);

    // Deleting all peers and various data on this machine
    system("rm -rf .*-files-info");
    system("rm -rf .secrets");
    system("rm -rf .vpn-secrets");


    //WE SHOULD FREE THE MEMORY...


    exit(EXIT_SUCCESS);
}