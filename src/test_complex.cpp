#include "complex.h"

#include <cmath>
#include <gtest/gtest.h>
#include <limits>
#include <numbers>
#include <sstream>
#include <type_traits>

TEST(ComplexTest, traits)
{
    EXPECT_EQ(sizeof(Complex), sizeof(double) * 2);
    EXPECT_TRUE(std::is_trivially_copyable_v<Complex>);
}

TEST(ComplexTest, construct)
{
    const Complex zero;
    EXPECT_EQ(zero.real(), 0);
    EXPECT_EQ(zero.imag(), 0);

    const Complex re = 1;
    EXPECT_EQ(re.real(), 1);
    EXPECT_EQ(re.imag(), 0);

    const Complex im(0, -1);
    EXPECT_EQ(im.real(), 0);
    EXPECT_EQ(im.imag(), -1);

    const Complex both(42, -4.2);
    EXPECT_EQ(both.real(), 42);
    EXPECT_EQ(both.imag(), -4.2);
}

TEST(ComplexTest, abs)
{
    EXPECT_EQ(Complex().abs(), 0);

    EXPECT_EQ(Complex(1, 0).abs(), 1);
    EXPECT_EQ(Complex(0, 1).abs(), 1);
    EXPECT_EQ(Complex(-1, 0).abs(), 1);
    EXPECT_EQ(Complex(0, -1).abs(), 1);

    EXPECT_DOUBLE_EQ(Complex(std::numbers::sqrt2, std::numbers::sqrt2).abs(), 2);
    EXPECT_DOUBLE_EQ(Complex(1, std::numbers::sqrt3).abs(), 2);

    EXPECT_DOUBLE_EQ(Complex(123, -321).abs(), 343.75863625514923199617);
    EXPECT_DOUBLE_EQ(Complex(1234.56789, 9876.54321).abs(), 9953.40462625810044487251);
}

TEST(ComplexTest, abs_overflow)
{
    double big = std::numeric_limits<double>::max() / 2;
    EXPECT_DOUBLE_EQ(Complex(big, big).abs(), 1.27116100615364613677e308);

    if (std::numeric_limits<double>::is_iec559) {
        double bigger = std::numeric_limits<double>::max() * .8;
        double infty = std::numeric_limits<double>::infinity();
        EXPECT_EQ(Complex(bigger, bigger).abs(), infty);
    }
}

TEST(ComplexTest, equals)
{
    const Complex x(1, 2);
    const Complex y(1, 2);
    const Complex z = x;
    const Complex other(1, -2);

    EXPECT_EQ(x, x);

    EXPECT_EQ(x, y);
    EXPECT_EQ(y, x);

    EXPECT_EQ(x, z);
    EXPECT_EQ(y, z);

    EXPECT_NE(x, other);
    EXPECT_NE(other, x);
}

TEST(ComplexTest, add)
{
    Complex x(1, 2);
    const Complex y(42, -42);

    EXPECT_EQ(x + 0, x);
    EXPECT_EQ(x + 1, Complex(2, 2));

    EXPECT_EQ(x + y, Complex(43, -40));

    x += y;
    EXPECT_EQ(x.real(), 43);
    EXPECT_EQ(x.imag(), -40);
    EXPECT_EQ(y.real(), 42);
    EXPECT_EQ(y.imag(), -42);
}

TEST(ComplexTest, subtract)
{
    Complex x(1, 2);
    const Complex y(42, -42);

    EXPECT_EQ(x - 0, x);
    EXPECT_EQ(x - 1, Complex(0, 2));

    EXPECT_EQ(x - y, Complex(-41, 44));

    x -= y;
    EXPECT_EQ(x.real(), -41);
    EXPECT_EQ(x.imag(), 44);
    EXPECT_EQ(y.real(), 42);
    EXPECT_EQ(y.imag(), -42);
}

