#pragma once

#include <cstdint>
#include <string>
#include <type_traits>

namespace stdx::details {

// Разработанные концепты
template <typename T, typename... U>
concept same_types = (std::same_as<std::remove_cv_t<T>, U> || ...);

template <typename T>
concept d = same_types<T, int8_t, int16_t, int32_t>;

template <typename T>
concept u = same_types<T, uint8_t, uint16_t, uint32_t, uint64_t>;

template <typename T>
concept f = std::same_as<std::remove_cv_t<T>, float> || std::same_as<std::remove_cv_t<T>, double>;

template <typename T>
concept s = std::same_as<std::remove_cv_t<T>, std::string> || std::same_as<std::remove_cv_t<T>, std::string_view>;

template <typename T>
concept number = d<T> || u<T> || f<T>;

template <typename T>
concept correct_type = number<T> || s<T>;

// Класс для хранения ошибки неуспешного сканирования

struct scan_error {
    std::string message;
};

// Шаблонный класс для хранения результатов успешного сканирования

template <typename... Ts>
struct scan_result {
    std::tuple<typename std::remove_reference<Ts>::type...> data;
    const std::tuple<typename std::remove_reference<Ts>::type...> &values() const { return data; }
};

}  // namespace stdx::details
