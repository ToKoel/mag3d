#include <gtest/gtest.h>
#include "file_loader.h"

TEST(FileLoaderTest, CanLoadValidObjFile) {
    const std::string path = "../test/data/arrow.obj";
    EXPECT_NO_THROW({
        const auto vertices = FileLoader::load_obj_file(path);
        EXPECT_EQ(vertices.size(), 558);
        EXPECT_FLOAT_EQ(vertices[0].position.x, 0.0);
        EXPECT_FLOAT_EQ(vertices[0].position.y, 0.75164);
        EXPECT_FLOAT_EQ(vertices[0].position.z, -0.108066);

        EXPECT_FLOAT_EQ(vertices[0].normal.x, 0.0980);
        EXPECT_FLOAT_EQ(vertices[0].normal.y, 0.0);
        EXPECT_FLOAT_EQ(vertices[0].normal.z, -0.9952);

        EXPECT_FLOAT_EQ(vertices[0].uvs.x, 1.0);
        EXPECT_FLOAT_EQ(vertices[0].uvs.y, 1.0);
    });
}

TEST(FileLoaderTest, ThrowsOnMissingFile) {
    const std::string path = "non_existent.obj";
    EXPECT_THROW({
        FileLoader::load_obj_file(path);
    }, std::runtime_error);
}

