#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <winsock2.h>
#include <windows.h>
#include "logger.h"

#pragma comment(lib, "ws2_32.lib")

class Client {
public:
    void Run();

private:
    static const std::string SERVER_IP;
    static const int PORT = 12345;

    // GUI элементы
    HWND hLogEdit;
    HWND hStatusLabel;

    // Сокет
    SOCKET client_socket = INVALID_SOCKET;

    // Логгер
    Logger* clientLog;

    // Флаги состояния
    bool connected = false;

    // Методы
    void InitializeGUI(HWND hwnd);
    void ConnectToServer();
    void DisconnectFromServer();
    void ReceiveData();
    void AppendToLog(const std::string& message);
    void SetStatus(const std::string& status);
    static void SetConsoleColor(const std::string& color);

    // Обработчик сообщений
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

#endif // CLIENT_H