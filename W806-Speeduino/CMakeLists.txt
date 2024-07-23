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

#Remove error in globals and replace with warning
#fix revolutionTime type
file(READ ${CMAKE_BINARY_DIR}/_deps/speeduino-src/speeduino/globals.h FILE_CONTENTS)
string(REPLACE "#error" "//#warning" FILE_CONTENTS "${FILE_CONTENTS}")
string(REPLACE "unsigned long revolutionTime" "uint32_t revolutionTime" FILE_CONTENTS "${FILE_CONTENTS}")
file(WRITE ${CMAKE_BINARY_DIR}/_deps/speeduino-src/speeduino/globals.h "${FILE_CONTENTS}")

#do modifications to init.cpp to inject our board pins
file(READ ${CMAKE_BINARY_DIR}/_deps/speeduino-src/speeduino/init.cpp FILE_CONTENTS)
file(READ src/init.cppi FILE_MOD_CONTENTS)
string(REPLACE "#if defined(CORE_TEENSY)" "${FILE_MOD_CONTENTS}" FILE_CONTENTS "${FILE_CONTENTS}")
string(REPLACE "#if defined(CORE_STM32)" "#if defined(INPUT_ANALOG)" FILE_CONTENTS "${FILE_CONTENTS}")
string(REPLACE "pinMode(pinFuelPressure, INPUT)" "pinMode(pinFuelPressure, INPUT_ANALOG)" FILE_CONTENTS "${FILE_CONTENTS}")
string(REPLACE "pinMode(pinOilPressure, INPUT)" "pinMode(pinOilPressure, INPUT_ANALOG)" FILE_CONTENTS "${FILE_CONTENTS}")
string(REPLACE "configPage9.intcan_available = 1" "//configPage9.intcan_available = 1" FILE_CONTENTS "${FILE_CONTENTS}")
file(WRITE ${CMAKE_BINARY_DIR}/_deps/speeduino-src/speeduino/initMod.cpp "${FILE_CONTENTS}")

# let trigger 3 be configured for fun stuff too
file(READ ${CMAKE_BINARY_DIR}/_deps/speeduino-src/speeduino/init.h FILE_CONTENTS)
string(REPLACE "&& (pinVSS == pinTrigger2) && !BIT_CHECK(decoderState, BIT_DECODER_HAS_SECONDARY)" "&& ((configPage10.vvt2Enabled < 1 && pinVSS == pinTrigger3) || ((pinVSS == pinTrigger2) && !BIT_CHECK(decoderState, BIT_DECODER_HAS_SECONDARY)))" FILE_CONTENTS "${FILE_CONTENTS}")
string(REPLACE "&& (pinFlex == pinTrigger2) && !BIT_CHECK(decoderState, BIT_DECODER_HAS_SECONDARY)" "&& ((configPage10.vvt2Enabled < 1 && pinFlex == pinTrigger3) || ((pinFlex == pinTrigger2) && !BIT_CHECK(decoderState, BIT_DECODER_HAS_SECONDARY)))" FILE_CONTENTS "${FILE_CONTENTS}")
file(WRITE ${CMAKE_BINARY_DIR}/_deps/speeduino-src/speeduino/init.h "${FILE_CONTENTS}")

file(READ ${CMAKE_BINARY_DIR}/_deps/speeduino-src/reference/speeduino.ini FILE_CONTENTS)
string(REPLACE "\"INVALID\", \"INVALID\", \"3\"" "\"1\", \"2\", \"3\"" FILE_CONTENTS "${FILE_CONTENTS}")
string(REPLACE 
                "\"1\", \"2\", \"3\", \"4\", \"5\", \"6\", \"7\", \"8\", \"9\", \"10\", \"11\", \"12\", \"13\", \"14\", \"15\", \"16\", \"17\", \"18\", \"19\", \"20\", \"21\", \"22\", \"23\", \"24\", \"25\", \"26\", \"27\", \"28\", \"29\", \"30\", \"31\", \"32\", \"33\", \"34\", \"35\", \"36\", \"37\", \"38\", \"39\", \"40\", \"41\", \"42\", \"43\", \"44\", \"45\", \"46\", \"47\", \"48\", \"49\", \"50\", \"51\", \"52\", \"53\","
                "\"A1\", \"B1\", \"C1\", \"D1\", \"E1\", \"F1\", \"G1\", \"H1\", \"J1\", \"K1\", \"L1\", \"M1\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"A2\", \"B2\", \"C2\", \"D2\", \"E2\", \"F2\", \"G2\", \"H2\", \"J2\", \"K2\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"B3\", \"C3\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"A4\", \"B4\", \"C4\", \"D4\", \"E4\", \"F4\", \"G4\", \"H4\", \"J4\", \"K4\","
                FILE_CONTENTS "${FILE_CONTENTS}")

