#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <thread>
#include <vector>


#pragma comment(lib,"ws2_32.lib")

const int port = 5555;
const int max_client = 3;

HANDLE semaphore;

void handleclient(SOCKET client_socket) {

    char buffer[1024] = {};
    int  messagerecv = recv(client_socket, buffer, sizeof(buffer), 0);

    if (messagerecv>0) {
        buffer[messagerecv] = '\0';
        std::cout<<"[Server] Принял: "<< buffer << std::endl;

        const char* response = "[SERVER] Данные полученны";
        send(client_socket, response, strlen(response), 0);
    }
    closesocket(client_socket);
    ReleaseSemaphore(semaphore,1,NULL);
}
int main() {
    WSAData wsa_data;
    WSAStartup(MAKEWORD(2,2),&wsa_data);

    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    SOCKET server_socket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr = {};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    bind(server_socket, (sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(server_socket,max_client);

    semaphore = CreateSemaphore(NULL, max_client, max_client, NULL);
    std::vector<std::thread> threads;

    std::cout << "[SERVER] Ожидание подключения клинта... "<<std::endl;

    while (true) {
        WaitForSingleObject(semaphore,INFINITE);
        SOCKET client_socket = accept(server_socket, NULL, NULL);
        threads.emplace_back([client_socket](){
            handleclient(client_socket);
        });
    }

    for (auto& t: threads) {
        if (t.joinable()) t.join();
    }

    closesocket(server_socket);
    WSACleanup();
    CloseHandle(semaphore);
    return 0;
}