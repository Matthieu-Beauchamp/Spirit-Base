#include "celero/Celero.h"

CELERO_MAIN

class SortFixture : public celero::TestFixture
{
public:

    SortFixture() {}

    virtual std::vector<celero::TestFixture::ExperimentValue>
    getExperimentValues() const override
    {
        std::vector<celero::TestFixture::ExperimentValue> problemSpace;

        // We will run some total number of sets of tests together.
        // Each one growing by a power of 2.
        const int totalNumberOfTests = 6;

        for (int i = 0; i < totalNumberOfTests; i++)
        {
            // ExperimentValues is part of the base class and allows us to specify
            // some values to control various test runs to end up building a nice graph.
            problemSpace.push_back({2 << (i + 1)});
        }

        return problemSpace;
    }

    /// Before each run, build a vector of random integers.
    virtual void
    setUp(const celero::TestFixture::ExperimentValue & experimentValue)
    {
        this->arraySize = experimentValue.Value;
        this->array.reserve(this->arraySize);
    }

    virtual void
    tearDown() override
    {
    }

    /// Before each iteration. A common utility function to push back random ints to sort.
    void
    randomize()
    {
        for (int i = 0; i < this->arraySize; i++)
        {
            this->array.push_back(rand());
        }
    }

    /// After each iteration, clear the vector of random integers.
    void
    clear()
    {
        this->array.clear();
    }

    std::vector<int64_t> testArray;

    std::vector<int64_t> array;
    int64_t arraySize;
};


// For a baseline, I'll choose Bubble Sort.
BASELINE_F(SortRandInts, BubbleSort, SortFixture, 30, 10000)
{
    this->randomize();

    for (int x = 0; x < this->arraySize; x++)
    {
        for (int y = 0; y < this->arraySize - 1; y++)
        {
            if (this->array[y] > this->array[y + 1])
            {
                std::swap(this->array[y], this->array[y + 1]);
            }
        }
    }

    this->clear();
}

BENCHMARK_F(SortRandInts, SelectionSort, SortFixture, 30, 10000)
{
    this->randomize();

    for (int x = 0; x < this->arraySize; x++)
    {
        auto minIdx = x;

        for (int y = x; y < this->arraySize; y++)
        {
            if (this->array[minIdx] > this->array[y])
            {
                minIdx = y;
            }
        }

        std::swap(this->array[x], this->array[minIdx]);
    }

    this->clear();
}


BENCHMARK_F(SortRandInts, stdSort, SortFixture, 30, 10000)
{
    this->randomize();
    std::sort(this->array.begin(), this->array.end());
    this->clear();
}
