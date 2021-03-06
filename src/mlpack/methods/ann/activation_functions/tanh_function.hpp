/**
 * @file tanh_function.hpp
 * @author Marcus Edel
 *
 * Definition and implementation of the Tangens Hyperbolic function.
 */
#ifndef __MLPACK_METHODS_ANN_ACTIVATION_FUNCTIONS_TANH_FUNCTION_HPP
#define __MLPACK_METHODS_ANN_ACTIVATION_FUNCTIONS_TANH_FUNCTION_HPP

#include <mlpack/core.hpp>

namespace mlpack {
namespace ann /** Artificial Neural Network. */ {

/**
 * The tanh function, defined by
 *
 * @f[
 * f(x) &=& \frac{e^x - e^{-x}{e^x + e^{-x}}} \\
 * f'(x) &=& 1 - tanh^2(x) \\
 * f^{-1}(x) &=& atan(x)
 * @f]
 */
class TanhFunction
{
  public:
  /**
   * Computes the tanh function.
   *
   * @param x Input data.
   * @return f(x).
   */
  static double fn(const double x)
  {
    return std::tanh(x);
  }

  /**
   * Computes the tanh function.
   *
   * @param x Input data.
   * @param y The resulting output activation.
   */
  template<typename InputVecType, typename OutputVecType>
  static void fn(const InputVecType& x, OutputVecType& y)
  {
    y = arma::tanh(x);
  }

  /**
   * Computes the first derivative of the tanh function.
   *
   * @param y Input data.
   * @return f'(x)
   */
  static double deriv(const double y)
  {
    return 1 - std::pow(y, 2);
  }

  /**
   * Computes the first derivatives of the tanh function.
   *
   * @param y Input data.
   * @param x The resulting derivatives.
   */
  template<typename InputVecType, typename OutputVecType>
  static void deriv(const InputVecType& y, OutputVecType& x)
  {
    x = 1 - arma::pow(y, 2);
  }

  /**
   * Computes the inverse of the tanh function.
   *
   * @param y Input data.
   * @return f^{-1}(x)
   */
  static double inv(const double y)
  {
    return std::atanh(y);
  }

  /**
   * Computes the inverse of the tanh function.
   *
   * @param y Input data.
   * @param x The resulting inverse of the input data.
   */
  template<typename InputVecType, typename OutputVecType>
  static void inv(const InputVecType& y, OutputVecType& x)
  {
    x = arma::atanh(y);
  }
}; // class TanhFunction

}; // namespace ann
}; // namespace mlpack

#endif
