#ifndef FENWICK_TREE_IO_TOOLS_H_
#define FENWICK_TREE_IO_TOOLS_H_

#include <iostream>
#include <vector>
#include <string>

template <typename ValueType>
ValueType ReadValue(std::istream& inputStream)
{
    ValueType value;
    inputStream >> value;
    return value;
}

template <typename ValueType>
void WriteValue(std::ostream& outputStream, 
    const ValueType& value, 
    std::string separator = "")
{
    outputStream << value << separator;
}

template <typename ValueType>
std::vector<ValueType> ReadValues(std::istream& inputStream)
{
    size_t count;
    inputStream >> count;
    std::vector<ValueType> numbers(count);

    for (size_t index = 0; index < count; ++index)
    {
        inputStream >> numbers[index];
    }

    return numbers;
}

template<typename ValueType>
void WriteValues(std::ostream& outputStream,
    const std::vector<ValueType>& values,
    bool printSize = false)
{
    if (printSize)
    {
        outputStream << values.size() << '\n';
    }
    for (size_t index = 0; index < values.size(); ++index)
    {
        outputStream << values[index] << ' ';
    }
    outputStream << '\n';
}



#endif