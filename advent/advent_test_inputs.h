#pragma once

#define TEST_ONE_A "1abc2"
#define TEST_ONE_B "pqr3stu8vwx"
#define TEST_ONE_C "a1b2c3d4e5f"
#define TEST_ONE_D "treb7uchet"
constexpr const char* TEST_FILE_A = TEST_ONE_A "\n" TEST_ONE_B "\n" TEST_ONE_C "\n" TEST_ONE_D;
#define TEST_ONE_F "two1nine"
#define TEST_ONE_G "eightwothree"
#define TEST_ONE_H "abcone2threexyz"
#define TEST_ONE_I "xtwone3four"
#define TEST_ONE_J "4nineeightseven2"
#define TEST_ONE_K "zoneight234"
#define TEST_ONE_L "7pqrstsixteen"
constexpr const char* TEST_FILE_B = TEST_ONE_F "\n" TEST_ONE_G "\n" TEST_ONE_H "\n" TEST_ONE_I "\n" TEST_ONE_J "\n" TEST_ONE_K "\n" TEST_ONE_L;

constexpr const char* TEST_TWO_A = "Game 1: 3 blue, 4 red; 1 red, 2 green, 6 blue; 2 green";
constexpr const char* TEST_TWO_B = "Game 2: 1 blue, 2 green; 3 green, 4 blue, 1 red; 1 green, 1 blue";
constexpr const char* TEST_TWO_C = "Game 3: 8 green, 6 blue, 20 red; 5 blue, 4 red, 13 green; 5 green, 1 red";
constexpr const char* TEST_TWO_D = "Game 4: 1 green, 3 red, 6 blue; 3 green, 6 red; 3 green, 15 blue, 14 red";
constexpr const char* TEST_TWO_E = "Game 5: 6 red, 1 blue, 3 green; 2 blue, 1 red, 2 green";
constexpr const char* TEST_TWO_F = R"(Game 1: 3 blue, 4 red; 1 red, 2 green, 6 blue; 2 green
Game 2: 1 blue, 2 green; 3 green, 4 blue, 1 red; 1 green, 1 blue
Game 3: 8 green, 6 blue, 20 red; 5 blue, 4 red, 13 green; 5 green, 1 red
Game 4: 1 green, 3 red, 6 blue; 3 green, 6 red; 3 green, 15 blue, 14 red
Game 5: 6 red, 1 blue, 3 green; 2 blue, 1 red, 2 green)";