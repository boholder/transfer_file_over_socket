#include <gtest/gtest.h>

#include "../src/file_getter.h"

namespace
{
class TestFileGetter : public testing::Test
{
protected:
    void SetUp() override
    { file_getter::root_dir = std::filesystem::current_path() / "test_resources"; }
};

TEST_F(TestFileGetter, test_resources_in_correct_location)
{
    char buffer[2];
    buffer[0] = '\0';
    auto getter = file_getter::build_getter("single_char.txt", buffer, 2);
    ASSERT_EQ(true, getter());
    ASSERT_EQ('A', buffer[0]);
}

} // namespace
