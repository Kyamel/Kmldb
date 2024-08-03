# Defina a versão mínima do CMake e a versão do projeto
cmake_minimum_required(VERSION 3.10)
project(programa VERSION 2.0.0 LANGUAGES C)

# Defina o padrão do compilador e flags de compilação
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")  # Flag para modo de depuração

# Adicione diretórios onde procurar os arquivos de cabeçalho
include_directories(src/model src/kmldb src/controler)

# Adicione uma opção para selecionar o tipo de aplicação
option(BUILD_GUI "Build GUI application" OFF)

# Defina o diretório de fontes com base na opção
if(BUILD_GUI)
    set(VIEW_DIR src/view/gui)
else()
    set(VIEW_DIR src/view/cli)
endif()

# Coletar todos os arquivos de código fonte
file(GLOB_RECURSE SOURCES
    src/*.c
    src/model/*.c
    src/kmldb/*.c
    src/controler/*.c
    ${VIEW_DIR}/*.c
)

# Adicione o executável
add_executable(programa ${SOURCES})

# Defina o diretório onde o executável será gerado
set_target_properties(programa PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Mensagem informativa sobre o tipo de build
message(STATUS "Building ${BUILD_GUI} application. View directory: ${VIEW_DIR}")
