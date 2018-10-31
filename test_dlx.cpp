//  USAGE
//      test_dlx cover_file mode
//
//  COVER FILE FORMAT
//      num_pairs   Number of 1s in cover matrix.
//      row col     Row and column indices of each 1 in matrix.
//      ...
//      num_opts    Number of optional columns in matrix
//                      (i.e., "at most one" vs. "exactly one" constraints).
//      col         Indices of optional columns.
//      ...
//
//  MODE
//      -1          Output number of solutions at each depth.
//      -2          Output list of all solutions (as lists of row indices).
//      0           Run forever, displaying running estimate of total number
//                      of leave vertices in search tree.
//      +d          Run forever, displaying running estimate of number of
//                      vertices at depth d in search tree.

#include "dlx.h"
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
using std::size_t;

int mode = -1;
std::map<size_t, size_t> counts;

class Solver : public dlx::DLX<int, int>
{
public:
    virtual bool found()
    {
        if (mode == -1)
        {
            ++counts[solution.size()];
        }
        else
        {
            for (auto&& row : solution)
            {
                std::cout << " " << row;
            }
            std::cout << std::endl;
        }
        return true;
    }
};

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cout << "Usage: test_dlx cover_file mode" << std::endl;
        return 0;
    }

    // Enter cover matrix.
    std::ifstream cover(argv[1]);
    Solver solver;
    // Enter number of (row, col) pairs.
    int n;
    cover >> n;
    // Enter pairs.
    for (int i = 0; i < n; ++i)
    {
        int row, col;
        cover >> row >> col;
        solver.add(row, col);
    }
    // Enter number of optional columns.
    cover >> n;
    // Enter optional columns.
    for (int i = 0; i < n; ++i)
    {
        int col;
        cover >> col;
        solver.optional(col);
    }

    // Enter solver mode:
    std::istringstream is(argv[2]);
    is >> mode;

    int period = 100;
    if (mode >= 0)
    {
        size_t samples = 0;
        size_t total = 0;
        std::mt19937 rng;
        while (true)
        {
            for (int i = 0; i < period; ++i)
            {
                dlx::Degrees degrees = solver.sample(rng);
                if (mode != 0)
                {
                    degrees.resize(mode);
                }
                size_t t = 1;
                for (auto&& d : degrees)
                {
                    t *= d;
                }
                total += t;
                ++samples;
            }
            std::cout << samples << " " <<
                static_cast<double>(total) / samples << std::endl;
        }
    }
    else
    {
        solver.search();
        if (mode == -1)
        {
            for (auto&& count : counts)
            {
                std::cout << count.first << " " << count.second << std::endl;
            }
        }
    }
}
