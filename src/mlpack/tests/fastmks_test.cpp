/**
 * @file fastmks_test.cpp
 * @author Ryan Curtin
 *
 * Ensure that fast max-kernel search is correct.
 */
#include <mlpack/core.hpp>
#include <mlpack/methods/fastmks/fastmks.hpp>
#include <mlpack/core/kernels/linear_kernel.hpp>
#include <mlpack/core/kernels/polynomial_kernel.hpp>

#include <boost/test/unit_test.hpp>
#include "old_boost_test_definitions.hpp"

using namespace mlpack;
using namespace mlpack::tree;
using namespace mlpack::fastmks;
using namespace mlpack::kernel;
using namespace mlpack::metric;

BOOST_AUTO_TEST_SUITE(FastMKSTest);

/**
 * Compare single-tree and naive.
 */
BOOST_AUTO_TEST_CASE(SingleTreeVsNaive)
{
  // First create a random dataset.
  arma::mat data;
  data.randn(5, 1000);
  LinearKernel lk;

  // Now run FastMKS naively.
  FastMKS<LinearKernel> naive(data, lk, false, true);

  arma::Mat<size_t> naiveIndices;
  arma::mat naiveProducts;
  naive.Search(10, naiveIndices, naiveProducts);

  // Now run it in single-tree mode.
  FastMKS<LinearKernel> single(data, lk, true);

  arma::Mat<size_t> singleIndices;
  arma::mat singleProducts;
  single.Search(10, singleIndices, singleProducts);

  // Compare the results.
  for (size_t q = 0; q < singleIndices.n_cols; ++q)
  {
    for (size_t r = 0; r < singleIndices.n_rows; ++r)
    {
      BOOST_REQUIRE_EQUAL(singleIndices(r, q), naiveIndices(r, q));
      BOOST_REQUIRE_CLOSE(singleProducts(r, q), naiveProducts(r, q), 1e-5);
    }
  }
}

/**
 * Compare dual-tree and naive.
 */
BOOST_AUTO_TEST_CASE(DualTreeVsNaive)
{
  // First create a random dataset.
  arma::mat data;
  data.randn(10, 5000);
  LinearKernel lk;

  // Now run FastMKS naively.
  FastMKS<LinearKernel> naive(data, lk, false, true);

  arma::Mat<size_t> naiveIndices;
  arma::mat naiveProducts;
  naive.Search(10, naiveIndices, naiveProducts);

  // Now run it in dual-tree mode.
  FastMKS<LinearKernel> tree(data, lk);

  arma::Mat<size_t> treeIndices;
  arma::mat treeProducts;
  tree.Search(10, treeIndices, treeProducts);

  for (size_t q = 0; q < treeIndices.n_cols; ++q)
  {
    for (size_t r = 0; r < treeIndices.n_rows; ++r)
    {
      BOOST_REQUIRE_EQUAL(treeIndices(r, q), naiveIndices(r, q));
      BOOST_REQUIRE_CLOSE(treeProducts(r, q), naiveProducts(r, q), 1e-5);
    }
  }
}

/**
 * Compare dual-tree and single-tree on a larger dataset.
 */
BOOST_AUTO_TEST_CASE(DualTreeVsSingleTree)
{
  // First create a random dataset.
  arma::mat data;
  data.randu(8, 5000);
  PolynomialKernel pk(5.0, 2.5);

  FastMKS<PolynomialKernel> single(data, pk, true);

  arma::Mat<size_t> singleIndices;
  arma::mat singleProducts;
  single.Search(10, singleIndices, singleProducts);

  // Now run it in dual-tree mode.
  FastMKS<PolynomialKernel> tree(data, pk);

  arma::Mat<size_t> treeIndices;
  arma::mat treeProducts;
  tree.Search(10, treeIndices, treeProducts);

  for (size_t q = 0; q < treeIndices.n_cols; ++q)
  {
    for (size_t r = 0; r < treeIndices.n_rows; ++r)
    {
      BOOST_REQUIRE_EQUAL(treeIndices(r, q), singleIndices(r, q));
      BOOST_REQUIRE_CLOSE(treeProducts(r, q), singleProducts(r, q), 1e-5);
    }
  }
}

