#include "helpers.h"

#include <iostream>
#include <thread>

void f(int a, int b)
{
    std::this_thread::sleep_for(std::chrono::seconds(a + b));
}

int main()
{
    std::cout << helpers::latency_counter<>(&f, 1, 2) << std::endl;
}
