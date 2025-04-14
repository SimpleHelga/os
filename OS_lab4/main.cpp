#include <iostream>
#include <unordered_set>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

int main() {
    setlocale(LC_ALL,"RU");
    WSAData wsa;
    SOCKET server_socket, client_socket;
    sockaddr_in server, client;
    int clientlen = sizeof(client);

    WSAStartup(MAKEWORD(2, 2), &wsa);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family = AF_INET;
    server.sin_port=htons(12345);
    server.sin_addr.s_addr = INADDR_ANY;

    bind(server_socket, (sockaddr *)&server, sizeof(server));
    listen(server_socket, 1);

    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    std::cout << "Сервер запущен...\n";
    std::cout << "Ожидание клиента ...\n";
    client_socket = accept(server_socket, (sockaddr *)&client, &clientlen);
    std::cout << "Клиент подключен...\n";

    std::unordered_set<std::string> allowed_colors = {"red", "green", "blue", "reset","exit"};

    std::string message;
    while (true) {
        std::cout << "Введите команду (red, green, blue, reset, exit): ";
        std::getline(std::cin, message);
        if (allowed_colors.count(message) == 0) {
            std::cout << "Недопустимый цвет!\n";
            continue;
        }
        send(client_socket, message.c_str(), message.size(), 0);
        if (message == "exit") {
            break;
        }
    }
    closesocket(client_socket);
    closesocket(server_socket);
    WSACleanup();
    return 0;
}