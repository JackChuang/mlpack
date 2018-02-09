/**
 * @file kmeans_test.cpp
 * @author Prabhat Sharma
 *
 * Test mlpackMain() of kmeans_main.cpp
 */
#include <string>

#define BINDING_TYPE BINDING_TYPE_TEST
static const std::string testName = "Kmeans";

#include <mlpack/core.hpp>
#include <mlpack/core/util/mlpack_main.hpp>
#include "test_helper.hpp"
#include <mlpack/methods/kmeans/kmeans_main.cpp>

#include <boost/test/unit_test.hpp>
#include "../test_tools.hpp"

using namespace mlpack;

struct KmTestFixture
{
 public:
    KmTestFixture()
    {
        // Cache in the options for this program.
        CLI::RestoreSettings(testName);
    }

    ~KmTestFixture()
    {
        // Clear the settings.
        CLI::ClearSettings();
    }
};

void ResetKmSettings()
{
    CLI::ClearSettings();
    CLI::RestoreSettings(testName);
}

BOOST_FIXTURE_TEST_SUITE(KmeansMainTest, KmTestFixture);

/**
 * Checking that number of Clusters are non negative
 */
    BOOST_AUTO_TEST_CASE(NonNegativeClustersTest)
    {
        constexpr int N = 10;
        constexpr int D = 4;

        arma::mat InputData = arma::randu<arma::mat>(N, D);

        SetInputParam("input", std::move(InputData));
        SetInputParam("clusters", (int) -1); // Invalid

        Log::Fatal.ignoreInput = true;
        BOOST_REQUIRE_THROW(mlpackMain(), std::runtime_error);
        Log::Fatal.ignoreInput = false;
    }


/**
 * Checking that number of Clusters is less than number of points to be clustered
 */
    BOOST_AUTO_TEST_CASE(PointsLessThanClustersTest)
    {
        constexpr int N = 10;
        constexpr int D = 4;

        arma::mat InputData = arma::randu<arma::mat>(N, D);

        SetInputParam("input", std::move(InputData));
        SetInputParam("clusters", (int) 11); // Invalid

        Log::Fatal.ignoreInput = true;
        BOOST_REQUIRE_THROW(mlpackMain(), std::runtime_error);
        Log::Fatal.ignoreInput = false;
    }


/**
 * Checking that percentage is between 0 and 1 when --refined_start is specified
 */
    BOOST_AUTO_TEST_CASE(RefinedStartPercentageTest)
    {
        constexpr int N = 10;
        constexpr int D = 4;
        int C = 2;
        double P = 2.0;
        arma::mat InputData = arma::randu<arma::mat>(N, D);

        SetInputParam("input", std::move(InputData));
        SetInputParam("refined_start", true);
        SetInputParam("clusters", std::move(C));
        SetInputParam("percentage", std::move(P));     // Invalid

        Log::Fatal.ignoreInput = true;
        BOOST_REQUIRE_THROW(mlpackMain(), std::runtime_error);
        Log::Fatal.ignoreInput = false;
    }


/**
 * Checking percentage is non-negative when --refined_start is specified
 */
    BOOST_AUTO_TEST_CASE(NonNegativePercentageTest)
    {
        constexpr int N = 10;
        constexpr int D = 4;
        int C = 2;
        double P = -1.0;
        arma::mat InputData = arma::randu<arma::mat>(N, D);

        SetInputParam("input", std::move(InputData));
        SetInputParam("refined_start", true);
        SetInputParam("clusters", std::move(C));
        SetInputParam("percentage", std::move(P));     // Invalid

        Log::Fatal.ignoreInput = true;
        BOOST_REQUIRE_THROW(mlpackMain(), std::runtime_error);
        Log::Fatal.ignoreInput = false;
    }


