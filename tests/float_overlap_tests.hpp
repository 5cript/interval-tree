#pragma once

#include "test_utility.hpp"
#include "../draw.hpp"

class OverlapFindTests
    : public ::testing::Test
{
public:
    using types = IntervalTypes <double>;
protected:
    IntervalTypes <double>::tree_type tree;
};

TEST_F(OverlapFindTests, FloatOverlapTest)
{
    tree.insert(lib_interval_tree::make_safe_interval<double>(-1.483529864195180e+00, -1.296053859335657e+00));
    tree.insert(lib_interval_tree::make_safe_interval<double>(-1.308996938995747e+00, -1.127801743538376e+00));
    tree.insert(lib_interval_tree::make_safe_interval<double>(-1.134464013796314e+00, -9.562870818388700e-01));
    tree.insert(lib_interval_tree::make_safe_interval<double>(-9.599310885968813e-01, -7.834918877708545e-01));
    tree.insert(lib_interval_tree::make_safe_interval<double>(-7.853981633974484e-01, -6.090750919515169e-01));
    tree.insert(lib_interval_tree::make_safe_interval<double>(-6.108652381980154e-01, -4.348738075675338e-01));
    tree.insert(lib_interval_tree::make_safe_interval<double>(-4.363323129985824e-01, -2.608478200480425e-01));
    tree.insert(lib_interval_tree::make_safe_interval<double>(-2.617993877991495e-01, -8.693606119038631e-02));
    tree.insert(lib_interval_tree::make_safe_interval<double>(-8.726646259971654e-02, 8.726646259971654e-02));
    tree.insert(lib_interval_tree::make_safe_interval<double>(8.693606119038631e-02, 2.617993877991493e-01));
    tree.insert(lib_interval_tree::make_safe_interval<double>(2.608478200480422e-01, 4.363323129985823e-01));
    tree.insert(lib_interval_tree::make_safe_interval<double>(4.348738075675337e-01, 6.108652381980154e-01));
    tree.insert(lib_interval_tree::make_safe_interval<double>(6.090750919515169e-01, 7.853981633974484e-01));
    tree.insert(lib_interval_tree::make_safe_interval<double>(7.834918877708545e-01, 9.599310885968813e-01));
    tree.insert(lib_interval_tree::make_safe_interval<double>(9.562870818388700e-01, 1.134464013796314e+00)); //
    tree.insert(lib_interval_tree::make_safe_interval<double>(1.127801743538376e+00, 1.308996938995747e+00)); //
    tree.insert(lib_interval_tree::make_safe_interval<double>(1.296053859335657e+00, 1.483529864195180e+00)); //

    double lat0 = 1.040893537045970;
    double lat1 = 1.570796326794897;

    std::vector <lib_interval_tree::interval<double>> vecOverlapsA;
    lib_interval_tree::interval <double> intSource({lat0, lat1});
    for (auto itTargetInterval : tree) 
    {
        if (itTargetInterval.overlaps(intSource)) 
        {
            vecOverlapsA.push_back(itTargetInterval);
        }
    }

    std::vector <lib_interval_tree::interval<double>> vecOverlapsB;
    tree.overlap_find_all
    (
        {lat0, lat1},
        [&vecOverlapsB](lib_interval_tree::interval_tree_t<double>::iterator ittarget)
        {
            vecOverlapsB.push_back(*ittarget); 
            return true;
        },
        false
    );

    for (auto const& i : vecOverlapsA)
    {
        std::cout << i.low() << ", " << i.high() << "\n";
    }
    std::cout << "\n";
    for (auto const& i : vecOverlapsB)
    {
        std::cout << i.low() << ", " << i.high() << "\n";
    }

    lib_interval_tree::drawTree("here.png", tree);
    
    ASSERT_EQ(vecOverlapsA, vecOverlapsB);
}
