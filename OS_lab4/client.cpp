#include <iostream>
#include <winsock2.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")

void setconsoleColor(const std::string& cmd)
{
    HANDLE hconsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (cmd=="red") SetConsoleTextAttribute(hconsole, FOREGROUND_RED);
    else if (cmd=="green") SetConsoleTextAttribute(hconsole, FOREGROUND_GREEN);
    else if (cmd=="blue") SetConsoleTextAttribute(hconsole, FOREGROUND_BLUE);
    else if (cmd=="reset") SetConsoleTextAttribute(hconsole, 7);
}

int main() {
    WSADATA wsa;
    SOCKET s;
    sockaddr_in server;

    WSAStartup(MAKEWORD(2, 2), &wsa);
    s= socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family=AF_INET;
    server.sin_port=htons(12345);
    server.sin_addr.s_addr=inet_addr("127.0.0.1");

    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    connect(s, ( sockaddr *)&server, sizeof(server));
    std::cout << "Сервер подключен!" << std::endl;

    while (true) {
        char buffer[1024];
        int len;
        while ((len = recv(s, buffer, sizeof(buffer)-1, 0)) > 0)
        {
            buffer[len] = '\0';
            std::string command(buffer);
            if (command == "exit") break;
            setconsoleColor(command);
            std::cout << "Цвет изменен на: "<< command << std::endl;

        }
        closesocket(s);
        WSACleanup();
        return 0;
    }
}