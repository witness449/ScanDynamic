#include "scan.hpp"
#include <gtest/gtest.h>
#include <print>
#include <string_view>

//Позитивный тест - одно значение
TEST(ScanTest, SimpleTest) {
    auto result = stdx::scan<std::string>("number", "{}");
    auto [val]=result.value().values();
    EXPECT_EQ("number", val);
}

//Позитивный тест - все поддерживаемые типы
TEST(ScanTest, AllTypes) {
    auto result = stdx::scan<int8_t, int16_t, int32_t, int64_t, uint8_t, uint16_t, uint32_t, uint64_t, std::string_view, float, double >
    ("-8 -16 -32 -64 8 16 32 64 strv 3.14 3.14159", "{%d} {%d} {%d} {%d} {%u} {%u} {%u} {%u} {%s} {%f} {%f}");
    ASSERT_TRUE(result.has_value());
    auto [i8, i16, i32, i64, u8, u16, u32, u64, sv, f, d]=result.value().values();
    EXPECT_EQ(-8, i8);
    EXPECT_EQ(-16, i16);
    EXPECT_EQ(-32, i32);
    EXPECT_EQ(-64, i64);
    EXPECT_EQ(8, u8);
    EXPECT_EQ(16, u16);
    EXPECT_EQ(32, u32);
    EXPECT_EQ(64, u64);
    EXPECT_EQ("strv", sv);
    float pi=3.14;
    EXPECT_EQ(pi, f);
    EXPECT_EQ(3.14159, d);
}

//Позитивный тест - два значения
TEST (ScanTest, StringFormat){
    auto result = stdx::scan<std::string, float>("I want to sum 42 and 3.14 numbers", "I want to sum {} and {%f} numbers");
    ASSERT_TRUE(result.has_value());
    auto [s, f]=result.value().values();
    EXPECT_EQ("42", s);
    float pi=3.14;
    EXPECT_EQ(pi, f);
}

//Позитивный тест - константные типы
TEST (ScanTest, ConstTypes){
    auto result = stdx::scan<const std::string, const float>("I want to sum 42 and 3.14 numbers", "I want to sum {} and {%f} numbers");
    ASSERT_TRUE(result.has_value());
    auto [s, f]=result.value().values();
    EXPECT_EQ("42", s);
    const float pi=3.14;
    EXPECT_EQ(pi, f);
}

//Негативный тест - некорректные спецификаторы формата
TEST (ScanTest, WrongFormats){
    auto result = stdx::scan<const std::string, const float>("I want to sum 42 and 3.14 numbers", "I want to sum {%g} and {%ff} numbers");
    ASSERT_FALSE(result.has_value());
    std::string error =result.error().message;
    std::string error_check="Incorrect format specifier %g\nIncorrect format specifier %ff\n";
    EXPECT_EQ(error_check, error);   
}

//Нешативный тест - ссылочные типы
TEST (ScanTest, ReferenceTypes){
    auto result = stdx::scan<std::string&, float>("I want to sum 42 and 3.14 numbers", "I want to sum {} and {%f} numbers");
    ASSERT_FALSE(result.has_value());
    std::string error =result.error().message;
    std::string error_check="Incorrect used types";
    EXPECT_EQ(error_check, error);  
}

//Негативный тест - запрещенные типы
TEST (ScanTest, OtherTypes){
    auto result = stdx::scan<char, float>("I want to sum 4 and 3.14 numbers", "I want to sum {} and {%f} numbers");
    ASSERT_FALSE(result.has_value());
    std::string error =result.error().message;
    std::string error_check="Incorrect used types";
    EXPECT_EQ(error_check, error);  
}

//Негативный тест несоответствие типа и значения  
TEST (ScanTest, NotNumber){
    auto result = stdx::scan<int, const float>("I want to sum int and 3.14 numbers", "I want to sum {%d} and {%f} numbers");
    ASSERT_FALSE(result.has_value());
    std::string error =result.error().message;
    std::string error_check="This is not a number\n";
    EXPECT_EQ(error_check, error);   
}

//Негативный тест несоответствие типа и спецификатора  
TEST (ScanTest, IncorrectSpecificator){
    auto result = stdx::scan<double, const float>("I want to sum 42 and 3.14 numbers", "I want to sum {%d} and {%f} numbers");
    ASSERT_FALSE(result.has_value());
    std::string error =result.error().message;
    std::string error_check="Incorrect format specifier %d\n";
    EXPECT_EQ(error_check, error);   
}

//Негативный тест - выход за диапазон 
TEST (ScanTest, OutOfRange){
    auto result = stdx::scan<int8_t, const float>("I want to sum 128 and 3.14 numbers", "I want to sum {%d} and {%f} numbers");
    ASSERT_FALSE(result.has_value());
    std::string error =result.error().message;
    std::string error_check="Number is out of range\n";
    EXPECT_EQ(error_check, error);   
}

//Негативный тест - плейсхолдеров меньше типов 
TEST(ScanTest, PlaceHoldersVsTypes) {
    auto result = stdx::scan<std::string, int>("number", "{}");
    std::string error =result.error().message;
    EXPECT_EQ("Types are not correspond placeholders", error);
}