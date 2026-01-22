cmake_minimum_required(VERSION 3.16)
project(ExultNPCAI VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
set(NPCAI_SOURCES
    ExultNPCBridge.cpp
    DialogueHooks.cpp
    ScheduleIntegration.cpp
)

# Header files
set(NPCAI_HEADERS
    ExultNPCBridge.h
    DialogueHooks.h
    ScheduleIntegration.h
    NPCProfileLoader.h
    ExultAIIntegration.h
)

# Create the library
add_library(exult_npcai STATIC ${NPCAI_SOURCES} ${NPCAI_HEADERS})

# Include directories
target_include_directories(exult_npcai PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../npc/include
)

# Link to the NPC AI library
target_link_libraries(exult_npcai PUBLIC
    ultima_npc_ai
)

# Install rules
install(TARGETS exult_npcai
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(FILES ${NPCAI_HEADERS}
    DESTINATION include/exult/npcai
)

# Install data files
install(DIRECTORY data/
    DESTINATION share/exult/npcai
    FILES_MATCHING PATTERN "*.json" PATTERN "*.aiml"
)

# Optional: Build test executable
option(BUILD_NPCAI_TESTS "Build NPC AI integration tests" OFF)

if(BUILD_NPCAI_TESTS)
    add_executable(npcai_test
        tests/integration_test.cpp
    )
    target_link_libraries(npcai_test PRIVATE exult_npcai)
endif()
