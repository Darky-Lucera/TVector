# Syntactic sugar vector

Replacement of std::vector with syntactic sugar for easy playing.<br/>
Needs almost C++14. Better C++17.

This utility is derived from an earlier version employed in the MindShake video game engine from Lucera Project. While I can't recall the precise inception date of the initial class, I developed it prior to the establishment of Lucera in 2009, utilizing C++98. Subsequently, I enhanced it to leverage the features introduced in C++11, a transformation that took place several years ago.<br/>
In any case, this is a new implementation.

**Index**:

- [Element access](#element-access)
  - [erase_quick](#erase_quick)
- [Automatic conversions](#automatic-conversions)
- [Extra functionality](#extra-functionality)
  - [if_new](#if_new)
  - [Search operations](#search-operations)
  - [Copy / replace operations](#copy-/-replace-operations)
  - [for_each](#for_each)
  - [Accumulate / reduce](#accumulate-/-reduce)
  - [transform](#transform)
  - [transform reduce](#transform-reduce)
    - [Execution policies](#execution-policies)
  - [Sorting and related operations](#sorting-and-related-operations)
  - [Reverse / Rotate / Shuffle](#reverse-/-Rotate-/-Shuffle)
  - [Min / Max / MinMax](#Min-/-Max-/-MinMax)

## Element access

You can access elements with negative indexes like in Python:

- ```operator[](index)```
- ```at(index)```
- ```insert(index, value)```
- ```emplace(index, value)```
- ```erase(index)```

```cpp
TVector<int> ints {1, 2, 3};

int a = ints[-1];       // 3
ints[-1] = 42;          // {1, 2, 42}

int b = ints.at(-2);    // 2
ints.at(-2) = -42;      // {1, -42, 42}

ints.insert(-1, 123);   // {1, -42, 42, 123}
ints.emplace(-2, 456);  // {1, -42, 42, 456, 123}
ints.erase(-2);         // {1, -42, 42, 123}
```

### erase_quick

It swaps the element to be removed with the last element in the vector to avoid elements copy.<br/>
it does not preserve the order of the elements in the vector.

- ```erase_quick(index)```

```cpp
TVector<int> ints {1, 2, 3};

ints.erase_quick(0);            // {3, 2}
ints.erase_quick(-2);           // {2}
```

## Automatic conversions

You can convert from/to regular std::vector.

- ```asTVector(std::vector)```

```cpp
int funcTVector(TVector<int> &v) {
    return v.size();
}

//-------------------------------------
int funcStdVector(std::vector<int> &v) {
    return v.size();
}

TVector<int>     ints1 {1, 2, 3};
std::vector<int> ints2 {1, 2, 3};

funcTVector(ints1);
funcStdVector(ints1);

funcTVector(asTVector(ints2));  // uses cast. no copy
funcStdVector(ints2);
```

## Extra functionality

### if_new

Some set like funtions.

- ```push_back_if_new(value)```
- ```emplace_back_if_new(value)```

```cpp
TVector<int>     ints {1, 2, 3};

ints.push_back_if_new(1);       // returns a boolean
ints.emplace_back_if_new(2);    // returns a boolean
```

### Search operations

- ```find(value)```: Returns iterator.
- ```find_if(func)```: Returns iterator.
- ```find_if_not(func)```: Returns iterator.
- ```find_last(func)```: Returns reverse iterator.
- ```find_last_if(func)```: Returns reverse iterator.
- ```find_last_if_not(func)```: Returns reverse iterator.
- ```contains(value)```: Returns bool.
- ```get_index(value)```: Returns index or -1.
- ```count(value)```: Returns size_t.
- ```count_if(func)```: Returns bool.
- ```all_of(func)```: Returns bool.
- ```any_of(func)```: Returns bool.
- ```none_of(func)```: Returns bool.

In these operations ```func``` is something like ```bool(const T &value)```

More info:

- [find / find_if / find_if_not](https://en.cppreference.com/w/cpp/algorithm/find)
- [count / count_if](https://en.cppreference.com/w/cpp/algorithm/count)
- [all_of / any_of / none_of](https://en.cppreference.com/w/cpp/algorithm/all_any_none_of)

Examples:

```cpp
TVector<int> ints {1, 2, 1};

auto it = ints.find(1);         // returns an iterator to the first occurrence or end()/cend() if not found

ints.find_if([](int v) {        // returns an iterator to the first occurrence or end()/cend() if not found
    return (v % 2) == 1;
});

ints.find_if_not([](int v) {    // returns an iterator to the first occurrence or end()/cend() if not found
    return v < 5;
});

//--

auto it = ints.find_last(1);    // returns a reverse iterator or rend()/crend() if not found

ints.find_last_if([](int v) {   // returns a reverse iterator or rend()/crend() if not found
    return (v % 2) == 1;
});

ints.find_last_if_not([](int v) { // returns a reverse iterator or rend()/crend() if not found
    return v < 5;
});

//--

ints.count(0);                  // returns the number of elements

ints.count_if([](int v) {       // returns the number of elements
    return (v % 2) == 0;
});

//--

ints.contains(1);               // returns a boolean

ints.get_index(3);              // returns the index or -1 if not found

//--

ints.all_of([](int v) {         // returns true if all elements meet the condition
    return v >= -2; }
);

ints.any_of([](int v) {         // returns true if any element meet the condition
    return v < 0; }
);

ints.none_of([](int v) {        // returns true if no element meet the condition
    return v > 0; }
);
```

### Copy / replace operations

- ```replace(value, other)```: Replaces value with other if found. Returns the current container to allow chaining.
- ```replace_if(bool select(const T &), other)```: Replaces selected elements with other, using a function as a criterion. Returns the current container to allow chaining.
- ```replace_if(bool select(const T &), T other(const T &))```: Replaces selected elements with the result of another function, using a function as a criterion. Returns the current container to allow chaining.

- ```copy(tvector)```: Copies elements to another vector. Returns the current container to allow chaining.
- ```copy_if(tvector, bool select(const T &))```: Copies selected elements to another vector. Returns the current container to allow chaining.
- ```filter(bool select(const T &))```: Like copy_if but it creates a new vector. Returns the created vector.

- ```replace_copy(output, value, other)```: Copies replaced values if found. Returns the current container to allow chaining.
- ```replace_copy_if(bool select(const T &), other)```: Copies replaced values if found, using a function as a criterion. Returns the current container to allow chaining.
- ```replace_copy_if(bool select(const T &), T other(const T &))```: Copies generated values if found, using a function as a criterion. Returns the current container to allow chaining.

More info:

- [replace / replace_if](https://en.cppreference.com/w/cpp/algorithm/replace)
- [copy / copy_if](https://en.cppreference.com/w/cpp/algorithm/copy)
- [replace_copy / replace_copy_if](https://en.cppreference.com/w/cpp/algorithm/replace_copy)

Examples:

```cpp
TVector<int> ints  {1, 2, 3};
TVector<int> ints2 {0, 0}
TVector<int> ints3;

ints.replace(2, -2)         // {1, -2, 3}

    .replace_if([](int v) {
        return v > 0;
    }, 10)                  // {10, -2, 10}

    .copy(ints2)            // ints2 = {0, 0, 10, -2, 10}

    .replace_if([](int v) {
        return v >= 10;
    },
    [](int v) {
        return v /2;
    })                      // {5, -2, 5}

    .copy_if(ints2, [](int v) {
        return v < 0;
    });                     // ints2 = {0, 0, 10, -2, 10, -2}

    .replace_copy(ints3, 5, 8) // ints3 = {8, -2, 8}

    .replace_copy_if(ints3,
        [](int v) {
            return v == 5;
        }, 3)               // ints3 = {8, -2, 8, 3, -2, 3}

    .replace_copy_if(ints3,
        [](int v) {
            return v == 5;
        },
        [](int v) {
            return v * 3;
        });                 // ints3 = {8, -2, 8,  3, -2, 3,  15, -2, 15}
```

Replace to another vector.

```cpp
TVector<int> ints  {1, 2, 3};
TVector output;

ints.replace_copy(output, 2, -2);   // output = {1, -2, 3}

ints.replace_copy_if(output.clear(), // clear also returns this
    [](int v) {
        return v > 0;
    }, 3);                          // output = {3, 3, 3}

ints.replace_copy_if(output.clear(), // clear also returns this
    [](int v) {
        return v > 0;
    },
    [](int v) {
        return v * 2;
    });                             // output = {2, 4, 6}

// Filter returns another vector
auto vec = ints.filter([](int v) {
        return v > 1;
    });                             // vec = {2, 3}
```

### for_each

Applies the given function to every element in vector.

- ```for_each(void op(const T &))```

More info:

- [for_each](https://en.cppreference.com/w/cpp/algorithm/for_each)

Examples:

```cpp
TVector<int> ints {1, 2, 3};

ints.for_each([](int v) {
    printf("value: %d\n", v);
}
```

### Accumulate / reduce

Accumulate and reduce perform accumulation operations on a range of elements. However, accumulate is simpler and more straightforward, as it only performs a left-to-right accumulation of values, while reduce is more flexible, allowing for parallelized and optimized reductions. reduce may offer better performance for large datasets or when parallel execution is possible, but accumulate is often sufficient and easier to use for basic accumulation tasks.

Pseudocode:

```cpp
auto result = init;
for (const auto &v : values) {
    result = op(result, v);
}
return result;
```

The main differences between accumulate and reduce are:

- Order of Evaluation:
  - **accumulate**: Guarantees a strict left-to-right order of evaluation. Each element in the sequence is combined with the accumulated value in a sequential manner.
  - **reduce**: Does not guarantee the order of evaluation. It may apply the binary operation to the elements of the range in any order, depending on the execution policy and the underlying parallel execution strategy.
- Parallel Execution:
  - **reduce**: Designed to exploit parallelism when used with suitable execution policies and when the operation allows for it. It can efficiently utilize multiple threads to process different portions of the sequence concurrently, potentially improving performance.

You can also apply an [execution policy](#execution-policies) to reduce.

More info:

[accumulate])https://en.cppreference.com/w/cpp/algorithm/accumulate)
[reduce])https://en.cppreference.com/w/cpp/algorithm/reduce)

Example:

```cpp
TVector<int>    ints   = { 0, 1, 2, 3, 4, 5 };
TVector<float>  floats = { 0.1f, 1.2f, 2.3f, 3.4f, 4.5f, 5.6f };

ints.accumulate(0, [](const int &a, const int &b) { return a + b; });       // 15
ints.reduce(0, [](const int &a, const int &b) { return a + b; });       // 15
// This will not work using reduce because the order of operations is not guaranteed
ints.accumulate(std::string(), [](const std::string &a, const int &b) {     // "012345"
    return a + std::to_string(b);
});

floats.reduce(0,    [](float a, float b) { return a + b; });
floats.accumulate(0,    [](float a, float b) { return a + b; });            // 15, because the init parameter is an int, so all operations are rounded to int

floats.reduce(0.0f, [](float a, float b) { return a + b; });
floats.accumulate(0.0f, [](float a, float b) { return a + b; });            // 17.1f, because the init parameter is a float
```

### Transform

Applies the given function to the elements of the given input, and stores the result in an output vector.

You can also apply an [execution policy](#execution-policies) to transform.

- ```transform(output, O op(const T &))```:
- ```transform(firstOutput, O op(const T &))```:
- ```transform(firstInput, firstOutput, O op(const T &, const I &))```:

Pseudocode:

```cpp
for (const auto &v : values) {
    result.push_back(op(v));
}
```

More info:

- [transform](https://en.cppreference.com/w/cpp/algorithm/transform)

Examples:

```cpp
TVector<int> ints {1, 2, 3};
TVector output, output2;

//output.resize(ints.size());   // this transform reserves output memory for us
ints.transform(output, [](int v) { return v * 2;}); // output = {2, 4, 6}

output2.resize(ints.size());
ints.transform(output.begin(),  // Now we use output as input
               output2.begin(),
               [](int a, int b) {
                    return a + b;
                });                 // output2 = {3, 6, 9}
```

### Transform Reduce

transform_reduce combines both transforming and reducing operations into a single step, making it faster, simpler, and more memory-efficient compared to chaining transform and reduce. By doing both tasks in one go, it avoids unnecessary iterations over the data, leading to cleaner and more concise code.

You can also apply an [execution policy](#execution-policies) to transform_reduce.

- ```transform_reduce(U init, T reduce(const T &, const T &), T transform(const V &))```

Pseudocode:

```cpp
auto result = init;
for (const auto &v : values) {
    result = reduce(result, transform(v));
}
return result;
```

More info:

- [transform_reduce](https://en.cppreference.com/w/cpp/algorithm/transform_reduce)

#### Execution policies

If you are using C++17 you can also specify the execution policy (as an enum) for transform, reduce and transform_reduce.

- seq:       std::execution::seq: execution may not be parallelized.
- par:       std::execution::par: execution may be parallelized.
- par_unseq: std::execution::par_unseq: execution may be parallelized, vectorized, or migrated across threads.
- unseq:     std::execution::unseq: execution may be vectorized. [C++20]

- ```transform(policy, output, O op(const T &))```
- ```transform(policy, firstOutput, O op(const T &))```
- ```transform(policy, firstInput, firstOutput, O op(const T &, const I &))```
- ```reduce(policy, init, T reduce(const T &, const T &))```
- ```transform_reduce(policy, init, T reduce(const T &, const T &), T transform(const V &))```


### Sorting and related operations

- ```sort()```: Sorts the elements of the vector. The order of equivalent elements are NOT guaranteed.
- ```sort(bool less(const T &, conat T &))```: Sorts the elements of the vector using a given less function. The order of equivalent elements are NOT guaranteed.

- ```stable_sort()```: Sorts the elements of the vector. The order of equivalent elements are guaranteed.
- ```stable_sort(bool less(const T &, conat T &))```: Sorts the elements of the vector using a given less function. The order of equivalent elements are guaranteed.

- ```is_sorted()```: Check if the vector is sorted.
- ```is_sorted(bool less(const T &, conat T &))```: Check if the vector is sorted using a given less function.

- ```binary_search(value)```: Finds an element in a sorted vector.
- ```binary_search(value, bool less(const T &, conat T &))```: Finds an element in a sorted vector using a given less function.

- ```binary_search_it(value)```: Finds an element in a sorted vector and returns an iterator or cend().
- ```binary_search_it(value, bool less(const T &, conat T &))```: Finds an element in a sorted vector, using a given less function,  and returns an iterator or cend().

- ```unique()```: Removes duplicated elements in a sorted vector.

More info:

- [sort](https://en.cppreference.com/w/cpp/algorithm/sort)
- [stable_sort](https://en.cppreference.com/w/cpp/algorithm/stable_sort)
- [is_sorted](https://en.cppreference.com/w/cpp/algorithm/is_sorted)
- [binary_search](https://en.cppreference.com/w/cpp/algorithm/binary_search)
- [unique](https://en.cppreference.com/w/cpp/algorithm/unique)

### Reverse / Rotate / Shuffle

- ```reverse()```: Reverse elements in a vector.
- ```rotate(idx)```: Rotates elements to left (-idx) or to right (idx).
- ```shuffle()```: Reorders elements in the vector, such that each possible permutation of those elements has equal probability of appearance.

More info:

- [reverse](https://en.cppreference.com/w/cpp/algorithm/reverse)
- [rotate](https://en.cppreference.com/w/cpp/algorithm/rotate)
- [shuffle](https://en.cppreference.com/w/cpp/algorithm/random_shuffle)

### Min / Max / MinMax

- ```min()```: Returns the minimum element.
- ```min(bool less(const T &, conat T &))```: Returns the minimum element given a less function.
- ```max()```: Returns the maximum element.
- ```max(bool less(const T &, conat T &))```: Returns the maximum element given a less function.
- ```minmax()```: Returns the minimum and maximum elements.
- ```minmax(bool less(const T &, conat T &))```: Returns the minimum and maximum elements given a less function.

- ```min_it()```: Returns an iterator to the minimum element.
- ```min_it(bool less(const T &, conat T &))```: Returns an iterator to the minimum element given a less function.
- ```max_it()```: Returns an iterator to the maximum element.
- ```max_it(bool less(const T &, conat T &))```: Returns an iterator to the maximum element given a less function.
- ```minmax_it()```: Returns iterators to the minimum and maximum elements.
- ```minmax_it(bool less(const T &, conat T &))```: Returns iterators to the minimum and maximum elements given a less function.

More info:

- [min](https://en.cppreference.com/w/cpp/algorithm/min_element)
- [max](https://en.cppreference.com/w/cpp/algorithm/max_element)
- [minmax](https://en.cppreference.com/w/cpp/algorithm/minmax_element)
