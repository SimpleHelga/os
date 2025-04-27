cmake_minimum_required(VERSION 3.15)
project(ClientServerUniqueArray)

# Используем стандарт C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Настройки для Windows
if (WIN32)
    add_definitions(-D_WIN32_WINNT=0x0601)  # Windows 7+
endif()

# === Сервер ===
add_executable(server main.cpp)
# Линкуем библиотеку Winsock2 (обязательно для сокетов)
target_link_libraries(server ws2_32)
target_compile_features(server PRIVATE cxx_std_20)

# === Клиент ===
add_executable(client client.cpp)
target_link_libraries(client ws2_32)
target_compile_features(client PRIVATE cxx_std_20)

# Общая цель, которая собирает обе части вместе
add_custom_target(ClientServerLab ALL DEPENDS server client)
