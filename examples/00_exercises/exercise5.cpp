///////////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2019 Mikael Simberg
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
///////////////////////////////////////////////////////////////////////////////

#include <hpx/hpx_main.hpp>
#include <hpx/include/async.hpp>
#include <hpx/include/iostreams.hpp>
#include <hpx/include/lcos.hpp>
#include <hpx/include/parallel_for_loop.hpp>
#include <hpx/include/parallel_generate.hpp>
#include <hpx/include/util.hpp>

// This program compiles and runs correctly. Try to use a parallel HPX algorithm
// instead of the for loop for filtering.
//
// Hint: All headers that you might need are already included.
// Hint: for_loop is in the hpx::parallel namespace.
// Hint: Execution policies are in the hpx::parallel::execution namespace.
// Hint: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/n4725#parallel.alg.forloop.

int main()
{
    const int n = 10000000;

    std::vector<double> v(n);
    std::vector<double> w(n);

    std::mt19937 gen(0);
    std::uniform_real_distribution<double> dis(0.0, 1.0);

    hpx::cout << "generating... " << hpx::flush;
    hpx::parallel::generate(hpx::parallel::execution::seq, std::begin(v),
        std::end(v), [&dis, &gen]() { return dis(gen); });
    hpx::cout << "done." << hpx::endl;

    hpx::cout << "filtering... " << hpx::flush;

    hpx::util::high_resolution_timer timer;
    for (std::size_t i = 1; i < n - 1; ++i)
    {
        w[i] = (v[i - 1] + v[i] + v[i + 1]) / 3.0;
    }

    w[0] = (v[n - 1] + v[0] + v[1]) / 3.0;
    w[n - 1] = (v[n - 2] + v[n - 1] + v[0]) / 3.0;

    const double filter_duration = timer.elapsed();
    hpx::cout << "done." << hpx::endl;

    hpx::cout << "filter took " << filter_duration << " s" << hpx::endl;

    return 0;
}
