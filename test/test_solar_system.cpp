#include <gtest/gtest.h>
#include "solar_system_calculator.h"
#include "glm/glm.hpp"
#include <vector>

TEST(SolarSystemTest, BodiesAreInitialized) {
    EXPECT_NO_THROW({
        SolarSystem solar_system{};
        solar_system.init();
        EXPECT_EQ(2, solar_system.bodies.size());
    });
}

TEST(SolarSystemTest, ForcesAreCorrect) {
    EXPECT_NO_THROW({
        SolarSystem solar_system{};
        solar_system.init();
        solar_system.computeForces(solar_system.bodies);
        EXPECT_FLOAT_EQ(3.5907324e22, solar_system.bodies[0].force.x);
        EXPECT_FLOAT_EQ(0, solar_system.bodies[0].force.y);
        EXPECT_FLOAT_EQ(0, solar_system.bodies[0].force.z);
        EXPECT_FLOAT_EQ(-3.5907324e22, solar_system.bodies[1].force.x);
        EXPECT_FLOAT_EQ(0, solar_system.bodies[1].force.y);
        EXPECT_FLOAT_EQ(0, solar_system.bodies[1].force.z);

        solar_system.updateBodies(8.0);
        EXPECT_FLOAT_EQ(1.1490343e-6, solar_system.bodies[0].position.x);
        EXPECT_FLOAT_EQ(0.0, solar_system.bodies[0].position.y);
        EXPECT_FLOAT_EQ(0.0, solar_system.bodies[0].position.z);

        EXPECT_FLOAT_EQ(1.4362929e-7, solar_system.bodies[0].velocity.x);
        EXPECT_FLOAT_EQ(0.0, solar_system.bodies[0].velocity.y);
        EXPECT_FLOAT_EQ(0.0, solar_system.bodies[0].velocity.z);

        EXPECT_FLOAT_EQ(1.49e11, solar_system.bodies[1].position.x);
        EXPECT_FLOAT_EQ(0.0, solar_system.bodies[1].position.y);
        EXPECT_FLOAT_EQ(0.0, solar_system.bodies[1].position.z);

        EXPECT_FLOAT_EQ(-0.0481009, solar_system.bodies[1].velocity.x);
        EXPECT_FLOAT_EQ(0.0, solar_system.bodies[1].velocity.y);
        EXPECT_FLOAT_EQ(0.0, solar_system.bodies[1].velocity.z);
    });
}


