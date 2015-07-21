// FenwickTree.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include <functional>
#include <string>

#include "FenwickTree.h"
#include "IOTools.h"

// For a given array of fixed size we process
// 1) In O(1) time requests to add a number to the element of array at specified position
// 2) In O(log n) time requests to get the sum over specified segment of array

int main()
{
    std::istream& inputStream = std::cin;
    std::ostream& outputStream = std::cout;
    auto values = ReadValues<int>(inputStream);
    
    FenwickTree<int, std::plus<int>, std::minus<int>> tree(values);
    
    auto requestsCount = ReadValue<size_t>(inputStream);
    for (size_t requestIndex = 0; requestIndex < requestsCount; ++requestIndex)
    {
        auto command = ReadValue<std::string>(inputStream);
        if (command == "ADD")
        {
            auto position = ReadValue<size_t>(inputStream);
            auto value = ReadValue<int>(inputStream);
            tree.Update(position, value);
        }
        else if (command == "GET")
        {
            auto begin = ReadValue<size_t>(inputStream);
            auto end = ReadValue<size_t>(inputStream);
            WriteValue(outputStream, tree.ApplyRange(begin, end), "\n");
        }
    }

	return 0;
}

