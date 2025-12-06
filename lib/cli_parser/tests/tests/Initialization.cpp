#include "cli_parser.hpp"

#include <gtest/gtest.h>

TEST(cli_parsing, empty) {
    const char *argv[] = {"prog_name", nullptr};
    cli_parser::Parser p(1, argv);
    EXPECT_FALSE(p.hasFlag("-p"));
    EXPECT_EQ(p.getValue("-p"), "");
}

TEST(cli_parsing, one_empty_flag) {
    const char *argv[] = {"prog_name", "-p", nullptr}; // Utilisez NULL en C
    cli_parser::Parser p(2, argv);
    EXPECT_TRUE(p.hasFlag("-p"));
    EXPECT_EQ(p.getValue("-p"), "");
}

TEST(cli_parsing, one_flag) {
    const char *argv[] = {"prog_name", "-p", "p_flag", nullptr}; // Utilisez NULL en C
    cli_parser::Parser p(3, argv);
    EXPECT_TRUE(p.hasFlag("-p"));
    EXPECT_EQ(p.getValue("-p"), "p_flag");
}

TEST(cli_parsing, multiple_flags_with_value) {
    const char *argv[] = {"prog_name", "-p", "p_flag", "-h", "h_flag", nullptr}; // Utilisez NULL en C
    cli_parser::Parser p(5, argv);
    EXPECT_TRUE(p.hasFlag("-p"));
    EXPECT_EQ(p.getValue("-p"), "p_flag");
    EXPECT_TRUE(p.hasFlag("-h"));
    EXPECT_EQ(p.getValue("-h"), "h_flag");
}

TEST(cli_parsing, multiple_toggle_flags) {
    const char *argv[] = {"prog_name", "-p", "-d", nullptr}; // Utilisez NULL en C
    cli_parser::Parser p(3, argv);
    EXPECT_TRUE(p.hasFlag("-p"));
    EXPECT_TRUE(p.hasFlag("-d"));
}

TEST(cli_parsing, toggle_flag_and_value_flag) {
    const char *argv[] = {"prog_name", "-p", "p_flag", "-d", nullptr}; // Utilisez NULL en C
    cli_parser::Parser p(4, argv);
    EXPECT_TRUE(p.hasFlag("-p"));
    EXPECT_EQ(p.getValue("-p"), "p_flag");
    EXPECT_TRUE(p.hasFlag("-d"));
}
