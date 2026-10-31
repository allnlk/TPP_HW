#include "RootFinding.h"
#include "RootFindingExceptions.h"

#include <cmath>
#include <stdexcept>
#include <string>

namespace
{
    const double kTiny = 1e-14;

    void requireFunction(const Function& f, const char* name)
    {
        if (!f)
            throw std::invalid_argument(std::string("Function is empty: ") + name);
    }

    void requireParams(double eps, int maxIter)
    {
        if (!std::isfinite(eps) || eps <= 0.0)
            throw std::invalid_argument("eps must be finite and positive");
        if (maxIter <= 0)
            throw std::invalid_argument("maxIter must be positive");
    }

    void requireFinite(double value, const char* what)
    {
        if (!std::isfinite(value))
            throw std::domain_error(std::string("Not finite value: ") + what);
    }
}

double bisection(const Function& f, double a, double b,
                 double eps, int maxIter)
{
    requireFunction(f, "f");
    requireParams(eps, maxIter);
    if (!std::isfinite(a) || !std::isfinite(b) || a >= b)
        throw std::invalid_argument("Invalid interval: need finite a < b");

    double fa = f(a);
    double fb = f(b);
    requireFinite(fa, "f(a)");
    requireFinite(fb, "f(b)");

    if (fa == 0.0) return a;
    if (fb == 0.0) return b;

    if ((fa < 0.0) == (fb < 0.0))
        throw NoSignChangeException(a, b, fa, fb);

    for (int i = 0; i < maxIter; ++i)
    {
        double m = a + (b - a) / 2.0;
        double fm = f(m);
        requireFinite(fm, "f(m)");

        if (fm == 0.0 || (b - a) / 2.0 < eps)
            return m;

        if ((fa < 0.0) == (fm < 0.0)) { a = m; fa = fm; }
        else                          { b = m; }
    }
    throw ConvergenceException(maxIter, a + (b - a) / 2.0);
}

double newton(const Function& f, const Function& df, double x0,
              double eps, int maxIter)
{
    requireFunction(f, "f");
    requireFunction(df, "df");
    requireParams(eps, maxIter);
    if (!std::isfinite(x0))
        throw std::invalid_argument("x0 must be finite");

    double x = x0;
    for (int i = 0; i < maxIter; ++i)
    {
        double fx = f(x);
        double dfx = df(x);
        requireFinite(fx, "f(x)");
        requireFinite(dfx, "df(x)");

        if (std::fabs(dfx) < kTiny)
            throw ZeroDerivativeException("newton", x, dfx);

        double xNew = x - fx / dfx;
        requireFinite(xNew, "next approximation");

        if (std::fabs(xNew - x) < eps)
            return xNew;
        x = xNew;
    }
    throw ConvergenceException(maxIter, x);
}

double secant(const Function& f, double x0, double x1,
              double eps, int maxIter)
{
    requireFunction(f, "f");
    requireParams(eps, maxIter);
    if (!std::isfinite(x0) || !std::isfinite(x1) || x0 == x1)
        throw std::invalid_argument("x0 and x1 must be finite and different");

    double f0 = f(x0);
    double f1 = f(x1);
    requireFinite(f0, "f(x0)");
    requireFinite(f1, "f(x1)");

    for (int i = 0; i < maxIter; ++i)
    {
        double denom = f1 - f0;
        if (std::fabs(denom) < kTiny)
            throw ZeroDerivativeException("secant", x1, denom);

        double x2 = x1 - f1 * (x1 - x0) / denom;
        requireFinite(x2, "next approximation");

        if (std::fabs(x2 - x1) < eps)
            return x2;

        x0 = x1; f0 = f1;
        x1 = x2; f1 = f(x1);
        requireFinite(f1, "f(x1)");
    }
    throw ConvergenceException(maxIter, x1);
}

double residual(const Function& f, double x)
{
    requireFunction(f, "f");
    if (!std::isfinite(x))
        throw std::invalid_argument("x must be finite");

    double fx = f(x);
    requireFinite(fx, "f(x)");
    return std::fabs(fx);
}
