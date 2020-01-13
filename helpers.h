#pragma once

#include <algorithm>
#include <chrono>
#include <cmath>
#include <future>
#include <numeric>
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

template <typename F, typename ...Args>
double latency_counter_average_parallel(F func, int iteration, Args ...args)
{
    std::vector<std::future<long int>> latencies(iteration);
    std::for_each(latencies.begin(), latencies.end(), [&] (auto & l) {
        l = std::async([func, args...] {
            return latency_counter(func, args...);
        });
    });
    double count = 0;
    std::for_each(latencies.begin(), latencies.end(), [&count] (auto & l) {
        l.wait();
        count += l.get();
    });
    return count / iteration;
    // @TODO figure out why accumulate fails here
    return (std::accumulate(latencies.begin(), latencies.end(), 0, [] (auto accumulator, auto & rhs) {
        rhs.wait();
        return accumulator + rhs.get();
    }) / iteration);
}

// returns the fastest function, does not validate the result
template <typename F, typename ...Args>
auto fastest_function_parallel(int iteration, std::vector<F> funcs, Args ...args)
{
    float epsilon = 0.01f;
    std::vector<std::tuple<std::future<double>, double /* latency retrieved */, F>> functions_latency;
    std::for_each(funcs.begin(), funcs.end(), [&](auto & f) {
        functions_latency.push_back({std::async([&f, &iteration, &args...] {
            return latency_counter_average_parallel(f, iteration, args...);
        }), 0, f});
    });

    for (auto & latency : functions_latency) {
        std::get<0>(latency).wait();
        auto val = std::get<0>(latency).get();
        std::get<1>(latency) = val;
    }
    auto ret = std::get<2>(functions_latency[0]);
    auto lll = std::get<1>(functions_latency[0]);
    std::for_each(functions_latency.begin(), functions_latency.end(), [&ret, &lll](auto & latency) {
        if (std::get<1>(latency) < lll) {
            ret = std::get<2>(latency);
            lll = std::get<1>(latency);
        }
    });
    return ret;
    // @TODO figure out why min_element fails here
    return std::get<2>(*std::min_element(functions_latency.begin(), functions_latency.end(), [&epsilon](auto & f1, auto & f2) {
        return std::abs(std::get<1>(f1) - std::get<1>(f2)) < epsilon;
    }));
}

} // namespace helpers