string(REPLACE 
                "\"A0\", \"A1\", \"A2\", \"A3\", \"A4\", \"A5\", \"A6\", \"A7\", \"A8\", \"A9\", \"A10\", \"A11\", \"A12\", \"A13\", \"A14\", \"A15\", \"A16\", \"A17\", \"A18\", \"A19\", \"A20\", \"A21\", \"A22\""
                "\"C1\", \"A4\", \"B4\", \"C4\", \"D4\", \"E4\", \"F4\", \"G4\", \"H4\", \"J4\", \"K4\", \"A2\", \"B2\", \"C2\", \"D2\", \"E2\", \"F2\", \"G2\", \"H2\", \"J2\", \"K2\", \"A1\", \"B1\""
                FILE_CONTENTS "${FILE_CONTENTS}")

string(REPLACE 
                "\"A0\", \"A1\", \"A2\", \"A3\", \"A4\", \"A5\", \"A6\", \"A7\", \"A8\", \"A9\", \"A10\", \"A11\", \"A12\", \"A13\", \"A14\", \"A15\""
                "\"C1\", \"A4\", \"B4\", \"C4\", \"D4\", \"E4\", \"F4\", \"G4\", \"H4\", \"J4\", \"K4\", \"A2\", \"B2\", \"C2\", \"D2\", \"E2\", \"F2\", \"G2\", \"H2\", \"J2\", \"K2\", \"A1\", \"B1\""
                FILE_CONTENTS "${FILE_CONTENTS}")

string(REPLACE 
                "tachoPin      = bits,   U08,      16, [0:5],  \"Board Default\", \"A1\", \"B1\", \"C1\", \"D1\", \"E1\", \"F1\", \"G1\", \"H1\", \"J1\", \"K1\", \"L1\", \"M1\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"A2\", \"B2\", \"C2\", \"D2\", \"E2\", \"F2\", \"G2\", \"H2\", \"J2\", \"K2\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"B3\", \"C3\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"A4\", \"B4\", \"C4\", \"D4\", \"E4\", \"F4\", \"G4\", \"H4\", \"J4\", \"K4\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\""
                "tachoPin      = bits,   U08,      16, [0:5],  \"Unused\", \"A1\", \"B1\", \"C1\", \"D1\", \"E1\", \"F1\", \"G1\", \"H1\", \"J1\", \"K1\", \"L1\", \"M1\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"A2\", \"B2\", \"C2\", \"D2\", \"E2\", \"F2\", \"G2\", \"H2\", \"J2\", \"K2\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\""
                FILE_CONTENTS "${FILE_CONTENTS}")

string(REPLACE 
                "idleUpPin     = bits,   U08,      57, [0:5], \"Board Default\", \"A1\", \"B1\", \"C1\", \"D1\", \"E1\", \"F1\", \"G1\", \"H1\", \"J1\", \"K1\", \"L1\", \"M1\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"A2\", \"B2\", \"C2\", \"D2\", \"E2\", \"F2\", \"G2\", \"H2\", \"J2\", \"K2\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"B3\", \"C3\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"A4\", \"B4\", \"C4\", \"D4\", \"E4\", \"F4\", \"G4\", \"H4\", \"J4\", \"K4\", \"INVALID\", \"A8\", \"A9\", \"A10\", \"A11\", \"A12\", \"A13\", \"A14\", \"A15\", \"INVALID\""
                "idleUpPin     = bits,   U08,      57, [0:5], \"INVALID\", \"A1\", \"B1\", \"C1\", \"D1\", \"E1\", \"F1\", \"G1\", \"H1\", \"INVALID\", \"INVALID\", \"L1\", \"M1\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"A2\", \"B2\", \"C2\", \"D2\", \"E2\", \"F2\", \"G2\", \"H2\", \"J2\", \"K2\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"B3\", \"C3\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"A4\", \"B4\", \"C4\", \"D4\", \"E4\", \"F4\", \"G4\", \"H4\", \"J4\", \"K4\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\""
                FILE_CONTENTS "${FILE_CONTENTS}")

