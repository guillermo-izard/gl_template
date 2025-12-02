# Code coverage configuration
# This module enables code coverage instrumentation for GCC and Clang

function(enable_coverage target)
    if(ENABLE_COVERAGE)
        if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
            message(STATUS "Code coverage enabled for ${target}")

            # Coverage flags
            set(COVERAGE_FLAGS
                --coverage
                -fprofile-arcs
                -ftest-coverage
                -fno-inline
                -fno-inline-small-functions
                -fno-default-inline
                -O0
            )

            target_compile_options(${target} PRIVATE ${COVERAGE_FLAGS})
            target_link_options(${target} PRIVATE --coverage)

            # Add gcov library for GCC
            if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
                target_link_libraries(${target} PRIVATE gcov)
            endif()
        else()
            message(WARNING "Code coverage is only supported with GCC and Clang")
        endif()
    endif()
endfunction()
