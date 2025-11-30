# Sanitizers configuration
# Enables AddressSanitizer and UndefinedBehaviorSanitizer

function(enable_sanitizers target)
    if(NOT ENABLE_SANITIZERS)
        return()
    endif()

    message(STATUS "Enabling sanitizers for target: ${target}")

    if(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(SANITIZER_FLAGS
            -fsanitize=address
            -fsanitize=undefined
            -fsanitize=leak
            -fno-omit-frame-pointer
            -fno-optimize-sibling-calls
            -g
        )

        target_compile_options(${target} PRIVATE ${SANITIZER_FLAGS})
        target_link_options(${target} PRIVATE ${SANITIZER_FLAGS})

        message(STATUS "  - AddressSanitizer: ON")
        message(STATUS "  - UndefinedBehaviorSanitizer: ON")
        message(STATUS "  - LeakSanitizer: ON")

    elseif(MSVC)
        # MSVC has limited sanitizer support
        target_compile_options(${target} PRIVATE /fsanitize=address)

        message(STATUS "  - AddressSanitizer: ON")
        message(WARNING "  - UBSan not fully supported on MSVC")

    else()
        message(WARNING "Sanitizers not supported for compiler: ${CMAKE_CXX_COMPILER_ID}")
    endif()
endfunction()
