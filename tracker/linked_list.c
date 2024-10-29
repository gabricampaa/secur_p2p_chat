#include "linked_list.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>


#define HASH_TABLE_SIZE 255

Node* create_node(const char* key, const char* ip_address) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    if (new_node == NULL) {
        fprintf(stderr, "Memoria esaurita\n");
        exit(EXIT_FAILURE);
    }
    strcpy(new_node->key, key);
    strcpy(new_node->ip_address, ip_address);
    new_node->next = NULL;
    return new_node;
}

void insert(HashTable* hash_table, const char* key, const char* ip_address) {
    Node* new_node = create_node(key, ip_address);
    new_node->next = hash_table->head;
    hash_table->head = new_node;
    hash_table->num_connections++;
}

char* search(HashTable* hash_table, const char* key) {
    Node* current = hash_table->head;
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            return current->ip_address;
        }
        current = current->next;
    }
    return NULL;
}

void hash_display(HashTable* hash_table) {
    printf("Numero di connessioni attive: %d\n", hash_table->num_connections);
    printf("Active Nodes:\n");
    Node* current = hash_table->head;
    while (current != NULL) {
        printf("Key: %s, IP Address: %s\n", current->key, current->ip_address);
        current = current->next;
    }
}
