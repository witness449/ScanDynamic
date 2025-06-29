#include <gtest/gtest.h>
#include "parse.hpp"
#include <print>

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    auto result=stdx::details::parse_value_with_format<std::string>("1.0", "%d");
    if(result.has_value()){
        std::cout<<*result<<std::endl;
    }
    else{
        std::string error=result.error().message;
        std::cout<<error<<std::endl;
    }
    //return RUN_ALL_TESTS();
}
