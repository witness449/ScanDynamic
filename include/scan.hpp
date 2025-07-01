#pragma once

#include "parse.hpp"
#include "types.hpp"

namespace stdx {

// замените болванку функции scan на рабочую версию

template <typename... Ts>
std::expected<details::scan_result<Ts...>, details::scan_error> scan(std::string_view input, std::string_view format) {

    auto parsed_sources = details::parse_sources<Ts...>(input, format);
    if (!parsed_sources.has_value()) {
        return std::unexpected(parsed_sources.error());
    }
    auto [parsed_format, parsed_input] = parsed_sources.value();

    auto values = [&]<std::size_t... Is>(std::index_sequence<Is...>) {
        return std::make_tuple(details::parse_value_with_format<Ts>(parsed_input[Is], parsed_format[Is])...);
    }(std::index_sequence_for<Ts...>{});

    std::string parsed_format_error{};
    auto lambda = [&parsed_format_error](auto &arg) {
        if (!arg.has_value()) {
            parsed_format_error += arg.error().message + '\n';
        }
    };

    std::apply([&](auto &...arg) { (lambda(arg), ...); }, values);

    if (parsed_format_error.size() > 0) {
        return std::unexpected(details::scan_error{parsed_format_error});
    }

    auto result = [&]<std::size_t... Is>(std::index_sequence<Is...>) -> details::scan_result<Ts...> {
        return {std::make_tuple((std::move(std::get<Is>(values)).value())...)};
    }(std::index_sequence_for<Ts...>{});

    return result;
}

}  // namespace stdx
