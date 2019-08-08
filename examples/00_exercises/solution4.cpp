///////////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2019 Mikael Simberg
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
///////////////////////////////////////////////////////////////////////////////

#include <hpx/hpx_main.hpp>
#include <hpx/include/iostreams.hpp>
#include <hpx/include/parallel_executors.hpp>
#include <hpx/include/parallel_generate.hpp>
#include <hpx/include/parallel_reduce.hpp>
#include <hpx/include/threads.hpp>
#include <hpx/include/util.hpp>

#include <algorithm>
#include <cstddef>
#include <functional>
#include <random>

int main()
{
    std::vector<double> v(100000000);

    std::mt19937 gen(0);
    std::uniform_real_distribution<double> dis(0.0, 1.0);

    hpx::cout << "generating... " << hpx::flush;
    // We fill the vector sequentially.
    hpx::parallel::generate(hpx::parallel::execution::seq,
        std::begin(v), std::end(v), [&dis, &gen]() { return dis(gen); });
    hpx::cout << "done." << hpx::endl;

    // We create an executor which schedules work on at most 4 threads.
    auto reduce_threads = std::min(
        hpx::this_thread::get_pool()->get_os_thread_count(), std::size_t(4));
    auto exec = hpx::parallel::execution::par.on(
        hpx::parallel::execution::local_queue_executor(reduce_threads));

    hpx::cout << "reducing... " << hpx::flush;
    hpx::util::high_resolution_timer timer;
    // Finally, we do the reduction in parallel.
    double result = hpx::parallel::reduce(exec,
        std::begin(v), std::end(v), 0.0, std::plus<>());
    const double reduce_duration = timer.elapsed();
    hpx::cout << "done." << hpx::endl;

    hpx::cout << "result is " << result << hpx::endl;
    hpx::cout << "reduction took " << reduce_duration << " s" << hpx::endl;

    return 0;
}
