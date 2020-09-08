#include <cassert>
#include <random>
#include <memory>
#include <iostream>

#include "fenwick_tree.h"

template <typename ValueType>
    using UniformDistribution = typename std::enable_if<
    std::is_integral<ValueType>::value || std::is_floating_point<ValueType>::value,
    typename std::conditional<
        std::is_integral<ValueType>::value,
        std::uniform_int_distribution<ValueType>,
        std::uniform_real_distribution<ValueType>
    >::type
>::type;

template <typename ValueType>
class ActionBase
{
public:
    virtual ~ActionBase() = default;
};

template <typename ValueType>
class ActionAdd : public ActionBase<ValueType>
{
public:
    ActionAdd(size_t position, const ValueType& value)
        : position_(position)
        , value_(value)
    {}

    size_t GetPosition() const
    {
        return position_;
    }

    const ValueType& GetValue() const
    {
        return value_;
    }

private:
    size_t position_;
    ValueType value_;
};

template <typename ValueType>
class ActionGet : public ActionBase<ValueType>
{
public:
    ActionGet(size_t begin, size_t end)
        : begin_(begin)
        , end_(end)
    {}

    size_t GetBegin() const
    {
        return begin_;
    }

    size_t GetEnd() const
    {
        return end_;
    }
private:
    size_t begin_;
    size_t end_;
};

size_t GetRandomValuesCount(size_t min, size_t max)
{
    static std::mt19937_64 engine;
    std::uniform_int_distribution<size_t> countGenerator(min, max);
    return countGenerator(engine);
}

template <typename ValueType>
std::unique_ptr<ActionBase<ValueType>> GetRandomAction(
        size_t treeSize, const ValueType& min, const ValueType& max)
{
    assert(treeSize > 0);
    static std::mt19937_64 engine;
    std::uniform_int_distribution<size_t> typeGenerator(0, 1);
    size_t type = typeGenerator(engine); // 0 -> Add, 1 -> Get
    std::unique_ptr<ActionBase<ValueType>> action = nullptr;
    if (type == 0)
    {
        std::uniform_int_distribution<size_t> positionGenerator(0, treeSize - 1);
        UniformDistribution<ValueType> valueGenerator(min, max);
        action = std::make_unique<ActionAdd<ValueType>>(
            positionGenerator(engine), valueGenerator(engine));
    }
    else if (type == 1)
    {
        std::uniform_int_distribution<size_t> beginGenerator(0, treeSize - 1);
        std::uniform_int_distribution<size_t> endGenerator(0, treeSize);
        size_t begin = beginGenerator(engine);
        size_t end = endGenerator(engine);
        action = std::make_unique<ActionGet<ValueType>>(
            std::min(begin, end), std::max(begin, end));
    }
    return action;
}

template <typename ValueType>
std::vector<std::unique_ptr<ActionBase<ValueType>>> GetRandomActions(
        size_t treeSize, const ValueType& min, const ValueType& max)
{
    std::vector<std::unique_ptr<ActionBase<ValueType>>> actions;
    while (treeSize > 0)
    {
        actions.push_back(GetRandomAction<ValueType>(treeSize, min, max));
        --treeSize;
    }
    return actions;
}


template <typename ValueType>
void PrintAction(std::ostream& out, const ActionBase<ValueType>& action)
{
    const ActionAdd<ValueType>* actionAdd = dynamic_cast<const ActionAdd<ValueType>*>(&action);
    if (actionAdd != nullptr)
    {
        out << "add " << actionAdd->GetPosition() << ' ' << actionAdd->GetValue() << '\n';
        return;
    }

    const ActionGet<ValueType>* actionGet = dynamic_cast<const ActionGet<ValueType>*>(&action);
    if (actionGet != nullptr)
    {
        out << "get " << actionGet->GetBegin() << ' ' << actionGet->GetEnd() << '\n';
    }
    // Unknown action for the queue
    assert(false);
}

template <typename ValueType>
void PrintActions(std::ostream& out, const std::vector<std::unique_ptr<ActionBase<ValueType>>>& actions)
{
    for (size_t index = 0; index < actions.size(); ++index)
    {
        PrintAction(out, *actions[index]);
    }
}

template<typename ValueT, typename FunctorT, typename InverseFunctorT>
class NaiveFenwickTree
{
public:
    typedef ValueT ValueType;
    typedef FunctorT FunctorType;
    typedef InverseFunctorT InverseFunctorType;

public:
    explicit NaiveFenwickTree(size_t size)
        : values_(size, ValueType())
    {
        Initialize(ValueType());
    }

    explicit NaiveFenwickTree(const std::vector<ValueType>& values)
        : values_(values.size(), ValueType())
    {
        Initialize(values);
    }

    void Update(size_t position, const ValueType& value)
    {
        values_[position] = functor_(values_[position], value);
    }

    ValueType ApplyRange(size_t begin, size_t end)
    {
        assert(begin <= end);
        ValueType result = ValueType();
        for (size_t index = begin; index != end; ++index)
        {
            result = functor_(result, values_[index]);
        }
        return result;
    }

private:
    void Initialize(const std::vector<ValueType>& values)
    {
        for (size_t index = 0; index < values_.size(); ++index)
        {
            Update(index, values[index]);
        }
    }

    void Initialize(const ValueType& value)
    {
        for (size_t index = 0; index < values_.size(); ++index)
        {
            Update(index, value);
        }
    }

private:
    std::vector<ValueType> values_;
    InverseFunctorType inverse_functor_;
    FunctorType functor_;
};

template <typename ValueType>
bool RunTest(
    std::ostream& out,
    size_t treeSize,
    const std::vector<std::unique_ptr<ActionBase<ValueType>>>& actions)
{
    FenwickTree<ValueType, std::plus<ValueType>, std::minus<ValueType>> tree(treeSize);
    NaiveFenwickTree<ValueType, std::plus<ValueType>, std::minus<ValueType>> naiveTree(treeSize);
    for (size_t index = 0; index < actions.size(); ++index)
    {
        const ActionAdd<ValueType>* actionAdd = dynamic_cast<
            const ActionAdd<ValueType>*>(actions[index].get());
        if (actionAdd != nullptr)
        {
            tree.Update(actionAdd->GetPosition(), actionAdd->GetValue());
            naiveTree.Update(actionAdd->GetPosition(), actionAdd->GetValue());
            continue;
        }
        const ActionGet<ValueType>* actionGet = dynamic_cast<
            const ActionGet<ValueType>*>(actions[index].get());
        if (actionGet != nullptr)
        {
            ValueType result = tree.ApplyRange(actionGet->GetBegin(), actionGet->GetEnd());
            ValueType naiveResult = naiveTree.ApplyRange(actionGet->GetBegin(), actionGet->GetEnd());
            if (result != naiveResult)
            {
                out << "Wrong result for action #" << index << ": ";
                PrintAction(out, *actionGet);
                out << "for sequence of actions:\n";
                PrintActions(out, actions);
                return false;
            }
            continue;
        }
        // Unknown action for the tree
        assert(false);
    }
    out << "Test passed\n";
    return true;
}


int main()
{
    for (int attempt = 0; attempt < 10; ++attempt)
    {
        size_t treeSize = GetRandomValuesCount(1, 10);
        if (!RunTest<int>(std::cout, treeSize, GetRandomActions<int>(treeSize, -10, 10)))
        {
            return 1;
        }
    }
    return 0;
}

