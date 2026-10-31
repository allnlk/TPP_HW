#include <gtest/gtest.h>
#include "RootFinding.h"
#include "RootFindingExceptions.h"

#include <cmath>
#include <stdexcept>

static double f(double x)  { return x * x - 2.0; }
static double df(double x) { return 2.0 * x; }

TEST(RootFindingTest, BisectionFindsSqrt2)
{
    EXPECT_NEAR(bisection(f, 0.0, 2.0, 1e-9, 100), std::sqrt(2.0), 1e-8);
}

TEST(RootFindingTest, NewtonFindsSqrt2)
{
    EXPECT_NEAR(newton(f, df, 1.0, 1e-12, 50), std::sqrt(2.0), 1e-10);
}

TEST(RootFindingTest, SecantFindsSqrt2)
{
    EXPECT_NEAR(secant(f, 1.0, 2.0, 1e-12, 50), std::sqrt(2.0), 1e-10);
}

TEST(RootFindingTest, BisectionThrowsWhenNoSignChange)
{
    EXPECT_THROW(bisection(f, 2.0, 3.0, 1e-9, 100), NoSignChangeException);
}

TEST(RootFindingTest, NewtonThrowsOnZeroDerivative)
{
    EXPECT_THROW(newton(f, df, 0.0, 1e-9, 50), ZeroDerivativeException);
}

TEST(RootFindingTest, BisectionThrowsOnInvalidInterval)
{
    EXPECT_THROW(bisection(f, 2.0, 0.0, 1e-9, 100), std::invalid_argument);
}

TEST(RootFindingTest, BisectionThrowsWhenNoConvergence)
{
    EXPECT_THROW(bisection(f, 0.0, 2.0, 1e-15, 3), ConvergenceException);
}
