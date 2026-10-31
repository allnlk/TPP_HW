#pragma once

#include <functional>

using Function = std::function<double(double)>;

// Усі функції кидають винятки (див. RootFindingExceptions.h):
//  std::invalid_argument  - порожня функція, eps <= 0, maxIter <= 0, a >= b, NaN/Inf параметри
//  std::domain_error      - функція повернула NaN/Inf
//  NoSignChangeException, ZeroDerivativeException, ConvergenceException
double bisection(const Function& f, double a, double b, double eps, int maxIter);
double newton(const Function& f, const Function& df, double x0, double eps, int maxIter);
double secant(const Function& f, double x0, double x1, double eps, int maxIter);
double residual(const Function& f, double x);
