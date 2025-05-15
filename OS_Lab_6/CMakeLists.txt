cmake_minimum_required(VERSION 3.20)
project(test)

set(CMAKE_CXX_STANDARD 20)

# Указываем исходные файлы
set(SOURCE_FILES
        main.cpp
        server.cpp
        client.cpp
        logger.cpp
)

# Указываем заголовочные файлы (для ясности)
set(HEADER_FILES
        server.h
        client.h
        logger.h
)

# Создаем исполняемый файл
add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES})

# Линкуем библиотеку Winsock
target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32)