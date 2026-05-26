#include <fstream>
#include <iostream>

#include <gtest/gtest.h>

#include "../src/file_getter.h"

#define TEST_RESOURCES_DIR "test_resources"

namespace
{
class TestFileGetter : public testing::Test
{
public:
    char buffer[1024];

protected:
    void SetUp() override
    {
        file_getter::root_dir = std::filesystem::current_path() / TEST_RESOURCES_DIR;
        file_getter::single_file_mode = false;
        memset(buffer, 0, sizeof(buffer));
    }

    /**
     * @return output file path
     */
    std::filesystem::path copy_file_via_file_getter(const std::string& file_name)
    {
        const auto out_file = file_getter::root_dir / (file_name + ".out");
        auto getter = file_getter::build_getter(file_name.c_str(), buffer, sizeof(buffer));
        long long count = 0;
        std::ofstream ofs(out_file, std::ios::binary);
        do // NOLINT(*-avoid-do-while)
        {
            count = getter();
            ofs.write(buffer, count);
        } while (count != 0);
        return out_file;
    }

    static std::string calc_sha256(const std::filesystem::path& file)
    {
        const std::string sha_file = file.string().append(".sha");

#ifdef _WIN32
        std::system( // NOLINT(*-command-processor, *-env33-c, *-mt-unsafe)
            ("powershell -Command \"(Get-FileHash " + file.string() + ").Hash\" > " + sha_file).c_str());
#elifdef __linux__
        std::system(("sha256sum -b " + file.string() + " | cut -d' ' -f1 > " + sha_file).c_str()); // NOLINT(*-command-processor)
#endif

        char b[65];
        {
            std::ifstream ifs(sha_file, std::ios::binary);
            ifs.getline(b, 65);
        }
        std::filesystem::remove(sha_file);
        std::string str = {b};
        std::ranges::transform(str, str.begin(), toupper);
        return str;
    }
};

TEST_F(TestFileGetter, test_resources_in_correct_location)
{
    auto getter = file_getter::build_getter("single_char.txt", buffer, sizeof(buffer));
    ASSERT_EQ(1, getter());
    ASSERT_EQ('A', buffer[0]);
}

TEST_F(TestFileGetter, empty_file)
{
    auto getter = file_getter::build_getter("empty.txt", buffer, sizeof(buffer));
    ASSERT_EQ(0, getter());
    ASSERT_EQ('\0', buffer[0]);
}

TEST_F(TestFileGetter, non_existing_file)
{
    auto getter = file_getter::build_getter("non-existing-file.txt", buffer, sizeof(buffer));
    ASSERT_EQ(0, getter());
    ASSERT_EQ('\0', buffer[0]);
}

TEST_F(TestFileGetter, single_file_mode)
{
    file_getter::single_file_mode = true;
    file_getter::root_dir = std::filesystem::current_path() / TEST_RESOURCES_DIR / "single_char.txt";
    auto getter = file_getter::build_getter("any-input", buffer, sizeof(buffer));
    ASSERT_EQ(1, getter());
    ASSERT_EQ('A', buffer[0]);
}

TEST_F(TestFileGetter, text_file)
{
    const auto out_path = copy_file_via_file_getter("the_little_prince.txt");
    ASSERT_EQ("7FA80B27F53782ABA750F3730ED3431270D79F9F7CD9FFDBA3B65C9843FEA670", calc_sha256(out_path));
}

TEST_F(TestFileGetter, binary_file)
{
    const auto out_path = copy_file_via_file_getter("cat");
    ASSERT_EQ("A63158E6E5BCE20616425F5D61E5BD7374BB5BCCF15BBB93AE2E40238248F179", calc_sha256(out_path));
}

} // namespace
