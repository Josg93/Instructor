cmake_minimum_required(VERSION 3.16)
project(Instructor LANGUAGES CXX)

# Configura el estándar C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Busca jsoncpp con el nombre correcto (diferente en algunas distribuciones)
find_package(jsoncpp REQUIRED)  # Nota: minúsculas en algunas versiones
find_package(jwt-cpp REQUIRED)

# Configura Drogon sin requerir UUID explícitamente
find_package(Drogon CONFIG REQUIRED COMPONENTS core)

# Fuentes del proyecto
file(GLOB_RECURSE SOURCES
    "src/*.cpp"
    "controllers/*.cc"
    "services/*.cpp"
)

# Asegúrate de incluir main.cpp en tus fuentes
set(SOURCES
    src/Usuario.cpp
    src/Estudiante.cpp
    src/Profesor.cpp
    src/Curso.cpp
    src/Clase.cpp
    src/FileDatabase.cpp
    services/UsuarioService.cpp
    services/CursoService.cpp
    services/ClaseService.cpp
    controllers/AuthController.cc
    controllers/EstudianteController.cc
    controllers/ProfesorController.cc
    controllers/UsuarioController.cc
    main.cpp
)


add_executable(${PROJECT_NAME} ${SOURCES})

# Configuración de enlaces
target_link_libraries(${PROJECT_NAME} PRIVATE
    Drogon::Drogon
    jwt-cpp::jwt-cpp
    jsoncpp_lib 
    pthread
)

# Configuración de includes
target_include_directories(${PROJECT_NAME} PRIVATE
    include
    src
    services
    controllers
    utils
    ${JSONCPP_INCLUDE_DIRS}
    ${JWT_CPP_INCLUDE_DIRS}
)