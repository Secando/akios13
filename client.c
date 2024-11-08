#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 2255
#define BUFFER_SIZE 1024

void send_request(const char *request) {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Ошибка создания сокета\n");
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("Неверный адрес или адрес не поддерживается\n");
        close(sock);
        return;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Ошибка подключения\n");
        close(sock);
        return;
    }

    send(sock, request, strlen(request), 0);
    read(sock, buffer, BUFFER_SIZE);
    printf("Ответ сервера на запрос '%s': %s\n", request, buffer);

    close(sock);
}

int main() {
    send_request("etc");
    send_request("dev");
    return 0;
}