/**
 * Checking that size and dimensionality of prediction is correct.
 */
    BOOST_AUTO_TEST_CASE(KmClusteringSizeCheck)
    {
        constexpr int N = 10;
        constexpr int D = 4;
        int C = 2;

        arma::mat InputData = arma::randu<arma::mat>(N, D);
        SetInputParam("input", std::move(InputData));
        SetInputParam("clusters", std::move(C));


        mlpackMain();

        BOOST_REQUIRE_EQUAL(CLI::GetParam<arma::mat>("output").n_rows, N);
        BOOST_REQUIRE_EQUAL(CLI::GetParam<arma::mat>("output").n_cols, D+1);
        BOOST_REQUIRE_EQUAL(CLI::GetParam<arma::mat>("centroid").n_rows, C);
        BOOST_REQUIRE_EQUAL(CLI::GetParam<arma::mat>("centroid").n_cols, D);
    }


/**
 * Checking that size and dimensionality of prediction is correct when --labels_only is specified
 */
    BOOST_AUTO_TEST_CASE(KmClusteringSizeCheckLabelOnly)
    {
        constexpr int N = 10;
        constexpr int D = 4;
        int C = 2;

        arma::mat InputData = arma::randu<arma::mat>(N, D);
        SetInputParam("input", std::move(InputData));
        SetInputParam("clusters", std::move(C));
        SetInputParam("labels_only", true);


        mlpackMain();

        BOOST_REQUIRE_EQUAL(CLI::GetParam<arma::mat>("output").n_rows, N);
        BOOST_REQUIRE_EQUAL(CLI::GetParam<arma::mat>("output").n_cols, 1);
        BOOST_REQUIRE_EQUAL(CLI::GetParam<arma::mat>("centroid").n_rows, C);
        BOOST_REQUIRE_EQUAL(CLI::GetParam<arma::mat>("centroid").n_cols, D);
    }


/**
 * Checking that predictions are not same when --allow_empty_clusters or kill_empty_clusters are specified
 */
    BOOST_AUTO_TEST_CASE(KmClusteringEmptyClustersCheck)
    {
        constexpr int N = 100;
        constexpr int D = 4;
        int C = 95;
        int iterations = 100;
        arma::mat InputData = arma::randu<arma::mat>(N, D);
        arma::mat InitialCentroids=arma::ones(C, D);

        SetInputParam("input", std::move(InputData));
        SetInputParam("clusters", std::move(C));
        SetInputParam("labels_only", true);
        SetInputParam("initial_centroids", std::move(InitialCentroids));
        SetInputParam("max_iterations", std::move(iterations));

        mlpackMain();

        arma::mat NormalOutput;
        NormalOutput = std::move(CLI::GetParam<arma::mat>("output"));

        ResetKmSettings();

        SetInputParam("input", std::move(InputData));
        SetInputParam("clusters", std::move(C));
        SetInputParam("labels_only", true);
        SetInputParam("initial_centroids", std::move(InitialCentroids));
        SetInputParam("allow_empty_clusters", true);
        SetInputParam("max_iterations", std::move(iterations));

        mlpackMain();

        arma::mat AllowEmptyOutput;
        AllowEmptyOutput = std::move(CLI::GetParam<arma::mat>("output"));

        ResetKmSettings();

        SetInputParam("input", std::move(InputData));
        SetInputParam("clusters", std::move(C));
        SetInputParam("labels_only", true);
        SetInputParam("initial_centroids", std::move(InitialCentroids));
        SetInputParam("kill_empty_clusters", true);
        SetInputParam("max_iterations", std::move(iterations));

        mlpackMain();

        arma::mat KillEmptyOutput;
        KillEmptyOutput = std::move(CLI::GetParam<arma::mat>("output"));

        ResetKmSettings();

        //Checking that the results are not identical

        for (int j = 0; j <N ; ++j)
        {
            BOOST_CHECK_NE(NormalOutput(j), AllowEmptyOutput(j));
        }
        for (int j = 0; j <N ; ++j)
        {
            BOOST_CHECK_NE(KillEmptyOutput(j), AllowEmptyOutput(j));
        }

    }