string(REPLACE 
                "CTPSPin       = bits,  U08,  91, [0:5], \"Board Default\", \"A1\", \"B1\", \"C1\", \"D1\", \"E1\", \"F1\", \"G1\", \"H1\", \"J1\", \"K1\", \"L1\", \"M1\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"A2\", \"B2\", \"C2\", \"D2\", \"E2\", \"F2\", \"G2\", \"H2\", \"J2\", \"K2\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"B3\", \"C3\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"A4\", \"B4\", \"C4\", \"D4\", \"E4\", \"F4\", \"G4\", \"H4\", \"J4\", \"K4\", \"INVALID\", \"A8\", \"A9\", \"A10\", \"A11\", \"A12\", \"A13\", \"A14\", \"A15\", \"INVALID\""
                "CTPSPin       = bits,  U08,  91, [0:5], \"INVALID\", \"A1\", \"B1\", \"C1\", \"D1\", \"E1\", \"F1\", \"G1\", \"H1\", \"INVALID\", \"INVALID\", \"L1\", \"M1\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"A2\", \"B2\", \"C2\", \"D2\", \"E2\", \"F2\", \"G2\", \"H2\", \"J2\", \"K2\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"B3\", \"C3\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"A4\", \"B4\", \"C4\", \"D4\", \"E4\", \"F4\", \"G4\", \"H4\", \"J4\", \"K4\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\""
                FILE_CONTENTS "${FILE_CONTENTS}")

string(REPLACE 
                "vssPin        = bits,  U08,       102, [2:7], \"Board Default\", \"A1\", \"B1\", \"C1\", \"D1\", \"E1\", \"F1\", \"G1\", \"H1\", \"J1\", \"K1\", \"L1\", \"M1\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"A2\", \"B2\", \"C2\", \"D2\", \"E2\", \"F2\", \"G2\", \"H2\", \"J2\", \"K2\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"B3\", \"C3\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"A4\", \"B4\", \"C4\", \"D4\", \"E4\", \"F4\", \"G4\", \"H4\", \"J4\", \"K4\", \"INVALID\", \"A8\", \"A9\", \"A10\", \"A11\", \"A12\", \"A13\", \"A14\", \"A15\", \"INVALID\""
                "vssPin        = bits,  U08,       102, [2:7], \"INVALID\", \"A1\", \"B1\", \"C1\", \"4\", \"5\", \"6\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"L1\", \"M1\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"A2\", \"B2\", \"C2\", \"D2\", \"E2\", \"F2\", \"G2\", \"H2\", \"J2\", \"K2\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"B3\", \"C3\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\""
                FILE_CONTENTS "${FILE_CONTENTS}")

string(REPLACE 
                "idleUpOutputPin       = bits,   U08,      118, [2:7], \"Board Default\", \"A1\", \"B1\", \"C1\", \"D1\", \"E1\", \"F1\", \"G1\", \"H1\", \"J1\", \"K1\", \"L1\", \"M1\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"A2\", \"B2\", \"C2\", \"D2\", \"E2\", \"F2\", \"G2\", \"H2\", \"J2\", \"K2\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"B3\", \"C3\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"A4\", \"B4\", \"C4\", \"D4\", \"E4\", \"F4\", \"G4\", \"H4\", \"J4\", \"K4\", \"INVALID\", \"A8\", \"A9\", \"A10\", \"A11\", \"A12\", \"A13\", \"A14\", \"A15\", \"INVALID\""
                "idleUpOutputPin       = bits,   U08,      118, [2:7], \"INVALID\", \"A1\", \"B1\", \"C1\", \"D1\", \"E1\", \"F1\", \"G1\", \"H1\", \"J1\", \"K1\", \"L1\", \"M1\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"A2\", \"B2\", \"C2\", \"D2\", \"E2\", \"F2\", \"G2\", \"H2\", \"J2\", \"K2\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\""
                FILE_CONTENTS "${FILE_CONTENTS}")