TEST(ComplexTest, multiply)
{
    EXPECT_EQ(Complex(0) * 0, 0);
    EXPECT_EQ(Complex(0) * 1, 0);
    EXPECT_EQ(Complex(1) * 0, 0);
    EXPECT_EQ(Complex(1) * 1, 1);

    Complex x(-2, 3);

    EXPECT_EQ(x * 0, 0);
    EXPECT_EQ(x * 1, x);
    EXPECT_EQ(0 * x, 0);
    EXPECT_EQ(1 * x, x);

    const Complex y(10, 20);

    EXPECT_EQ(x * y, Complex(-80, -10));
    EXPECT_EQ(y * x, Complex(-80, -10));

    const Complex z(-42, -40);

    EXPECT_EQ((x * y) * z * .1, Complex(296, 362));
    EXPECT_EQ(x * (y * z) * .1, Complex(296, 362));

    x *= -1;
    EXPECT_EQ(x, Complex(2, -3));
}

TEST(ComplexTest, divide)
{
    EXPECT_EQ(Complex(0) / 1, 0);
    EXPECT_EQ(Complex(0) / std::numeric_limits<double>::max(), 0);
    EXPECT_EQ(Complex(1) / 1, 1);

    Complex x(-2, 3);

    EXPECT_EQ(x / 1, x);
    EXPECT_EQ(0 / x, 0);
    EXPECT_EQ(1 / x, Complex(-2 / 13., -3 / 13.));

    const Complex y(42, 123);

    auto expectDoubleEq = [](const Complex & first, const Complex & second) {
        EXPECT_DOUBLE_EQ(first.real(), second.real());
        EXPECT_DOUBLE_EQ(first.imag(), second.imag());
    };

    expectDoubleEq(x / y, Complex(95 / 5631., 124 / 5631.));
    expectDoubleEq(y / x, Complex(285 / 13., -372 / 13.));
    expectDoubleEq(x / y, 1 / (y / x));
    expectDoubleEq(y / x, 1 / (x / y));

    x /= -1;
    EXPECT_EQ(x, Complex(2, -3));
}

TEST(ComplexTest, divide_by_zero)
{
    if (!std::numeric_limits<double>::is_iec559) {
        GTEST_SKIP();
    }

    double infty = std::numeric_limits<double>::infinity();

    const Complex c1 = Complex() / 0;
    EXPECT_TRUE(std::isnan(c1.real()));
    EXPECT_TRUE(std::isnan(c1.imag()));

    const Complex c2 = Complex(5) / 0;
    EXPECT_EQ(c2.real(), infty);
    EXPECT_TRUE(std::isnan(c2.imag()));

    const Complex c3 = Complex(-5) / 0;
    EXPECT_EQ(c3.real(), -infty);
    EXPECT_TRUE(std::isnan(c3.imag()));

    const Complex c4 = Complex(0, 5) / 0;
    EXPECT_TRUE(std::isnan(c4.real()));
    EXPECT_EQ(c4.imag(), infty);

    const Complex c5 = Complex(0, -5) / 0;
    EXPECT_TRUE(std::isnan(c5.real()));
    EXPECT_EQ(c5.imag(), -infty);

    const Complex c6 = Complex(5, -5) / 0;
    EXPECT_EQ(c6.real(), infty);
    EXPECT_EQ(c6.imag(), -infty);
}

TEST(ComplexTest, negate)
{
    const Complex x(1, -2);

    EXPECT_EQ(-x, 0 - x);
    EXPECT_EQ(-(-x), x);
}

TEST(ComplexTest, conjugate)
{
    const Complex x(1, 2), y(-3, -4);

    EXPECT_EQ(~x, Complex(1, -2));
    EXPECT_EQ(~y, Complex(-3, 4));

    EXPECT_EQ(~(~x), x);
    EXPECT_EQ(~(~y), y);
}

TEST(ComplexTest, string)
{
    static constexpr auto str = [](const Complex & n) {
        return n.str();
    };

    EXPECT_EQ(str(Complex()), "(0,0)");
    EXPECT_EQ(str(Complex(42)), "(42,0)");
    EXPECT_EQ(str(Complex(42, -43)), "(42,-43)");
    EXPECT_EQ(str(Complex(-42, 43)), "(-42,43)");
}

TEST(ComplexTest, print)
{
    static constexpr auto str = [](const Complex & n) {
        std::ostringstream ss;
        ss << n;
        return ss.str();
    };

    EXPECT_EQ(str(Complex()), "(0,0)");
    EXPECT_EQ(str(Complex(42)), "(42,0)");
    EXPECT_EQ(str(Complex(42, -43)), "(42,-43)");
    EXPECT_EQ(str(Complex(-42, 43)), "(-42,43)");
}
