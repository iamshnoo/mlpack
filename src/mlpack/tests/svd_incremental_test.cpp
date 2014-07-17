#include <mlpack/core.hpp>
#include <mlpack/methods/amf/amf.hpp>
#include <mlpack/methods/amf/update_rules/svd_incomplete_incremental_learning.hpp>
#include <mlpack/methods/amf/init_rules/random_init.hpp>
#include <mlpack/methods/amf/termination_policies/incomplete_incremental_termination.hpp>
#include <mlpack/methods/amf/termination_policies/simple_tolerance_termination.hpp>
#include <mlpack/methods/amf/termination_policies/validation_RMSE_termination.hpp>

#include <boost/test/unit_test.hpp>
#include "old_boost_test_definitions.hpp"

BOOST_AUTO_TEST_SUITE(SVDIncrementalTest);

using namespace std;
using namespace mlpack;
using namespace mlpack::amf;
using namespace arma;

/**
 * Test for convergence
 */
BOOST_AUTO_TEST_CASE(SVDIncompleteIncrementalConvergenceTest)
{
  mlpack::math::RandomSeed(10);
  sp_mat data;
  data.sprandn(1000, 1000, 0.2);
  SVDIncompleteIncrementalLearning svd(0.01);
  IncompleteIncrementalTermination<SimpleToleranceTermination<sp_mat> > iit;
  AMF<IncompleteIncrementalTermination<SimpleToleranceTermination<sp_mat> >, 
      RandomInitialization, 
      SVDIncompleteIncrementalLearning> amf(iit, RandomInitialization(), svd);
  mat m1,m2;
  amf.Apply(data, 2, m1, m2);
  
  BOOST_REQUIRE_NE(amf.TerminationPolicy().Iteration(), 
                    amf.TerminationPolicy().MaxIterations());
}


BOOST_AUTO_TEST_CASE(SVDIncompleteIncrementalRegularizationTest)
{
  mat dataset;
  data::Load("GroupLens100k.csv", dataset);

  // Generate list of locations for batch insert constructor for sparse
  // matrices.
  arma::umat locations(2, dataset.n_cols);
  arma::vec values(dataset.n_cols);
  for (size_t i = 0; i < dataset.n_cols; ++i)
  {
    // We have to transpose it because items are rows, and users are columns.
    locations(0, i) = ((arma::uword) dataset(0, i));
    locations(1, i) = ((arma::uword) dataset(1, i));
    values(i) = dataset(2, i);
  }

  // Find maximum user and item IDs.
  const size_t maxUserID = (size_t) max(locations.row(0)) + 1;
  const size_t maxItemID = (size_t) max(locations.row(1)) + 1;

  // Fill sparse matrix.
  sp_mat cleanedData = arma::sp_mat(locations, values, maxUserID, maxItemID);
  sp_mat cleanedData2 = cleanedData;

  mlpack::math::RandomSeed(10);
  ValidationRMSETermination<sp_mat> vrt(cleanedData, 2000);
  AMF<IncompleteIncrementalTermination<ValidationRMSETermination<sp_mat> >,
      RandomInitialization,
      SVDIncompleteIncrementalLearning> amf_1(vrt,
                              RandomInitialization(),
                              SVDIncompleteIncrementalLearning(0.001, 0, 0));

  mat m1,m2;
  double RMSE_1 = amf_1.Apply(cleanedData, 2, m1, m2);

  mlpack::math::RandomSeed(10);
  ValidationRMSETermination<sp_mat> vrt2(cleanedData2, 2000);
  AMF<IncompleteIncrementalTermination<ValidationRMSETermination<sp_mat> >,
      RandomInitialization,
      SVDIncompleteIncrementalLearning> amf_2(vrt2,
                              RandomInitialization(),
                              SVDIncompleteIncrementalLearning(0.001, 0.01, 0.01));

  mat m3, m4;
  double RMSE_2 = amf_2.Apply(cleanedData2, 2, m3, m4);
  
  BOOST_REQUIRE_LT(RMSE_2, RMSE_1);
}

BOOST_AUTO_TEST_SUITE_END();
