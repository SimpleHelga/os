#include <iostream>
#include "client.h"
#include "server.h"

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    std::cout << "1. Сервер\n2. Клиент\nВыбор: ";
    int choice;
    std::cin >> choice;

    if (choice == 1) {
        Server server;
        server.Run();
    } else if (choice == 2) {
        Client client;
        client.Run();
    } else {
        std::cout << "Неверный выбор!" << std::endl;
        return 1;
    }

    return 0;
}
