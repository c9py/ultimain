cmake_minimum_required(VERSION 3.16)

enable_testing()

add_library(avatar_test_framework INTERFACE)
target_include_directories(avatar_test_framework INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/../../../tests)

function(add_avatar_test name source)
    add_executable(${name} ${source})
    target_link_libraries(${name} PRIVATE ultima_avatar avatar_test_framework)
    target_include_directories(${name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
    add_test(NAME ${name} COMMAND ${name})
endfunction()

add_avatar_test(test_facs test_facs.cpp)
add_avatar_test(test_rig_and_dna test_rig_and_dna.cpp)
add_avatar_test(test_live2d_meshy test_live2d_meshy.cpp)
add_avatar_test(test_deltecho_pipeline test_deltecho_pipeline.cpp)

add_custom_target(avatar_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS test_facs test_rig_and_dna test_live2d_meshy test_deltecho_pipeline
)
