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

// ============ Тести класу RootFindingTask ============
#include "RootFindingTask.h"
#include <limits>

namespace
{
    double badF(double x)  { return x * x + 1.0; }   // коренів немає
    double badDf(double x) { return 2.0 * x; }
}

TEST(RootFindingTaskTest, CreatesValidTask)
{
    RootFindingTask task(0.0, 2.0, 1e-9, 100);
    EXPECT_DOUBLE_EQ(task.a(), 0.0);
    EXPECT_DOUBLE_EQ(task.b(), 2.0);
    EXPECT_DOUBLE_EQ(task.eps(), 1e-9);
    EXPECT_EQ(task.maxIter(), 100);
    EXPECT_FALSE(task.hasResults());
}

TEST(RootFindingTaskTest, ConstructorRejectsInvalidInterval)
{
    EXPECT_THROW(RootFindingTask(2.0, 0.0, 1e-9, 100), std::invalid_argument);
    EXPECT_THROW(RootFindingTask(1.0, 1.0, 1e-9, 100), std::invalid_argument);
}

TEST(RootFindingTaskTest, ConstructorRejectsNonFiniteInterval)
{
    const double nan = std::numeric_limits<double>::quiet_NaN();
    const double inf = std::numeric_limits<double>::infinity();
    EXPECT_THROW(RootFindingTask(nan, 1.0, 1e-9, 100), std::invalid_argument);
    EXPECT_THROW(RootFindingTask(0.0, inf, 1e-9, 100), std::invalid_argument);
}

TEST(RootFindingTaskTest, ConstructorRejectsInvalidPrecision)
{
    EXPECT_THROW(RootFindingTask(0.0, 2.0, 0.0, 100), std::invalid_argument);
    EXPECT_THROW(RootFindingTask(0.0, 2.0, -1e-9, 100), std::invalid_argument);
    EXPECT_THROW(RootFindingTask(0.0, 2.0, 1e-9, 0), std::invalid_argument);
}

TEST(RootFindingTaskTest, ConstructorThrowsOnLengthOverflow)
{
    const double big = std::numeric_limits<double>::max();
    EXPECT_THROW(RootFindingTask(-big, big, 1e-9, 100), std::overflow_error);
}

TEST(RootFindingTaskTest, ComputeRootsStoresResults)
{
    RootFindingTask task(0.0, 2.0, 1e-9, 100);
    task.computeRoots(f, df, 1.0);

    ASSERT_TRUE(task.hasResults());
    EXPECT_NEAR(task.results().bisectionRoot, std::sqrt(2.0), 1e-8);
    EXPECT_NEAR(task.results().newtonRoot, std::sqrt(2.0), 1e-8);
    EXPECT_NEAR(task.results().bisectionResidual, 0.0, 1e-7);
    EXPECT_NEAR(task.results().newtonResidual, 0.0, 1e-7);
}

TEST(RootFindingTaskTest, ResultsThrowsWhenNotComputed)
{
    RootFindingTask task(0.0, 2.0, 1e-9, 100);
    EXPECT_THROW(task.results(), std::logic_error);
}

// Сильна гарантія: перший метод успішний, другий (Ньютон) кидає виняток
TEST(RootFindingTaskTest, ComputeRootsPreservesResultsWhenSecondMethodFails)
{
    RootFindingTask task(0.0, 2.0, 1e-9, 100);
    task.computeRoots(f, df, 1.0);

    const RootResults before = task.results();

    // x0 = 0 -> df(0) = 0 -> ZeroDerivativeException після успішної бісекції
    EXPECT_THROW(task.computeRoots(f, df, 0.0), ZeroDerivativeException);

    ASSERT_TRUE(task.hasResults());
    EXPECT_DOUBLE_EQ(task.results().bisectionRoot, before.bisectionRoot);
    EXPECT_DOUBLE_EQ(task.results().newtonRoot, before.newtonRoot);
    EXPECT_DOUBLE_EQ(task.results().bisectionResidual, before.bisectionResidual);
    EXPECT_DOUBLE_EQ(task.results().newtonResidual, before.newtonResidual);
}

