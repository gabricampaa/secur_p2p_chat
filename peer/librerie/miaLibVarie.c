#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/err.h>
#include <curl/curl.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdbool.h>
#include <libgen.h> // For basename()



char* concatenateStrings(const char* str1, const char* str2) {
    // Alloca memoria per la nuova stringa, considerando la lunghezza di entrambe le stringhe più il terminatore NULL
    char* result = malloc(strlen(str1) + strlen(str2) + 1);
    if (result == NULL) {
        fprintf(stderr, "Errore durante l'allocazione di memoria\n");
        exit(1);
    }

    // Copia la prima stringa nella nuova stringa
    strcpy(result, str1);

    // Concatena la seconda stringa alla fine della prima
    strcat(result, str2);

    return result;
}





size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata) {
    // Concatenate received data into userdata
    strcat(userdata, ptr);
    return size * nmemb;
}



// Function to retrieve public IPv4 address
char* getHostIp() {
    CURL *curl;
    CURLcode res;

    // Initialize curl
    curl = curl_easy_init();
    if (curl) {
        // Set URL to get public IP
        curl_easy_setopt(curl, CURLOPT_URL, "http://api.ipify.org/");

        // Response buffer
        char* response = (char*)malloc(100);
        if (!response) {
            fprintf(stderr, "Memory allocation failed.\n");
            return NULL;
        }
        response[0] = '\0';

        // Set callback function to write response to buffer
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);

        // Perform HTTP request
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            free(response);
            return NULL;
        }

        // Cleanup
        curl_easy_cleanup(curl);

        // Return public IP address
        return response;
    } else {
        fprintf(stderr, "Failed to initialize curl.\n");
        return NULL;
    }
}



char * readFile(const char * filename) {
  FILE * file;
  long fileSize;
  char * buffer;

  // Open the file for reading
  file = fopen(filename, "rb");
  if (file == NULL) {
    printf("Unable to open file.\n");
    return NULL;
  }

  // Determine the file size
  fseek(file, 0, SEEK_END);
  fileSize = ftell(file);
  rewind(file);

  // Allocate memory to store the file content
  buffer = (char * ) malloc(fileSize + 1);
  if (buffer == NULL) {
    printf("Memory allocation failed.\n");
    fclose(file);
    return NULL;
  }

  // Read the file content into the buffer
  if (fread(buffer, 1, fileSize, file) != fileSize) {
    printf("Error reading file.\n");
    fclose(file);
    free(buffer);
    return NULL;
  }

  // Null-terminate the buffer
  buffer[fileSize] = '\0';

  // Close the file
  fclose(file);

  return buffer;

}
