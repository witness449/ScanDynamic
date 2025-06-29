#include <gtest/gtest.h>
#include "parse.hpp"
#include <print>
#include "scan.hpp"

// Helper function to print a single element (with a comma separator)
template <typename T>
void print_element(const T& val, bool is_first) {
    if (!is_first) {
        std::cout << ", ";
    }
    std::cout << val;
}

// Function to print the tuple using index_sequence and a lambda
template <typename TupleT, std::size_t... Is>
void print_tuple_impl(const TupleT& tp, std::index_sequence<Is...>) {
    std::cout << "(";
    // Use a fold expression to iterate and print each element
    // The lambda captures `tp` by reference
    // The `is_first` boolean helps manage comma placement
    ([&](std::size_t index) {
        print_element(std::get<index>(tp), index == 0);
    }(Is), ...);
    std::cout << ")";
}

template <typename TupleT>
void print_tuple(const TupleT& tp) {
    print_tuple_impl(tp, std::make_index_sequence<std::tuple_size_v<TupleT>>{});
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    auto result=stdx::details::parse_value_with_format<int>("{10000000000000000000000000000}", "%d");
    auto result2=stdx::scan<int, unsigned int, std::string>("One {-100000} two {2} {dtriiiiiiink}", "One {%d} two {%u} {%s}");
    if(result.has_value()){
        std::cout<<*result<<std::endl;
    }
    else{
        std::string error=result.error().message;
        std::cout<<error<<std::endl;
    }
    if (result2.has_value()){
        std::apply([](auto&&... args) {((std::cout << args << '\n'), ...);}, result2->values());
    }
    else{
        std::string error=result2.error().message;
        std::cout<<error<<std::endl;
    }
    //return RUN_ALL_TESTS();
}


