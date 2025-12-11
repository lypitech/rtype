#include <gtest/gtest.h>

#include "cli_parser.hpp"

TEST(cli_types, integer)
{
    const char *argv[] = {"prog_name", "-p", "4242", nullptr};
    cli_parser::Parser p(3, argv);
    EXPECT_TRUE(p.hasFlag("-p"));
    EXPECT_EQ(p.getValue("-p").as<int>(), 4242.0);
}

TEST(cli_types, double)
{
    const char *argv[] = {"prog_name", "-p", "6.7", nullptr};
    cli_parser::Parser p(3, argv);
    EXPECT_TRUE(p.hasFlag("-p"));
    EXPECT_EQ(p.getValue("-p").as<double>(), 6.7);
}

TEST(cli_types, cast_int_to_double)
{
    const char *argv[] = {"prog_name", "-p", "67", nullptr};
    cli_parser::Parser p(3, argv);
    EXPECT_TRUE(p.hasFlag("-p"));
    EXPECT_EQ(p.getValue("-p").as<double>(), 67);
}

TEST(cli_types, string)
{
    const char *argv[] = {"prog_name", "-p", "p_flag", nullptr};
    cli_parser::Parser p(3, argv);
    EXPECT_TRUE(p.hasFlag("-p"));
    EXPECT_EQ(p.getValue("-p").as<std::string>(), "p_flag");
}

TEST(cli_types, boolean)
{
    const char *argv[] = {"prog_name", "-p", "true", nullptr};
    cli_parser::Parser p(3, argv);
    EXPECT_TRUE(p.hasFlag("-p"));
    EXPECT_TRUE(p.getValue("-p").as<bool>());
}

TEST(cli_types, boolean2)
{
    const char *argv[] = {"prog_name", "-p", "1", nullptr};
    cli_parser::Parser p(3, argv);
    EXPECT_TRUE(p.hasFlag("-p"));
    EXPECT_TRUE(p.getValue("-p").as<bool>());
}

