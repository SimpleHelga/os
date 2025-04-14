cmake_minimum_required(VERSION 3.30)
project(OS_lab4)

# Настройка для Windows (Winsock)
if(WIN32)
    add_definitions(-D_WIN32_WINNT=0x0601)
endif()

# Серверная часть
add_executable(server main.cpp)
target_link_libraries(server ws2_32 wsock32)
target_compile_features(server PRIVATE cxx_std_20)

# Клиентская часть
add_executable(client client.cpp)
target_link_libraries(client ws2_32 wsock32)
target_compile_features(client PRIVATE cxx_std_20)

# Общая цель для сборки всего
add_custom_target(OS_lab4 ALL DEPENDS server client)