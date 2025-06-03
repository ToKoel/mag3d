#include <gtest/gtest.h>
#include "solar_system_calculator.h"
#include "glm/glm.hpp"
#include <vector>

TEST(SolarSystemTest, BodiesAreInitialized) {
    EXPECT_NO_THROW({
        SolarSystemCalculator solar_system{};
        solar_system.init();
        EXPECT_EQ(5, solar_system.bodies.size());
    });
}

TEST(SolarSystemTest, ForcesAreCorrect) {
        SolarSystemCalculator solar_system{};
        solar_system.init();
        for (int i = 0; i < 100; i++) {
            solar_system.update_bodies_verlet(0.1);
        }
        EXPECT_FLOAT_EQ(solar_system.bodies[3].position.x, 0.98523641);
        EXPECT_FLOAT_EQ(solar_system.bodies[3].position.y, 0.17114672);
        EXPECT_NEAR(solar_system.bodies[3].position.z, 0.0, 1e-8);
}


