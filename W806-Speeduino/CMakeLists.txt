cmake_minimum_required ( VERSION 3.9 )
include(FetchContent)
Set(FETCHCONTENT_QUIET FALSE)

#Fetch wm-sdk-w806
FetchContent_Declare(
    wm-sdk-w806
    GIT_REPOSITORY    https://github.com/IOsetting/wm-sdk-w806
    GIT_TAG           main
    GIT_PROGRESS      TRUE
)
FetchContent_Populate(wm-sdk-w806)

# build wm-sdk-w806 libraries
include_directories(${CMAKE_BINARY_DIR}/_deps/wm-sdk-w806-src/include)
include_directories(${CMAKE_BINARY_DIR}/_deps/wm-sdk-w806-src/include/arch/xt804)
include_directories(${CMAKE_BINARY_DIR}/_deps/wm-sdk-w806-src/include/arch/xt804/csi_core)
include_directories(${CMAKE_BINARY_DIR}/_deps/wm-sdk-w806-src/include/driver)
project(w806_bsp C ASM)
file(GLOB_RECURSE w806_bsp_csrcs ${CMAKE_BINARY_DIR}/_deps/wm-sdk-w806-src/platform/arch/xt804/bsp/*.c)
file(GLOB_RECURSE w806_bsp_asmsrcs ${CMAKE_BINARY_DIR}/_deps/wm-sdk-w806-src/platform/arch/xt804/bsp/*.S)
add_library(w806_bsp STATIC ${w806_bsp_csrcs} ${w806_bsp_asmsrcs})
project(w806_libc C)
file(GLOB_RECURSE w806_libc_csrcs ${CMAKE_BINARY_DIR}/_deps/wm-sdk-w806-src/platform/arch/xt804/libc/*.c)
add_library(w806_libc STATIC ${w806_libc_csrcs})
project(w806_drivers C)
file(GLOB_RECURSE w806_drivers_csrcs ${CMAKE_BINARY_DIR}/_deps/wm-sdk-w806-src/platform/drivers/*.c)
add_library(w806_drivers STATIC ${w806_drivers_csrcs})

#Fetch and build EmbeddedIOServices
FetchContent_Declare(
    EmbeddedIOServices
    GIT_REPOSITORY    https://github.com/FL0WL0W/EmbeddedIOServices
    GIT_TAG           main
    GIT_PROGRESS      TRUE
)
FetchContent_MakeAvailable(EmbeddedIOServices)

#Fetch Speeduino
FetchContent_Declare(
    Speeduino
    URL    https://github.com/noisymime/speeduino/archive/refs/tags/202402.1.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(Speeduino)

set(TARGET		W806Speeduino)

project(${TARGET} C CXX ASM)

set(LIBRARIES	libs)

include_directories(include)
include_directories(${CMAKE_BINARY_DIR}/_deps/embeddedioservices-src/HardwareAbstractions/W80x/include)
include_directories(${CMAKE_BINARY_DIR}/_deps/speeduino-src/speeduino)

file(GLOB_RECURSE USER_CSOURCES src/*.c)
file(GLOB_RECURSE USER_CPPSOURCES src/*.cpp)
file(GLOB_RECURSE EmbeddedIOService_SOURCES ${CMAKE_BINARY_DIR}/_deps/embeddedioservices-src/HardwareAbstractions/W80x/src/*.cpp)
file(GLOB_RECURSE Speeduino_SOURCES ${CMAKE_BINARY_DIR}/_deps/speeduino-src/speeduino/*.cpp)
list(FILTER Speeduino_SOURCES EXCLUDE REGEX "FRAM/") # remove FRAM
list(FILTER Speeduino_SOURCES EXCLUDE REGEX "SPIAsEEPROM/") # remove SPIAsEEPROM
add_compile_definitions(PROGMEM=)
add_compile_definitions(BOARD_H="board.h")
add_compile_definitions(ARDUINO=100)

set(SRCS 
        ${USER_CSOURCES} 
        ${USER_CPPSOURCES} 
        ${EmbeddedIOService_SOURCES}
        ${Speeduino_SOURCES})
add_executable(${TARGET}.elf ${SRCS})

target_link_libraries(${TARGET}.elf w806_bsp)
target_link_libraries(${TARGET}.elf w806_libc)
target_link_libraries(${TARGET}.elf w806_drivers)
target_link_libraries(${TARGET}.elf EmbeddedIOServices)

set(BIN_FILE ${CMAKE_BINARY_DIR}/${TARGET}.bin)
add_custom_command(TARGET ${TARGET}.elf POST_BUILD
        COMMAND bash -c "gcc ${CMAKE_BINARY_DIR}/_deps/wm-sdk-w806-src/tools/W806/wm_tool.c -lpthread -o ${CMAKE_BINARY_DIR}/wm_tool"
        COMMAND ${CMAKE_OBJCOPY} ARGS -O binary $<TARGET_FILE:${TARGET}.elf> ${BIN_FILE}
        COMMAND bash -c "${CMAKE_BINARY_DIR}/wm_tool -b ${BIN_FILE} -fc 0 -it 0 -ih 8002000 -ra 8002400 -ua 8002000 -nh 0 -un 0 -o ${CMAKE_SOURCE_DIR}/build/${TARGET}"
        COMMENT "Building ${BIN_FILE} ${CMAKE_SOURCE_DIR}/build/${TARGET}.img")

set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install")
install(CODE "execute_process( 
    COMMAND bash -c \"curl --data-binary @${CMAKE_SOURCE_DIR}/build/${TARGET}.img http://192.168.4.1/upload/w806\"
)")
