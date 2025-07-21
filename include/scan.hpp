#pragma once

#include "parse.hpp"
#include <type_traits>

namespace stdx {

// Проверка типов на корректность
template <details::correct_type T>
consteval bool checkType() {
    return true;
}

template <typename T>
consteval bool checkType() {
    return false;
}

template <typename... Ts>
std::expected<details::scan_result<Ts...>, details::scan_error> scan(std::string_view input, std::string_view format) {

    // Проверка переданных типов
    bool check_types = false;
    check_types = []<std::size_t... Is>(std::index_sequence<Is...>) -> bool {
        return (... && ([] -> bool {
                    if constexpr (!checkType<Ts>()) {
                        return false;
                    } else {
                        return true;
                    };
                }()));
    }(std::index_sequence_for<Ts...>{});
    if (!check_types) {
        return std::unexpected(std::move(details::scan_error{"Incorrect used types"}));
    }

    // Парсинг исходных вью
    auto parsed_sources = details::parse_sources<Ts...>(input, format);
    if (!parsed_sources.has_value()) {
        return std::unexpected(std::move(parsed_sources.error()));
    }
    auto [parsed_format, parsed_input] = parsed_sources.value();
    if (parsed_format.size() != sizeof...(Ts)) {
        return std::unexpected(std::move(details::scan_error{"Types are not correspond placeholders"}));
    }

    // Формирование tuple с результатами парсинга значений
    //  remove reference используется чтобы избежать проблем при выводе типов в случае ссылочных типов
    auto values = [&]<std::size_t... Is>(std::index_sequence<Is...>) {
        return std::make_tuple(
            details::parse_value_with_format<std::remove_reference_t<Ts>>(parsed_input[Is], parsed_format[Is])...);
    }(std::index_sequence_for<Ts...>{});

    // Проверка на наличие в tuple результатов ошибок
    //  Заводим переменную, чтобы в нее отправить ошибки спецификаторов
    std::string parsed_format_error{};
    auto get_errors = [&parsed_format_error](auto &arg) {
        if (!arg.has_value()) {
            parsed_format_error += arg.error().message + '\n';
        }
    };
    std::apply([&](auto &...arg) { (get_errors(arg), ...); }, values);
    if (parsed_format_error.size() > 0) {
        return std::unexpected(std::move(details::scan_error{parsed_format_error}));
    }

    // Если проверка показало что в tuple результатов нет ошибок формируем результат функции
    auto result = [&]<std::size_t... Is>(std::index_sequence<Is...>) -> details::scan_result<Ts...> {
        return {std::make_tuple((std::move(std::get<Is>(values)).value())...)};
    }(std::index_sequence_for<Ts...>{});

    return result;
}

}  // namespace stdx
