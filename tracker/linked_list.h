#define IP_ADDRESS_LENGTH_LINKED_LIST 16

//creating an hash table to assign private ips to the client (and checking it it had alreay been assigned)
typedef struct Node {
    char key[IP_ADDRESS_LENGTH_LINKED_LIST]; // IP pubblico
    char ip_address[IP_ADDRESS_LENGTH_LINKED_LIST]; // IP privato
    struct Node* next;
} Node;

typedef struct HashTable {
    Node* head;
    int num_connections;
} HashTable;


Node* create_node(const char* key, const char* ip_address);
void insert(HashTable* hash_table, const char* key, const char* ip_address);
char* search(HashTable* hash_table, const char* key);
void hash_display(HashTable* hash_table);
