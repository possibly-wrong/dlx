// Generalized Dancing Links (Knuth: http://arxiv.org/abs/cs/0011047)

#ifndef DLX_H
#define DLX_H

#include <vector>
#include <map>
#include <random>

namespace dlx
{
    typedef std::vector<int> Degrees;

    template<typename Row = int, typename Column = int>
    class DLX
    {
    public:

        // Create "empty" exact cover problem with no rows or columns.
        DLX();
        virtual ~DLX();

        // Add element (row, col) to constraint relation.
        void add(const Row& row, const Column& col);

        // Mark column as optional, so that it may be covered *at most* once.
        void optional(const Column& col);

        // Push/pop row in partial solution; results are undefined if add() or
        // optional() is called *after* push() or pop().
        void push(const Row& row);
        void pop();

        // Search for exact covers, calling found() or stuck() for each
        // terminal node, stopping if either returns false.
        virtual bool search();
        virtual bool found() { return true; }
        virtual bool stuck() { return true; }

        // A solution is a list of selected row *names*; Knuth originally
        // represented each row as a *list* of occupied column names.
        typedef std::vector<Row> Solution;

        // Traverse a single random path in the search tree, returning a list
        // of degrees of each non-leaf vertex on the path.
        template<typename URNG>
        Degrees sample(URNG& g);

    protected:
        struct Node;
        Node *root;
        std::map<Row, Node *> rows;
        std::map<Column, Node *> columns;
        Solution solution;

        void cover(Node *c);
        void uncover(Node *c);

    private:
        DLX(const DLX& copy);
        DLX& operator= (const DLX& rhs);
    };
} // namespace dlx

#include "dlx.hpp"
#endif // DLX_H