string(REPLACE 
                "fuelPumpPin= bits  , U08,      6,[1:6],    \"Board Default\", \"A1\", \"B1\", \"C1\", \"D1\", \"E1\", \"F1\", \"G1\", \"H1\", \"J1\", \"K1\", \"L1\", \"M1\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"A2\", \"B2\", \"C2\", \"D2\", \"E2\", \"F2\", \"G2\", \"H2\", \"J2\", \"K2\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"B3\", \"C3\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"A4\", \"B4\", \"C4\", \"D4\", \"E4\", \"F4\", \"G4\", \"H4\", \"J4\", \"K4\", \"INVALID\", \"A8\", \"A9\", \"A10\", \"A11\", \"A12\", \"A13\", \"A14\", \"A15\", \"INVALID\""
                "fuelPumpPin= bits  , U08,      6,[1:6],    \"Unused\", \"A1\", \"B1\", \"C1\", \"D1\", \"E1\", \"F1\", \"G1\", \"H1\", \"J1\", \"K1\", \"L1\", \"M1\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"A2\", \"B2\", \"C2\", \"D2\", \"E2\", \"F2\", \"G2\", \"H2\", \"J2\", \"K2\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\""
                FILE_CONTENTS "${FILE_CONTENTS}")

string(REPLACE 
                "resetControlPin = bits,   U08, 10,[2:7],  \"Board Default\", \"A1\", \"B1\", \"C1\", \"D1\", \"E1\", \"F1\", \"G1\", \"H1\", \"J1\", \"K1\", \"L1\", \"M1\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"A2\", \"B2\", \"C2\", \"D2\", \"E2\", \"F2\", \"G2\", \"H2\", \"J2\", \"K2\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"B3\", \"C3\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"A4\", \"B4\", \"C4\", \"D4\", \"E4\", \"F4\", \"G4\", \"H4\", \"J4\", \"K4\", \"INVALID\", \"A8\", \"A9\", \"A10\", \"A11\", \"A12\", \"A13\", \"A14\", \"A15\", \"INVALID\""
                "resetControlPin = bits,   U08, 10,[2:7],  \"Board Default\""
                FILE_CONTENTS "${FILE_CONTENTS}")

string(REPLACE 
                "vvt1Pin      = bits  , U08,      13, [0:5],      \"Board Default\", \"A1\", \"B1\", \"C1\", \"D1\", \"E1\", \"F1\", \"G1\", \"H1\", \"J1\", \"K1\", \"L1\", \"M1\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"A2\", \"B2\", \"C2\", \"D2\", \"E2\", \"F2\", \"G2\", \"H2\", \"J2\", \"K2\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"B3\", \"C3\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"A4\", \"B4\", \"C4\", \"D4\", \"E4\", \"F4\", \"G4\", \"H4\", \"J4\", \"K4\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\""
                "vvt1Pin      = bits  , U08,      13, [0:5],      \"INVALID\", \"A1\", \"B1\", \"C1\", \"D1\", \"E1\", \"F1\", \"G1\", \"H1\", \"J1\", \"K1\", \"L1\", \"M1\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"A2\", \"B2\", \"C2\", \"D2\", \"E2\", \"F2\", \"G2\", \"H2\", \"J2\", \"K2\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\""
                FILE_CONTENTS "${FILE_CONTENTS}")

string(REPLACE 
                "vvt2Pin             = bits  , U08,     173, [0:5], \"Board Default\", \"A1\", \"B1\", \"C1\", \"D1\", \"E1\", \"F1\", \"G1\", \"H1\", \"J1\", \"K1\", \"L1\", \"M1\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"A2\", \"B2\", \"C2\", \"D2\", \"E2\", \"F2\", \"G2\", \"H2\", \"J2\", \"K2\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"B3\", \"C3\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"A4\", \"B4\", \"C4\", \"D4\", \"E4\", \"F4\", \"G4\", \"H4\", \"J4\", \"K4\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\""
                "vvt2Pin             = bits  , U08,     173, [0:5], \"INVALID\", \"A1\", \"B1\", \"C1\", \"D1\", \"E1\", \"F1\", \"G1\", \"H1\", \"J1\", \"K1\", \"L1\", \"M1\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"A2\", \"B2\", \"C2\", \"D2\", \"E2\", \"F2\", \"G2\", \"H2\", \"J2\", \"K2\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\""
                FILE_CONTENTS "${FILE_CONTENTS}")

string(REPLACE 
                "boostPin    =   bits, U08,      14, [0:5],      \"Board Default\", \"A1\", \"B1\", \"C1\", \"D1\", \"E1\", \"F1\", \"G1\", \"H1\", \"J1\", \"K1\", \"L1\", \"M1\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"A2\", \"B2\", \"C2\", \"D2\", \"E2\", \"F2\", \"G2\", \"H2\", \"J2\", \"K2\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"B3\", \"C3\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"A4\", \"B4\", \"C4\", \"D4\", \"E4\", \"F4\", \"G4\", \"H4\", \"J4\", \"K4\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\""
                "boostPin    =   bits, U08,      14, [0:5],      \"INVALID\", \"A1\", \"B1\", \"C1\", \"D1\", \"E1\", \"F1\", \"G1\", \"H1\", \"J1\", \"K1\", \"L1\", \"M1\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"A2\", \"B2\", \"C2\", \"D2\", \"E2\", \"F2\", \"G2\", \"H2\", \"J2\", \"K2\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\""
                FILE_CONTENTS "${FILE_CONTENTS}")

