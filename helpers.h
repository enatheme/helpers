#pragma once

#include <algorithm>
#include <chrono>
#include <future>
#include <vector>

namespace helpers {

template <typename F, typename ...Args>
auto latency_counter(F func, Args ...args)
{
    auto start = std::chrono::high_resolution_clock::now();
    func(args...);
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
}

template <typename F, typename ...Args>
auto latency_counter_average(F func, int iteration, Args ...args)
{
    double count = 0;
    for(int i = 0 ; i < iteration ; ++i)
    {
        count += latency_counter(func, args...);
    }
    return count / iteration;
}

// returns the fastest function, does not validate the result
template <typename F, typename ...Args>
auto fastest_function(int iteration, std::vector<F> funcs, Args ...args)
{
    std::vector<std::tuple<double /* latency */, F>> functions_latency;
    std::for_each(funcs.begin(), funcs.end(), [&](auto f) {
        double latency = latency_counter_average(f, iteration, args...);
        functions_latency.push_back({latency, f});
    });

    return std::get<1>(*std::min_element(functions_latency.begin(), functions_latency.end(), [](auto & f1, auto & f2) {
        return std::get<0>(f1) < std::get<0>(f2);
    }));
}

} // namespace helpers
