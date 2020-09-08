#include <iostream>
#include <string>

#include "fenwick_tree.h"


void Run()
{
    std::ostream& out = std::cout;
    std::istream& in = std::cin;
    out << "For a given array of fixed size we process\n"
        << "1) In O(log n) time requests to add a number to the element of array at specified position\n"
        << "2) In O(log n) time requests to get the sum over specified segment of array\n"
        << "Available actions:\n"
        << "1) add <POSITION> <VALUE> (add <VALUE> at <POSITION> )\n"
        << "2) get <BEGIN> <END> (get sum of elements in the range [<BEGIN>, <END>))\n";
    using ValueT = int;

    out << "Enter values count: ";
    size_t valuesCount = 0;
    in >> valuesCount;

    FenwickTree<ValueT, std::plus<ValueT>, std::minus<ValueT>> tree(valuesCount);

    out << "Enter actions count: ";
    size_t actionsCount = 0;
    in >> actionsCount;

    while (actionsCount > 0)
    {
        out << "Enter action: ";
        std::string action;
        in >> action;
        if (action == "add")
        {
            size_t position = 0;
            ValueT value;
            in >> position >> value;
            tree.Update(position, value);
            --actionsCount;
        }
        else if (action == "get")
        {
            size_t begin = 0;
            size_t end = 0;
            in >> begin >> end;
            out << tree.ApplyRange(begin, end) << '\n';
            --actionsCount;
        }
        else
        {
            out << "Unknown action. "
                << "Valid actions are: add <POSITION> <VALUE> and get <BEGIN> <END>\n";
        }
    }
}

int main()
{
    Run();
    return 0;
}

