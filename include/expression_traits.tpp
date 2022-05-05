#pragma once

#include "complex.h"
#include "expression_traits.h"

#include <random>

template <>
struct ExpressionTraits<Complex> : BaseExpressionTraits<Complex>
{
private:
    static bool checkMultiplicative(const Complex & left, const Complex & right)
    {
        double rr = left.real() * right.real();
        double ri = left.real() * right.imag();
        double ir = left.imag() * right.real();
        double ii = left.imag() * right.imag();
        return checkBounds(rr) && checkBounds(ri) && checkBounds(ir) && checkBounds(ii) && checkBounds(rr - ii) && checkBounds(ri + ir);
    }

public:
    static Complex randomNumber(std::mt19937 & rng)
    {
        std::uniform_int_distribution<int> dist(-100, 100);
        return Complex(dist(rng), dist(rng));
    }

    static inline const UnaryOperation UNARY_OPERATIONS[]{
            UnaryOperation(std::negate{}, operator-),
            UnaryOperation(std::bit_not{}, operator~),
    };

    static inline const BinaryOperation BINARY_OPERATIONS[]{
            BinaryOperation(std::plus{}, operator+, [](const Complex & left, const Complex & right) {
                double real = left.real();
                double imag = left.imag();
                return checkBounds(real + right.real()) && checkBounds(imag + right.imag());
            }),
            BinaryOperation(std::minus{}, operator-, [](const Complex & left, const Complex & right) {
                double real = left.real();
                double imag = left.imag();
                return checkBounds(real - right.real()) && checkBounds(imag - right.imag());
            }),
            BinaryOperation(std::multiplies{}, operator*, checkMultiplicative),
            BinaryOperation(std::divides{}, operator/, [](const Complex & left, const Complex & right) {
                double r2 = right.real() * right.real();
                double i2 = right.imag() * right.imag();

                return checkMultiplicative(left, right) && checkBounds(r2) && checkBounds(i2) && r2 + i2 > 0 && checkBounds(r2 + i2);
            }),
    };
};

using Number = Complex;
