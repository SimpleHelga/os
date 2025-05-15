#ifndef SERVER_H
#define SERVER_H

#include <string>
#include <unordered_set>
#include <winsock2.h>
#include <windows.h>
#include "logger.h"

#pragma comment(lib, "ws2_32.lib")

class Server {
public:
    void Run();

private:
    static const int PORT = 12345;
    std::unordered_set<std::string> allowed_commands = {"red", "green", "blue", "reset", "exit"};

    // GUI элементы
    HWND hLogEdit;
    HWND hCommandEdit;

    // Сокеты
    SOCKET server_socket = INVALID_SOCKET;
    SOCKET client_socket = INVALID_SOCKET;

    // Логгер
    Logger* serverLog;

    // Флаги состояния
    bool server_running = false;

    // Методы
    void InitializeGUI(HWND hwnd);
    void StartServer();
    void StopServer();
    void HandleClient();
    void SendToClient(const std::string& message);
    void AppendToLog(const std::string& message);

    // Обработчик сообщений
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

#endif // SERVER_H