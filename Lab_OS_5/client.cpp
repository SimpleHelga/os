#include <winsock2.h>
#include <iostream>
#include <sstream>
#include <set>

#pragma comment(lib, "ws2_32.lib")

const char* Server_ip = "127.0.0.1";
const int port = 5555;

int main() {
    WSAData wsa_data;
    WSAStartup(MAKEWORD(2, 2), &wsa_data);

    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    SOCKET client_socket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serveraddr = {};
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(port);
    serveraddr.sin_addr.s_addr = inet_addr(Server_ip);

    if (connect(client_socket, (sockaddr*) &serveraddr, sizeof(serveraddr)) == SOCKET_ERROR) {
        std::cerr << "[Client] Подключение не удалось. " << std::endl;
        closesocket(client_socket);
        WSACleanup();
        return 1;
    }

    std::set<double> unigue_number;
    std::string line;

    std::cout << "[Client] введите числа через пробелы (exit для завершения): ";

    while (true) {
        std::cout << "> ";
        std::getline(std::cin, line);

        if (line == "exit") {
            closesocket(client_socket);
            WSACleanup();
            return 0;
        }

        if (line == "end" || line.empty()) {
            break;
        }

        std::istringstream iss(line);
        double num;
        while (iss >> num) {
            unigue_number.insert(num);
        }
    }

    std::ostringstream oss;
    for (auto n: unigue_number) oss << n << ' ';

    std::string data = oss.str();
    send(client_socket, data.c_str(),data.length(), 0);

    char buffer[512] = {};
    int bytesrecv = recv(client_socket,buffer,sizeof(buffer), 0);
    if (bytesrecv > 0) {
        buffer[bytesrecv] = '\0';
        std::cout << buffer << std::endl;
    }

    closesocket(client_socket);
    WSACleanup();
    return 0;
}