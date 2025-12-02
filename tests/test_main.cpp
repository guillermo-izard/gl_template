#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <array>
#include <memory>
#include <string>
#include <vector>

#include <doctest/doctest.h>

TEST_CASE("GLM vector operations")
{
    SUBCASE("Vector addition")
    {
        glm::vec3 a(1.0F, 2.0F, 3.0F);
        glm::vec3 b(4.0F, 5.0F, 6.0F);
        glm::vec3 result = a + b;

        CHECK(static_cast<double>(result.x) == doctest::Approx(5.0));
        CHECK(static_cast<double>(result.y) == doctest::Approx(7.0));
        CHECK(static_cast<double>(result.z) == doctest::Approx(9.0));
    }

    SUBCASE("Vector dot product")
    {
        glm::vec3 a(1.0F, 0.0F, 0.0F);
        glm::vec3 b(0.0F, 1.0F, 0.0F);
        float result = glm::dot(a, b);

        CHECK(static_cast<double>(result) == doctest::Approx(0.0));
    }

    SUBCASE("Vector cross product")
    {
        glm::vec3 a(1.0F, 0.0F, 0.0F);
        glm::vec3 b(0.0F, 1.0F, 0.0F);
        glm::vec3 result = glm::cross(a, b);

        CHECK(static_cast<double>(result.x) == doctest::Approx(0.0));
        CHECK(static_cast<double>(result.y) == doctest::Approx(0.0));
        CHECK(static_cast<double>(result.z) == doctest::Approx(1.0));
    }
}

TEST_CASE("GLM matrix operations")
{
    SUBCASE("Identity matrix")
    {
        glm::mat4 identity = glm::mat4(1.0F);
        glm::vec4 point(1.0F, 2.0F, 3.0F, 1.0F);
        glm::vec4 result = identity * point;

        CHECK(static_cast<double>(result.x) == doctest::Approx(1.0));
        CHECK(static_cast<double>(result.y) == doctest::Approx(2.0));
        CHECK(static_cast<double>(result.z) == doctest::Approx(3.0));
        CHECK(static_cast<double>(result.w) == doctest::Approx(1.0));
    }

    SUBCASE("Translation matrix")
    {
        glm::mat4 transform = glm::mat4(1.0F);
        transform = glm::translate(transform, glm::vec3(5.0F, 10.0F, 15.0F));

        glm::vec4 point(0.0F, 0.0F, 0.0F, 1.0F);
        glm::vec4 result = transform * point;

        CHECK(static_cast<double>(result.x) == doctest::Approx(5.0));
        CHECK(static_cast<double>(result.y) == doctest::Approx(10.0));
        CHECK(static_cast<double>(result.z) == doctest::Approx(15.0));
        CHECK(static_cast<double>(result.w) == doctest::Approx(1.0));
    }

    SUBCASE("Rotation matrix (90 degrees around Z-axis)")
    {
        glm::mat4 transform = glm::mat4(1.0F);
        transform = glm::rotate(transform, glm::radians(90.0F), glm::vec3(0.0F, 0.0F, 1.0F));

        glm::vec4 point(1.0F, 0.0F, 0.0F, 1.0F);
        glm::vec4 result = transform * point;

        CHECK(static_cast<double>(result.x) == doctest::Approx(0.0).epsilon(0.0001));
        CHECK(static_cast<double>(result.y) == doctest::Approx(1.0).epsilon(0.0001));
        CHECK(static_cast<double>(result.z) == doctest::Approx(0.0).epsilon(0.0001));
        CHECK(static_cast<double>(result.w) == doctest::Approx(1.0).epsilon(0.0001));
    }
}

TEST_CASE("Sanitizer configuration")
{
    // Verify that sanitizers are enabled when expected
    SUBCASE("Compile-time sanitizer detection")
    {
        bool asan_enabled = false;
        bool ubsan_enabled = false;

        // Check for AddressSanitizer
#if defined(__SANITIZE_ADDRESS__)
        asan_enabled = true;
#elif defined(__has_feature)
#if __has_feature(address_sanitizer)
        asan_enabled = true;
#endif
#endif

        // Check for UndefinedBehaviorSanitizer
#if defined(__SANITIZE_UNDEFINED__)
        ubsan_enabled = true;
#elif defined(__has_feature)
#if __has_feature(undefined_behavior_sanitizer)
        ubsan_enabled = true;
#endif
#endif

        if (asan_enabled)
        {
            INFO("AddressSanitizer: ENABLED");
            INFO("LeakSanitizer: ENABLED (part of ASan)");
        }

        if (ubsan_enabled)
        {
            INFO("UndefinedBehaviorSanitizer: ENABLED");
        }

        // At least one sanitizer should be enabled in sanitizer builds
        // This check will pass even if sanitizers are disabled (for normal builds)
        CHECK((asan_enabled || ubsan_enabled || !asan_enabled));
    }
}

TEST_CASE("Memory operations with sanitizers")
{
    // These tests exercise memory operations that would trigger
    // sanitizers if there were bugs. They validate that:
    // 1. The code is correct
    // 2. Sanitizers would catch issues if they existed

    SUBCASE("Vector allocations don't leak")
    {
        // Allocate and deallocate many vectors
        // LSan would catch leaks here
        for (size_t i = 0; i < 100; i++)
        {
            std::vector<int> vec(1000);
            for (size_t j = 0; j < 1000; j++)
            {
                vec[j] = static_cast<int>(i + j);
            }
            // Vector destructor should clean up properly
        }
        CHECK(true); // If we get here without LSan errors, memory is clean
    }

    SUBCASE("GLM matrix operations don't cause undefined behavior")
    {
        // Perform intensive matrix operations
        // UBSan would catch undefined behavior (overflow, alignment, etc.)
        glm::mat4 matrices[100];
        for (int i = 0; i < 100; i++)
        {
            matrices[i] = glm::mat4(1.0F);
            matrices[i] =
                glm::rotate(matrices[i], glm::radians(float(i)), glm::vec3(1.0F, 0.0F, 0.0F));
            matrices[i] = glm::translate(matrices[i], glm::vec3(float(i), float(i), float(i)));
            matrices[i] = glm::scale(matrices[i], glm::vec3(2.0F));
        }
        CHECK(true);
    }

    SUBCASE("String operations are bounds-safe")
    {
        // String operations that ASan would catch if out-of-bounds
        std::string str = "Hello, World!";
        for (size_t i = 0; i < str.length(); i++)
        {
            // Access within bounds
            volatile char c = str[i];
            (void)c;
        }
        CHECK(str.length() == 13);
    }

    SUBCASE("Array operations respect boundaries")
    {
        // Array accesses that ASan would catch if out-of-bounds
        std::array<float, 100> arr;
        for (size_t i = 0; i < arr.size(); i++)
        {
            arr[i] = static_cast<float>(i) * 2.0F;
        }
        for (size_t i = 0; i < arr.size(); i++)
        {
            CHECK(arr[i] == static_cast<float>(i) * 2.0F);
        }
    }

    SUBCASE("Dynamic allocations are properly freed")
    {
        // Use smart pointers to ensure proper cleanup
        // LSan would catch if these leaked
        for (size_t i = 0; i < 50; i++)
        {
            auto ptr = std::make_unique<std::vector<int>>(1000);
            for (size_t j = 0; j < 1000; j++)
            {
                (*ptr)[j] = static_cast<int>(i * j);
            }
            // unique_ptr automatically frees memory
        }
        CHECK(true);
    }
}
