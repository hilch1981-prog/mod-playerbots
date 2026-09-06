#pragma once

#include <string>

namespace chipa
{
namespace cxx14
{

template <typename Container, typename Key>
bool Contains(Container const& container, Key const& key)
{
    return container.find(key) != container.end();
}

inline bool StartsWith(std::string const& value, std::string const& prefix)
{
    return value.size() >= prefix.size() && value.compare(0, prefix.size(), prefix) == 0;
}

} // namespace cxx14
} // namespace chipa
