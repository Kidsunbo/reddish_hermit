#include <storage/database.h>
#include <chrono>
#include <optional>
#include <string>

KiedisDB db;

void KiedisDB::set(std::string key, std::string value)
{
    database.insert({key, value});
}

std::optional<std::string> KiedisDB::get(std::string key)
{
    if (expire.contains(key) && expire.at(key) < current<std::chrono::milliseconds>())
    {
        database.erase(key);
        expire.erase(key);
        return std::nullopt;
    }
    return database.contains(key) ? std::make_optional(database.at(key)) : std::nullopt;
}

void KiedisDB::set(std::string key, std::string value, std::int64_t px)
{
    expire.insert({key, current<std::chrono::milliseconds>() + px});
    this->set(key, value);
}