// Сильна гарантія: перший метод (бісекція) кидає виняток
TEST(RootFindingTaskTest, ComputeRootsPreservesResultsWhenFirstMethodFails)
{
    RootFindingTask task(0.0, 2.0, 1e-9, 100);
    task.computeRoots(f, df, 1.0);

    const RootResults before = task.results();

    EXPECT_THROW(task.computeRoots(badF, badDf, 1.0), NoSignChangeException);

    ASSERT_TRUE(task.hasResults());
    EXPECT_DOUBLE_EQ(task.results().bisectionRoot, before.bisectionRoot);
    EXPECT_DOUBLE_EQ(task.results().newtonRoot, before.newtonRoot);
}

// Якщо результатів не було, після невдачі їх теж немає
TEST(RootFindingTaskTest, FailedComputeLeavesNoResults)
{
    RootFindingTask task(0.0, 2.0, 1e-9, 100);
    EXPECT_THROW(task.computeRoots(badF, badDf, 1.0), NoSignChangeException);
    EXPECT_FALSE(task.hasResults());
}

TEST(RootFindingTaskTest, SetIntervalPreservesStateOnException)
{
    RootFindingTask task(0.0, 2.0, 1e-9, 100);
    task.computeRoots(f, df, 1.0);

    EXPECT_THROW(task.setInterval(5.0, 1.0), std::invalid_argument);

    EXPECT_DOUBLE_EQ(task.a(), 0.0);
    EXPECT_DOUBLE_EQ(task.b(), 2.0);
    EXPECT_TRUE(task.hasResults());
}

TEST(RootFindingTaskTest, SetIntervalResetsResultsOnSuccess)
{
    RootFindingTask task(0.0, 2.0, 1e-9, 100);
    task.computeRoots(f, df, 1.0);

    task.setInterval(-3.0, 3.0);

    EXPECT_DOUBLE_EQ(task.a(), -3.0);
    EXPECT_DOUBLE_EQ(task.b(), 3.0);
    EXPECT_FALSE(task.hasResults());
}

TEST(RootFindingTaskTest, SetPrecisionPreservesStateOnException)
{
    RootFindingTask task(0.0, 2.0, 1e-9, 100);

    EXPECT_THROW(task.setPrecision(-1.0, 50), std::invalid_argument);
    EXPECT_THROW(task.setPrecision(1e-6, 0), std::invalid_argument);

    EXPECT_DOUBLE_EQ(task.eps(), 1e-9);
    EXPECT_EQ(task.maxIter(), 100);
}

TEST(RootFindingTaskTest, SetPrecisionUpdatesValues)
{
    RootFindingTask task(0.0, 2.0, 1e-9, 100);
    task.setPrecision(1e-6, 50);
    EXPECT_DOUBLE_EQ(task.eps(), 1e-6);
    EXPECT_EQ(task.maxIter(), 50);
}

TEST(RootFindingTaskTest, ClearResultsRemovesResults)
{
    RootFindingTask task(0.0, 2.0, 1e-9, 100);
    task.computeRoots(f, df, 1.0);
    task.clearResults();
    EXPECT_FALSE(task.hasResults());
}

// Граничні значення: корінь прямо на межі інтервалу
TEST(RootFindingTaskTest, RootOnIntervalBoundary)
{
    RootFindingTask task(1.0, 3.0, 1e-9, 100);
    auto g  = [](double x) { return x - 1.0; };
    auto dg = [](double)   { return 1.0; };

    task.computeRoots(g, dg, 2.0);

    EXPECT_DOUBLE_EQ(task.results().bisectionRoot, 1.0);
    EXPECT_NEAR(task.results().newtonRoot, 1.0, 1e-8);
}
