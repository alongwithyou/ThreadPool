#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "ThreadPool.hh"
#include <vector>
#include <numeric>

TEST_CASE("Parallel for with MapRange")
{
    size_t N = 1000;

    std::vector<double> a (N);
    std::vector<double> b (N);
    std::iota(a.begin(), a.end(), 0);

    SECTION("Parallel version works with one thread")
    {
        ThreadPool pool(1);
        std::function<double(double)> workFxn ([](double v)->double{ return 4*v; });
        pool.ParallelMap(a.begin(), a.end(), b.begin(), workFxn);

        for (size_t i = 0; i < N; i++) {
            REQUIRE(b.at(i) == Approx(4*a.at(i)));
        }
    }

    SECTION("Parallel version works with four threads")
    {
        ThreadPool pool(4);
        std::function<double(double)> workFxn ([](double v)->double{ return 4*v; });
        pool.ParallelMap(a.begin(), a.end(), b.begin(), workFxn);

        for (size_t i = 0; i < N; i++) {
            REQUIRE(b.at(i) == Approx(4*a.at(i)));
        }
    }
}
