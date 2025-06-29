#pragma once

#include <concepts>
#include <exception>
#include <expected>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>
#include <print>
#include "types.hpp"
#include <array>
#include <iostream>
#include <type_traits>

namespace stdx::details {

template< class T >
concept d = std::same_as<T, int>;

template< class T >
concept u = std::same_as<T, unsigned int>;

template< class T >
concept f = std::same_as<T, float>||std::same_as<T, double>;

template< class T >
concept s = std::same_as<T, std::string>||std::same_as<T, std::string_view>;

template <class T>
concept number=d<T>||u<T>||f<T>;

template <number T>
std::expected<T, scan_error> parse_value (std::string_view& input){
    T result;
    auto [ptr, ec] = std::from_chars(input.data()+1, input.data() + input.size()-1, result);
    if (ec == std::errc()){
        return result;
    }
    else if (ec == std::errc::invalid_argument){
        return std::unexpected(scan_error{"This is not a number"});
    }
    else if (ec == std::errc::result_out_of_range){
        return std::unexpected(scan_error{"Value is out of range"});
    }
}

template <s T> 
std::expected<T, scan_error>parse_value(std::string_view &input){
    return T{input.begin(), input.end()};
}


// Функция для парсинга значения с учетом спецификатора формата
template <typename T>
std::expected<T, scan_error> parse_value_with_format(std::string_view input, std::string_view fmt) {
    bool format=[& fmt]()->bool {
        if constexpr (d<T>){
            return fmt.substr(0, 2)=="%d";
            }
        else if constexpr (f<T>){
            return fmt.substr(0, 2)=="%f";
        }
        else if constexpr (u<T>){
            return fmt.substr(0, 2)=="%u";
        }
        else if constexpr (s<T>){
            return fmt.substr(0, 2)=="%s";
        }
        else return false;
    }();
         
    if(format){
        auto result=parse_value<T>(input);
        return result;
    }
    else{
        std::string fmt_str{fmt.begin(), fmt.end()};
        return std::unexpected(scan_error{"Incorrect format specifier "+fmt_str});
    }
}

// Функция для проверки корректности входных данных и выделения из обеих строк интересующих данных для парсинга
template <typename... Ts>
std::expected<std::pair<std::vector<std::string_view>, std::vector<std::string_view>>, scan_error>
parse_sources(std::string_view input, std::string_view format) {
    std::vector<std::string_view> format_parts;  // Части формата между {}
    std::vector<std::string_view> input_parts;
    size_t start = 0;
    while (true) {
        size_t open = format.find('{', start);
        if (open == std::string_view::npos) {
            break;
        }
        size_t close = format.find('}', open);
        if (close == std::string_view::npos) {
            break;
        }

        // Если между предыдущей } и текущей { есть текст,
        // проверяем его наличие во входной строке
        if (open > start) {
            std::string_view between = format.substr(start, open - start);
            auto pos = input.find(between);
            if (input.size() < between.size() || pos == std::string_view::npos) {
                return std::unexpected(scan_error{"Unformatted text in input and format string are different"});
            }
            if (start != 0) {
                input_parts.emplace_back(input.substr(0, pos));
            }

            input = input.substr(pos + between.size());
        }

        // Сохраняем спецификатор формата (то, что между {})
        format_parts.push_back(format.substr(open + 1, close - open - 1));
        start = close + 1;
    }

    // Проверяем оставшийся текст после последней }
    if (start < format.size()) {
        std::string_view remaining_format = format.substr(start);
        auto pos = input.find(remaining_format);
        if (input.size() < remaining_format.size() || pos == std::string_view::npos) {
            return std::unexpected(scan_error{"Unformatted text in input and format string are different"});
        }
        input_parts.emplace_back(input.substr(0, pos));
        input = input.substr(pos + remaining_format.size());
    } else {
        input_parts.emplace_back(input);
    }
    return std::pair{format_parts, input_parts};
}

} // namespace stdx::details