string(REPLACE 
                "launchPin   = bits  , U08,      48, [0:5],      \"Board Default\", \"A1\", \"B1\", \"C1\", \"D1\", \"E1\", \"F1\", \"G1\", \"H1\", \"J1\", \"K1\", \"L1\", \"M1\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"A2\", \"B2\", \"C2\", \"D2\", \"E2\", \"F2\", \"G2\", \"H2\", \"J2\", \"K2\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"B3\", \"C3\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"A4\", \"B4\", \"C4\", \"D4\", \"E4\", \"F4\", \"G4\", \"H4\", \"J4\", \"K4\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\""
                "launchPin   = bits  , U08,      48, [0:5],      \"INVALID\", \"A1\", \"B1\", \"C1\", \"D1\", \"E1\", \"F1\", \"G1\", \"H1\", \"INVALID\", \"INVALID\", \"L1\", \"M1\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"A2\", \"B2\", \"C2\", \"D2\", \"E2\", \"F2\", \"G2\", \"H2\", \"J2\", \"K2\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"B3\", \"C3\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"A4\", \"B4\", \"C4\", \"D4\", \"E4\", \"F4\", \"G4\", \"H4\", \"J4\", \"K4\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\""
                FILE_CONTENTS "${FILE_CONTENTS}")

string(REPLACE 
                "fanPin       = bits,   U08,     120, [2:7], \"Board Default\", \"A1\", \"B1\", \"C1\", \"D1\", \"E1\", \"F1\", \"G1\", \"H1\", \"J1\", \"K1\", \"L1\", \"M1\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"A2\", \"B2\", \"C2\", \"D2\", \"E2\", \"F2\", \"G2\", \"H2\", \"J2\", \"K2\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"B3\", \"C3\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"A4\", \"B4\", \"C4\", \"D4\", \"E4\", \"F4\", \"G4\", \"H4\", \"J4\", \"K4\", \"INVALID\", \"A8\", \"A9\", \"A10\", \"A11\", \"A12\", \"A13\", \"A14\", \"A15\", \"INVALID\""
                "fanPin       = bits,   U08,     120, [2:7], \"INVALID\", \"A1\", \"B1\", \"C1\", \"D1\", \"E1\", \"F1\", \"G1\", \"H1\", \"J1\", \"K1\", \"L1\", \"M1\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"A2\", \"B2\", \"C2\", \"D2\", \"E2\", \"F2\", \"G2\", \"H2\", \"J2\", \"K2\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\""
                FILE_CONTENTS "${FILE_CONTENTS}")

string(REPLACE 
                "wmiIndicatorPin      = bits,   U08,     156, [1:6], \"Board Default\", $DIGITAL_PIN"
                "wmiIndicatorPin      = bits,   U08,     156, [1:6], \"INVALID\", \"A1\", \"B1\", \"C1\", \"D1\", \"E1\", \"F1\", \"G1\", \"H1\", \"J1\", \"K1\", \"L1\", \"M1\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"A2\", \"B2\", \"C2\", \"D2\", \"E2\", \"F2\", \"G2\", \"H2\", \"J2\", \"K2\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\""
                FILE_CONTENTS "${FILE_CONTENTS}")

string(REPLACE 
                "wmiEmptyPin      = bits,   U08,     157, [1:6], \"Board Default\", $DIGITAL_PIN"
                "wmiEmptyPin      = bits,   U08,     157, [1:6], \"INVALID\", $DIGITAL_PIN"
                FILE_CONTENTS "${FILE_CONTENTS}")

string(REPLACE 
                "wmiEnabledPin   = bits,   U08,     158, [0:5], \"Board Default\", $DIGITAL_PIN"
                "wmiEnabledPin   = bits,   U08,     158, [0:5], \"INVALID\", \"A1\", \"B1\", \"C1\", \"D1\", \"E1\", \"F1\", \"G1\", \"H1\", \"J1\", \"K1\", \"L1\", \"M1\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"A2\", \"B2\", \"C2\", \"D2\", \"E2\", \"F2\", \"G2\", \"H2\", \"J2\", \"K2\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\""
                FILE_CONTENTS "${FILE_CONTENTS}")

