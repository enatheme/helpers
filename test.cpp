#include "helpers.h"

#include <cassert>
#include <iostream>
#include <thread>

void f(int a, int b)
{
    std::this_thread::sleep_for(std::chrono::seconds(a + b));
}

void f2(int a, int b)
{
    std::cout << "must be displayed" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

void f3(int a, int b)
{
    std::this_thread::sleep_for(std::chrono::seconds(3));
}

int main()
{
    std::cout << helpers::latency_counter_average<>(&f, 2, 1, 2) << std::endl;

    std::vector<decltype(&f2)> fs = {f, f2, f3};
    auto fu = helpers::fastest_function(1, fs, 2, 3);
    fu(2, 3);
    assert(fu == &f2);
}
