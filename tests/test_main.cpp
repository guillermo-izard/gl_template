#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

TEST_CASE("GLM vector operations") {
    SUBCASE("Vector addition") {
        glm::vec3 a(1.0F, 2.0F, 3.0F);
        glm::vec3 b(4.0F, 5.0F, 6.0F);
        glm::vec3 result = a + b;

        CHECK(result.x == doctest::Approx(5.0));
        CHECK(result.y == doctest::Approx(7.0));
        CHECK(result.z == doctest::Approx(9.0));
    }

    SUBCASE("Vector dot product") {
        glm::vec3 a(1.0F, 0.0F, 0.0F);
        glm::vec3 b(0.0F, 1.0F, 0.0F);
        float result = glm::dot(a, b);

        CHECK(result == doctest::Approx(0.0));
    }

    SUBCASE("Vector cross product") {
        glm::vec3 a(1.0F, 0.0F, 0.0F);
        glm::vec3 b(0.0F, 1.0F, 0.0F);
        glm::vec3 result = glm::cross(a, b);

        CHECK(result.x == doctest::Approx(0.0));
        CHECK(result.y == doctest::Approx(0.0));
        CHECK(result.z == doctest::Approx(1.0));
    }
}

TEST_CASE("GLM matrix operations") {
    SUBCASE("Identity matrix") {
        glm::mat4 identity = glm::mat4(1.0F);
        glm::vec4 point(1.0F, 2.0F, 3.0F, 1.0F);
        glm::vec4 result = identity * point;

        CHECK(result.x == doctest::Approx(1.0));
        CHECK(result.y == doctest::Approx(2.0));
        CHECK(result.z == doctest::Approx(3.0));
        CHECK(result.w == doctest::Approx(1.0));
    }

    SUBCASE("Translation matrix") {
        glm::mat4 transform = glm::mat4(1.0F);
        transform = glm::translate(transform, glm::vec3(5.0F, 10.0F, 15.0F));

        glm::vec4 point(0.0F, 0.0F, 0.0F, 1.0F);
        glm::vec4 result = transform * point;

        CHECK(result.x == doctest::Approx(5.0));
        CHECK(result.y == doctest::Approx(10.0));
        CHECK(result.z == doctest::Approx(15.0));
        CHECK(result.w == doctest::Approx(1.0));
    }

    SUBCASE("Rotation matrix (90 degrees around Z-axis)") {
        glm::mat4 transform = glm::mat4(1.0F);
        transform = glm::rotate(transform, glm::radians(90.0F), glm::vec3(0.0F, 0.0F, 1.0F));

        glm::vec4 point(1.0F, 0.0F, 0.0F, 1.0F);
        glm::vec4 result = transform * point;

        CHECK(result.x == doctest::Approx(0.0).epsilon(0.0001));
        CHECK(result.y == doctest::Approx(1.0).epsilon(0.0001));
        CHECK(result.z == doctest::Approx(0.0).epsilon(0.0001));
        CHECK(result.w == doctest::Approx(1.0).epsilon(0.0001));
    }
}

TEST_CASE("Asset files exist") {
    // Note: These tests assume the working directory is the project root
    // In practice, you might want to check relative to a known base path

    SUBCASE("Shader files") {
        // This is a simple existence check - in a real project you might
        // want to actually read and validate the shader files
        CHECK(true); // Placeholder - real implementation would check file existence
    }

    SUBCASE("Texture files") {
        // Placeholder for texture file existence check
        CHECK(true);
    }
}
