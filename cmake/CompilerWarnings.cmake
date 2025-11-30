# Compiler-specific warning flags
# This module sets strict warning levels and treats warnings as errors

function(set_project_warnings target)
    set(MSVC_WARNINGS
        /W4         # High warning level
        /WX         # Treat warnings as errors
        /permissive- # Standards conformance
        /w14640     # Thread-unsafe static member initialization
        /w14242     # Possible loss of data
        /w14254     # Operator with different sizes
        /w14263     # Member function does not override
        /w14265     # Class has virtual functions but destructor is not virtual
        /w14287     # Unsigned/negative constant mismatch
        /we4289     # Loop control variable used outside loop
        /w14296     # Expression is always true/false
        /w14311     # Pointer truncation
        /w14545     # Expression before comma evaluates to a function
        /w14546     # Function call before comma missing argument list
        /w14547     # Operator before comma has no effect
        /w14549     # Operator before comma has no effect
        /w14555     # Expression has no effect
        /w14619     # Pragma warning with non-existent warning number
        /w14826     # Conversion is sign-extended
        /w14905     # Wide string literal cast
        /w14906     # String literal cast
        /w14928     # Illegal copy-initialization
    )

    set(CLANG_WARNINGS
        -Wall
        -Wextra
        -Wpedantic
        -Werror
        -Wshadow
        -Wnon-virtual-dtor
        -Wold-style-cast
        -Wcast-align
        -Wunused
        -Woverloaded-virtual
        -Wconversion
        -Wsign-conversion
        -Wformat=2
        -Wimplicit-fallthrough
        -Wnull-dereference
        -Wdouble-promotion
    )

    set(GCC_WARNINGS
        ${CLANG_WARNINGS}
        -Wmisleading-indentation
        -Wduplicated-cond
        -Wduplicated-branches
        -Wlogical-op
        -Wuseless-cast
    )

    if(MSVC)
        set(PROJECT_WARNINGS ${MSVC_WARNINGS})
    elseif(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
        set(PROJECT_WARNINGS ${CLANG_WARNINGS})
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(PROJECT_WARNINGS ${GCC_WARNINGS})
    else()
        message(WARNING "No compiler warnings set for '${CMAKE_CXX_COMPILER_ID}' compiler.")
    endif()

    target_compile_options(${target} PRIVATE ${PROJECT_WARNINGS})
endfunction()
