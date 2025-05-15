#include "server.h"
#include <ws2tcpip.h>
#include <thread>

void Server::Run() {
    // Регистрация класса окна
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = "ServerWindowClass";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);

    RegisterClass(&wc);

    // Создание окна
    HWND hwnd = CreateWindowEx(0, "ServerWindowClass", "Color Server",
                               WS_OVERLAPPEDWINDOW,
                               CW_USEDEFAULT, CW_USEDEFAULT, 600, 400,
                               NULL, NULL, GetModuleHandle(NULL), this);

    ShowWindow(hwnd, SW_SHOW);

    // Инициализация логгера
    serverLog = new Logger("server_log.txt");
    AppendToLog("Server application started");

    // Цикл сообщений
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    delete serverLog;
}

void Server::InitializeGUI(HWND hwnd) {
    // Кнопки
    CreateWindow("BUTTON", "Start Server",
                 WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                 10, 10, 120, 30, hwnd, (HMENU)1, NULL, NULL);

    CreateWindow("BUTTON", "Stop Server",
                 WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                 140, 10, 120, 30, hwnd, (HMENU)2, NULL, NULL);

    // Поле лога
    hLogEdit = CreateWindow("EDIT", "",
                            WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE |
                            ES_AUTOVSCROLL | ES_READONLY | WS_VSCROLL,
                            10, 50, 560, 280, hwnd, NULL, NULL, NULL);

    // Поле ввода команд
    hCommandEdit = CreateWindow("EDIT", "",
                                WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
                                10, 340, 460, 30, hwnd, NULL, NULL, NULL);

    // Кнопка отправки
    CreateWindow("BUTTON", "Send",
                 WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                 480, 340, 90, 30, hwnd, (HMENU)3, NULL, NULL);
}

void Server::StartServer() {
    if (server_running) return;

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        AppendToLog("WSAStartup failed");
        return;
    }

    server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_socket == INVALID_SOCKET) {
        AppendToLog("Socket creation failed");
        WSACleanup();
        return;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        AppendToLog("Bind failed");
        closesocket(server_socket);
        WSACleanup();
        return;
    }

    if (listen(server_socket, SOMAXCONN) == SOCKET_ERROR) {
        AppendToLog("Listen failed");
        closesocket(server_socket);
        WSACleanup();
        return;
    }

    server_running = true;
    AppendToLog("Server started on port " + std::to_string(PORT));

    // Запуск потока для принятия подключений
    std::thread([this]() {
        while (server_running) {
            sockaddr_in clientAddr;
            int clientAddrSize = sizeof(clientAddr);
            client_socket = accept(server_socket, (sockaddr*)&clientAddr, &clientAddrSize);

            if (client_socket == INVALID_SOCKET) {
                if (server_running) {
                    AppendToLog("Accept failed");
                }
                continue;
            }

            char clientIP[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);
            AppendToLog("Client connected: " + std::string(clientIP));

            // Обработка клиента
            HandleClient();
        }
    }).detach();
}

void Server::StopServer() {
    if (!server_running) return;

    server_running = false;

    if (client_socket != INVALID_SOCKET) {
        send(client_socket, "exit", 4, 0);
        closesocket(client_socket);
        client_socket = INVALID_SOCKET;
    }

    closesocket(server_socket);
    WSACleanup();

    AppendToLog("Server stopped");
}

void Server::HandleClient() {
    char buffer[1024];
    int bytesReceived;

    while ((bytesReceived = recv(client_socket, buffer, sizeof(buffer), 0)) > 0) {
        buffer[bytesReceived] = '\0';
        std::string message = "Received: " + std::string(buffer);
        AppendToLog(message);
    }

    if (bytesReceived == SOCKET_ERROR) {
        AppendToLog("Receive error");
    }

    closesocket(client_socket);
    client_socket = INVALID_SOCKET;
    AppendToLog("Client disconnected");
}

void Server::SendToClient(const std::string& message) {
    if (client_socket == INVALID_SOCKET) {
        AppendToLog("No client connected");
        return;
    }

    if (allowed_commands.count(message) == 0) {
        AppendToLog("Invalid command: " + message);
        return;
    }

    send(client_socket, message.c_str(), message.size(), 0);
    AppendToLog("Sent to client: " + message);

    if (message == "exit") {
        closesocket(client_socket);
        client_socket = INVALID_SOCKET;
    }
}

void Server::AppendToLog(const std::string& message) {
    serverLog->log(message);

    std::string logMessage = message + "\r\n";
    SendMessage(hLogEdit, EM_REPLACESEL, 0, (LPARAM)logMessage.c_str());
    SendMessage(hLogEdit, EM_SCROLLCARET, 0, 0);
}

LRESULT CALLBACK Server::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    Server* pThis = nullptr;

    if (uMsg == WM_NCCREATE) {
        CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
        pThis = (Server*)pCreate->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);
    } else {
        pThis = (Server*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    }

    if (pThis) {
        switch (uMsg) {
            case WM_CREATE:
                pThis->InitializeGUI(hwnd);
                break;

            case WM_COMMAND:
                switch (LOWORD(wParam)) {
                    case 1: // Start
                        pThis->StartServer();
                        break;

                    case 2: // Stop
                        pThis->StopServer();
                        break;

                    case 3: // Send
                        char buffer[256];
                        GetWindowText(pThis->hCommandEdit, buffer, sizeof(buffer));
                        pThis->SendToClient(buffer);
                        SetWindowText(pThis->hCommandEdit, "");
                        break;
                }
                break;

            case WM_DESTROY:
                pThis->StopServer();
                PostQuitMessage(0);
                break;

            default:
                return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}