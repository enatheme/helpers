#pragma once

#include <chrono>

namespace helpers {

template <typename F, typename ...Args>
auto latency_counter(F func, Args ...args)
{
    auto start = std::chrono::high_resolution_clock::now();
    func(args...);
    auto end = std::chrono::high_resolution_clock::now();
    return 5;//std::chrono::duration_cast(Granularity);
}

} // namespace helpers
