#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 2255
#define BUFFER_SIZE 1024
#define STUDENT_ID "123456" // замените на ваш ID

void get_conf_files(char *result) {
    struct dirent *entry;
    DIR *dir = opendir("/etc");
    if (dir == NULL) {
        perror("Ошибка открытия каталога /etc");
        strcpy(result, "Ошибка доступа к каталогу /etc");
        return;
    }
    strcpy(result, "server" STUDENT_ID ",");
    while ((entry = readdir(dir)) != NULL) {
        if (strstr(entry->d_name, ".conf")) {
            strcat(result, entry->d_name);
            strcat(result, ",");
        }
    }
    closedir(dir);
}

void get_dev_directories(char *result) {
    struct dirent *entry;
    DIR *dir = opendir("/dev");
    if (dir == NULL) {
        perror("Ошибка открытия каталога /dev");
        strcpy(result, "Ошибка доступа к каталогу /dev");
        return;
    }
    strcpy(result, "");
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            strcat(result, entry->d_name);
            strcat(result, ",");
        }
    }
    closedir(dir);
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};
    char response[BUFFER_SIZE] = {0};

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Ошибка создания сокета");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("Ошибка настройки сокета");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Ошибка привязки сокета");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("Ошибка прослушивания сокета");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Сервер запущен и ожидает подключения...\n");

    while ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) >= 0) {
        memset(buffer, 0, BUFFER_SIZE);
        read(new_socket, buffer, BUFFER_SIZE);

        if (strcmp(buffer, "etc") == 0) {
            get_conf_files(response);
        } else if (strcmp(buffer, "dev") == 0) {
            get_dev_directories(response);
        } else {
            strcpy(response, "Неверная команда");
        }

        send(new_socket, response, strlen(response), 0);
        close(new_socket);
        printf("Ответ отправлен: %s\n", response);
    }

    if (new_socket < 0) {
        perror("Ошибка подключения клиента");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    return 0;
}
