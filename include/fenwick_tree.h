#ifndef FENWICK_TREE_FENWICK_TREE_H_
#define FENWICK_TREE_FENWICK_TREE_H_

#include <vector>

template<typename ValueT, typename FunctorT, typename InverseFunctorT>
class FenwickTree
{
public:
    typedef ValueT ValueType;
    typedef FunctorT FunctorType;
    typedef InverseFunctorT InverseFunctorType;

public:
    explicit FenwickTree(size_t size)
        : aggregate_(size, ValueType())
    {
        Initialize(ValueType());
    }

    explicit FenwickTree(const std::vector<ValueType>& values)
        : aggregate_(values.size(), ValueType())
    {
        Initialize(values);
    }

    void Update(size_t position, const ValueType& value)
    {
        for (; position < aggregate_.size(); position = (position | (position + 1)))
        {
            aggregate_[position] = functor_(aggregate_[position], value);
        }
    }

    ValueType ApplyRange(size_t begin, size_t end)
    {
        return inverse_functor_(ApplyPrefix(end), ApplyPrefix(begin));
    }

private:
    void Initialize(const std::vector<ValueType>& values)
    {
        for (size_t index = 0; index < aggregate_.size(); ++index)
        {
            Update(index, values[index]);
        }
    }

    void Initialize(const ValueType& value)
    {
        for (size_t index = 0; index < aggregate_.size(); ++index)
        {
            Update(index, value);
        }
    }

    ValueType ApplyPrefix(size_t end)
    {
        ValueType result = ValueType();

        for (int position = static_cast<int>(end)-1;
            position >= 0;
            position = (position & (position + 1)) - 1)
        {
            result = functor_(result, aggregate_[position]);
        }
        return result;
    }

private:
    std::vector<ValueType> aggregate_;
    InverseFunctorType inverse_functor_;
    FunctorType functor_;
};

#endif