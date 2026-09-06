#include "Cxx14Compat.h"

#include <cassert>
#include <set>
#include <string>
#include <unordered_set>

int main()
{
    std::set<int> ordered{1, 3, 5};
    assert(chipa::cxx14::Contains(ordered, 3));
    assert(!chipa::cxx14::Contains(ordered, 4));

    std::unordered_set<std::string> hashed{"alpha", "beta"};
    assert(chipa::cxx14::Contains(hashed, std::string("beta")));
    assert(!chipa::cxx14::Contains(hashed, std::string("gamma")));

    assert(chipa::cxx14::StartsWith("init=123", "init="));
    assert(chipa::cxx14::StartsWith("prefix", ""));
    assert(chipa::cxx14::StartsWith("same", "same"));
    assert(!chipa::cxx14::StartsWith("short", "shorter"));
    assert(!chipa::cxx14::StartsWith("toxic-link", "safe"));

    return 0;
}
