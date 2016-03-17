#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "ThreadPool.hh"
#include <vector>
#include <numeric>

class ScalingFunctor
{
public:
    ScalingFunctor(const size_t N)
    {
        a.resize(N);
        b.resize(N);
        std::iota(b.begin(), b.end(), 0);
    }

    void operator()(const size_t i)
    {
        a.at(i) = 4*b.at(i);
    }

    std::vector<double> a;
    std::vector<double> b;
};

TEST_CASE("Parallel for with MapRange")
{
    size_t N = 1000;
    ScalingFunctor sf(N);

    SECTION("Serial version works")
    {
        for (size_t i = 0; i < N; i++) {
            sf(i);
            REQUIRE(sf.a.at(i) == Approx(4*sf.b.at(i)));
        }
    }

    SECTION("Parallel version works")
    {
        ThreadPool pool(4);
        pool.MapRange(sf, 0, N);

        for (size_t i = 0; i < N; i++) {
            REQUIRE(sf.a.at(i) == Approx(4*sf.b.at(i)));
        }
    }
}
