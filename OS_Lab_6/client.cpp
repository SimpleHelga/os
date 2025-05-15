#include "client.h"
#include <ws2tcpip.h>
#include <thread>
#include <iostream>
const std::string Client::SERVER_IP = "127.0.0.1";

void Client::Run() {
    // Регистрация класса окна
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = "ClientWindowClass";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);

    RegisterClass(&wc);

    // Создание окна
    HWND hwnd = CreateWindowEx(0, "ClientWindowClass", "Color Client",
                               WS_OVERLAPPEDWINDOW,
                               CW_USEDEFAULT, CW_USEDEFAULT, 600, 400,
                               NULL, NULL, GetModuleHandle(NULL), this);

    ShowWindow(hwnd, SW_SHOW);

    // Инициализация логгера
    clientLog = new Logger("client_log.txt");
    AppendToLog("Client application started");

    // Цикл сообщений
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    DisconnectFromServer();
    delete clientLog;
}

void Client::InitializeGUI(HWND hwnd) {
    // Кнопки
    CreateWindow("BUTTON", "Connect",
                 WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                 10, 10, 120, 30, hwnd, (HMENU)1, NULL, NULL);

    CreateWindow("BUTTON", "Disconnect",
                 WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                 140, 10, 120, 30, hwnd, (HMENU)2, NULL, NULL);

    // Статус
    hStatusLabel = CreateWindow("STATIC", "Disconnected",
                                WS_VISIBLE | WS_CHILD | SS_LEFT,
                                270, 15, 200, 20, hwnd, NULL, NULL, NULL);

    // Поле лога
    hLogEdit = CreateWindow("EDIT", "",
                            WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE |
                            ES_AUTOVSCROLL | ES_READONLY | WS_VSCROLL,
                            10, 50, 560, 300, hwnd, NULL, NULL, NULL);
}

void Client::ConnectToServer() {
    if (connected) return;

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        AppendToLog("WSAStartup failed");
        return;
    }

    client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client_socket == INVALID_SOCKET) {
        AppendToLog("Socket creation failed");
        WSACleanup();
        return;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP.c_str(), &serverAddr.sin_addr);

    if (connect(client_socket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        AppendToLog("Connection failed");
        closesocket(client_socket);
        WSACleanup();
        return;
    }

    connected = true;
    SetStatus("Connected to " + SERVER_IP);
    AppendToLog("Connected to server");

    // Запуск потока для получения данных
    std::thread([this]() {
        ReceiveData();
    }).detach();
}

void Client::DisconnectFromServer() {
    if (!connected) return;

    if (client_socket != INVALID_SOCKET) {
        send(client_socket, "exit", 4, 0);
        closesocket(client_socket);
        client_socket = INVALID_SOCKET;
    }

    WSACleanup();
    connected = false;
    SetStatus("Disconnected");
    AppendToLog("Disconnected from server");
}

void Client::ReceiveData() {
    char buffer[1024];
    int bytesReceived;

    while ((bytesReceived = recv(client_socket, buffer, sizeof(buffer), 0)) > 0) {
        buffer[bytesReceived] = '\0';
        std::string command(buffer);

        AppendToLog("Received command: " + command);
        SetConsoleColor(command);

        if (command == "exit") {
            break;
        }
    }

    if (bytesReceived == SOCKET_ERROR) {
        AppendToLog("Receive error");
    }

    DisconnectFromServer();
}

void Client::AppendToLog(const std::string& message) {
    clientLog->log(message);

    std::string logMessage = message + "\r\n";
    SendMessage(hLogEdit, EM_REPLACESEL, 0, (LPARAM)logMessage.c_str());
    SendMessage(hLogEdit, EM_SCROLLCARET, 0, 0);
}

void Client::SetStatus(const std::string& status) {
    SetWindowText(hStatusLabel, status.c_str());
}

void Client::SetConsoleColor(const std::string& color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    if (color == "red") {
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
    }
    else if (color == "green") {
        SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    }
    else if (color == "blue") {
        SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    }
    else if (color == "reset") {
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    }

    std::cout << "Color changed to: " << color << std::endl;
}

LRESULT CALLBACK Client::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    Client* pThis = nullptr;

    if (uMsg == WM_NCCREATE) {
        CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
        pThis = (Client*)pCreate->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);
    } else {
        pThis = (Client*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    }

    if (pThis) {
        switch (uMsg) {
            case WM_CREATE:
                pThis->InitializeGUI(hwnd);
                break;

            case WM_COMMAND:
                switch (LOWORD(wParam)) {
                    case 1: // Connect
                        pThis->ConnectToServer();
                        break;

                    case 2: // Disconnect
                        pThis->DisconnectFromServer();
                        break;
                }
                break;

            case WM_DESTROY:
                pThis->DisconnectFromServer();
                PostQuitMessage(0);
                break;

            default:
                return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