string(REPLACE 
                "airConCompPin                 = bits,    U08,   84,  [0:5], \"Unused\", \"A1\", \"B1\", \"C1\", \"D1\", \"E1\", \"F1\", \"G1\", \"H1\", \"J1\", \"K1\", \"L1\", \"M1\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"A2\", \"B2\", \"C2\", \"D2\", \"E2\", \"F2\", \"G2\", \"H2\", \"J2\", \"K2\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"B3\", \"C3\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"A4\", \"B4\", \"C4\", \"D4\", \"E4\", \"F4\", \"G4\", \"H4\", \"J4\", \"K4\", \"INVALID\", \"A8\", \"A9\", \"A10\", \"A11\", \"A12\", \"A13\", \"A14\", \"A15\", \"INVALID\""
                "airConCompPin                 = bits,    U08,   84,  [0:5], \"Unused\", \"A1\", \"B1\", \"C1\", \"D1\", \"E1\", \"F1\", \"G1\", \"H1\", \"J1\", \"K1\", \"L1\", \"M1\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"A2\", \"B2\", \"C2\", \"D2\", \"E2\", \"F2\", \"G2\", \"H2\", \"J2\", \"K2\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\""
                FILE_CONTENTS "${FILE_CONTENTS}")

string(REPLACE 
                "airConReqPin                  = bits,    U08,   85,  [0:5], \"Unused\", \"A1\", \"B1\", \"C1\", \"D1\", \"E1\", \"F1\", \"G1\", \"H1\", \"J1\", \"K1\", \"L1\", \"M1\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"A2\", \"B2\", \"C2\", \"D2\", \"E2\", \"F2\", \"G2\", \"H2\", \"J2\", \"K2\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"B3\", \"C3\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"A4\", \"B4\", \"C4\", \"D4\", \"E4\", \"F4\", \"G4\", \"H4\", \"J4\", \"K4\", \"INVALID\", \"A8\", \"A9\", \"A10\", \"A11\", \"A12\", \"A13\", \"A14\", \"A15\", \"INVALID\""
                "airConReqPin                  = bits,    U08,   85,  [0:5], \"Unused\", \"A1\", \"B1\", \"C1\", \"D1\", \"E1\", \"F1\", \"G1\", \"H1\", \"INVALID\", \"INVALID\", \"L1\", \"M1\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"A2\", \"B2\", \"C2\", \"D2\", \"E2\", \"F2\", \"G2\", \"H2\", \"J2\", \"K2\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"B3\", \"C3\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"A4\", \"B4\", \"C4\", \"D4\", \"E4\", \"F4\", \"G4\", \"H4\", \"J4\", \"K4\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\", \"INVALID\""
                FILE_CONTENTS "${FILE_CONTENTS}")

file(WRITE speeduino.ini "${FILE_CONTENTS}")

set(TARGET		W806Speeduino)

project(${TARGET} C CXX ASM)

set(LIBRARIES	libs)

include_directories(include)
include_directories(${CMAKE_BINARY_DIR}/_deps/embeddedioservices-src/HardwareAbstractions/W80x/include)
include_directories(${CMAKE_BINARY_DIR}/_deps/speeduino-src/speeduino)

file(GLOB_RECURSE USER_CSOURCES src/*.c)
file(GLOB_RECURSE USER_CPPSOURCES src/*.cpp)
file(GLOB_RECURSE EmbeddedIOService_SOURCES ${CMAKE_BINARY_DIR}/_deps/embeddedioservices-src/HardwareAbstractions/W80x/src/*.cpp)
configure_file(${CMAKE_BINARY_DIR}/_deps/speeduino-src/speeduino/speeduino.ino ${CMAKE_BINARY_DIR}/_deps/speeduino-src/speeduino/speeduino.cpp) #add speeduino.ino
file(GLOB_RECURSE Speeduino_SOURCES ${CMAKE_BINARY_DIR}/_deps/speeduino-src/speeduino/*.cpp)
list(FILTER Speeduino_SOURCES EXCLUDE REGEX "FRAM/") # remove FRAM
list(FILTER Speeduino_SOURCES EXCLUDE REGEX "SPIAsEEPROM/") # remove SPIAsEEPROM
list(FILTER Speeduino_SOURCES EXCLUDE REGEX "init.cpp") # remove init
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
