#pragma once

#include <chrono>
#include <concepts>
#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>

template <typename T>
concept TimeUnitTypeRequired =
    requires(T t) { requires std::same_as<T, std::chrono::seconds> || std::same_as<T, std::chrono::milliseconds>; };