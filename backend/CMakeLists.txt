cmake_minimum_required(VERSION 3.16)
project(Instructor LANGUAGES CXX)

# Configura el estándar C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Busca jsoncpp con el nombre correcto (diferente en algunas distribuciones)
find_package(Drogon CONFIG REQUIRED COMPONENTS core)
#find_package(BCrypt REQUIRED)
find_package(jsoncpp REQUIRED)
find_package(jwt-cpp REQUIRED)

set(BCRYPT_INCLUDE_DIR "/usr/local/include")  # Ruta donde se instalaron los headers
set(BCRYPT_LIBRARY "/usr/local/lib/libbcrypt.a")

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
    src/Utils.cpp
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
    ${BCRYPT_LIBRARY}
    jsoncpp_lib 
    pthread
    crypto 
)

# Configuración de includes
target_include_directories(${PROJECT_NAME} PRIVATE
    include
    src
    services
    controllers
    utils
    filters
    ${BCRYPT_INCLUDE_DIR} 
    ${JSONCPP_INCLUDE_DIRS}
    ${JWT_CPP_INCLUDE_DIRS}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(BCrypt DEFAULT_MSG
    BCRYPT_INCLUDE_DIR
    BCRYPT_LIBRARY
)