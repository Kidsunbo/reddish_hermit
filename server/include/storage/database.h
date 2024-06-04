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

// KiedisDB add a shared lock for writing operation and reading operation.
// Note: actually asio only use single thread because I only run it in the main thread. So there is no need to maintain
// a lock for now.
class KiedisDB
{
    std::unordered_map<std::string, std::string> database;
    std::unordered_map<std::string, std::int64_t> expire;

  private:
    template <TimeUnitTypeRequired TimeUnit> std::int64_t current()
    {
        return std::chrono::duration_cast<TimeUnit>(std::chrono::system_clock::now().time_since_epoch()).count();
    }

  public:
    explicit KiedisDB()
    {
    }

    void set(std::string, std::string);

    void set(std::string, std::string, std::int64_t);

    std::optional<std::string> get(std::string);
};