/**
 * Checking that that size and dimensionality of Final Input File is correct when flag --in_place is specified
 */
    BOOST_AUTO_TEST_CASE(KmClusteringResultSizeCheck)
    {
        constexpr int N = 10;
        constexpr int D = 4;
        int C = 2;

        arma::mat InputData = arma::randu<arma::mat>(N, D);
        SetInputParam("input", std::move(InputData));
        SetInputParam("clusters", std::move(C));
        SetInputParam("in_place", true);


        mlpackMain();

        BOOST_REQUIRE_EQUAL(InputData.n_cols, D+1);
        BOOST_REQUIRE_EQUAL(InputData.n_rows, N);
    }


/**
 * Ensuring that absence of Input is checked.
 */
    BOOST_AUTO_TEST_CASE(KmNoInputData)
    {
        constexpr int N = 10;
        constexpr int D = 4;
        int C = 2;
        arma::mat input = arma::randu<arma::mat>(N, D);

        SetInputParam("clusters", std::move(C));

        Log::Fatal.ignoreInput = true;
        BOOST_REQUIRE_THROW(mlpackMain(), std::runtime_error);
        Log::Fatal.ignoreInput = false;
    }

/**
 * Ensuring that absence of Number of Clusters is checked.
 */
    BOOST_AUTO_TEST_CASE(KmClustersNotDefined)
    {
        constexpr int N = 10;
        constexpr int D = 4;
        arma::mat input = arma::randu<arma::mat>(N, D);

        SetInputParam("input", std::move(input));

        Log::Fatal.ignoreInput = true;
        BOOST_REQUIRE_THROW(mlpackMain(), std::runtime_error);
        Log::Fatal.ignoreInput = false;
    }
/**
 * Checking that all the algorithms yield same results
 */
    BOOST_AUTO_TEST_CASE(AlgorithmsSimilarTest)
    {
        constexpr int N = 100;
        constexpr int D = 4;
        int C = 5;
        std::string algo = "naive";
        arma::mat InputData = arma::randu<arma::mat>(N, D);

        SetInputParam("input", std::move(InputData));
        SetInputParam("clusters", std::move(C));
        SetInputParam("algorithm", std::move(algo));

        mlpackMain();

        arma::mat NaiveOutput;
        NaiveOutput = std::move(CLI::GetParam<arma::mat>("output"));

        ResetKmSettings();

        algo = "elkan";

        SetInputParam("input", std::move(InputData));
        SetInputParam("clusters", std::move(C));
        SetInputParam("algorithm", std::move(algo));

        mlpackMain();

        arma::mat ElkanOutput;
        ElkanOutput = std::move(CLI::GetParam<arma::mat>("output"));

        ResetKmSettings();

        algo = "hamerly";

        SetInputParam("input", std::move(InputData));
        SetInputParam("clusters", std::move(C));
        SetInputParam("algorithm", std::move(algo));

        mlpackMain();

        arma::mat HamerlyOutput;
        HamerlyOutput = std::move(CLI::GetParam<arma::mat>("output"));

        ResetKmSettings();

        algo = "dualtree";

        SetInputParam("input", std::move(InputData));
        SetInputParam("clusters", std::move(C));
        SetInputParam("algorithm", std::move(algo));

        mlpackMain();

        arma::mat DualTreeOutput;
        DualTreeOutput = std::move(CLI::GetParam<arma::mat>("output"));

        ResetKmSettings();

        algo = "dualtree-covertree";

        SetInputParam("input", std::move(InputData));
        SetInputParam("clusters", std::move(C));
        SetInputParam("algorithm", std::move(algo));

        mlpackMain();

        arma::mat DualCoverTreeOutput;
        DualCoverTreeOutput = std::move(CLI::GetParam<arma::mat>("output"));

        // Check That all the algorithms yield the same clusters
        CheckMatrices(NaiveOutput, ElkanOutput);
        CheckMatrices(ElkanOutput, HamerlyOutput);
        CheckMatrices(HamerlyOutput, DualTreeOutput);
        CheckMatrices(DualTreeOutput, DualCoverTreeOutput);
    }


BOOST_AUTO_TEST_SUITE_END();
