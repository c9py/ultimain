# Examples CMakeLists.txt
cmake_minimum_required(VERSION 3.16)

# NPC Demo
add_executable(npc_demo npc_demo.cpp)
target_link_libraries(npc_demo PRIVATE ultima_npc_ai)
target_include_directories(npc_demo PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Dialogue Demo (TinyLLM + Hybrid Dialogue)
add_executable(dialogue_demo dialogue_demo.cpp)
target_link_libraries(dialogue_demo PRIVATE ultima_npc_ai)
target_include_directories(dialogue_demo PRIVATE ${CMAKE_SOURCE_DIR}/include)