/**
 * Test sparse FastMKS (how useful is this, I'm not sure).
 */
BOOST_AUTO_TEST_CASE(SparseFastMKSTest)
{
  // First create a random sparse dataset.
  arma::sp_mat dataset;
  dataset.sprandu(10, 100, 0.3);

  typedef CoverTree<IPMetric<LinearKernel>, FirstPointIsRoot, FastMKSStat,
      arma::sp_mat> SparseCoverTreeType;

  FastMKS<LinearKernel, SparseCoverTreeType> sparsemks(dataset);

  arma::mat denseset(dataset);
  FastMKS<LinearKernel> densemks(denseset);

  // Store the results in these.
  arma::Mat<size_t> sparseIndices, denseIndices;
  arma::mat sparseKernels, denseKernels; 

  // Do the searches.
  sparsemks.Search(3, sparseIndices, sparseKernels);
  densemks.Search(3, denseIndices, denseKernels);

  // Make sure the results are the same.
  for (size_t i = 0; i < sparseIndices.n_cols; ++i)
  {
    for (size_t j = 0; j < sparseIndices.n_rows; ++j)
    {
      if (std::abs(sparseKernels(j, i)) > 1e-15)
        BOOST_REQUIRE_CLOSE(sparseKernels(j, i), denseKernels(j, i), 1e-5);
      else
        BOOST_REQUIRE_SMALL(denseKernels(j, i), 1e-15);
      BOOST_REQUIRE_EQUAL(sparseIndices(j, i), denseIndices(j, i));
    }
  }
}

BOOST_AUTO_TEST_CASE(SparsePolynomialFastMKSTest)
{
  // Do it again with the polynomial kernel, just to be sure.
  arma::sp_mat dataset;
  dataset.sprandu(10, 100, 0.3);
  arma::mat denseset(dataset);

  typedef CoverTree<IPMetric<PolynomialKernel>, FirstPointIsRoot, FastMKSStat,
      arma::sp_mat> SparseCoverTreeType;

  PolynomialKernel pk(3);

  for (size_t i = 0; i < 100; ++i)
    for (size_t j = 0; j < 100; ++j)
      if (std::abs(pk.Evaluate(dataset.col(i), dataset.col(j))) < 1e-10)
        BOOST_REQUIRE_SMALL(pk.Evaluate(denseset.col(i), denseset.col(j)), 1e-10);
      else
        BOOST_REQUIRE_CLOSE(pk.Evaluate(dataset.col(i), dataset.col(j)),
                            pk.Evaluate(denseset.col(i), denseset.col(j)),
                            1e-5);

  FastMKS<PolynomialKernel, SparseCoverTreeType> sparsepoly(dataset);
  FastMKS<PolynomialKernel> densepoly(denseset);

  // Store the results in these.
  arma::Mat<size_t> sparseIndices, denseIndices;
  arma::mat sparseKernels, denseKernels; 

  // Do the searches.
  sparsepoly.Search(3, sparseIndices, sparseKernels);
  densepoly.Search(3, denseIndices, denseKernels);

  // Make sure the results are the same.
  for (size_t i = 0; i < sparseIndices.n_cols; ++i)
  {
    for (size_t j = 0; j < sparseIndices.n_rows; ++j)
    {
      if (std::abs(sparseKernels(j, i)) > 1e-15)
        BOOST_REQUIRE_CLOSE(sparseKernels(j, i), denseKernels(j, i), 1e-5);
      else
        BOOST_REQUIRE_SMALL(denseKernels(j, i), 1e-15);
      BOOST_REQUIRE_EQUAL(sparseIndices(j, i), denseIndices(j, i));
    }
  }

}

BOOST_AUTO_TEST_SUITE_END();
