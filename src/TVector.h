#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>
#include <algorithm>
#include <functional>
#include <numeric>
#include <cmath>

#if __cplusplus >= 201703L || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
    #include <random>
    #include <execution>
#endif

namespace MindShake {

//#if __cplusplus >= 201703L || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)

    enum class ExecutionPolicy {
        seq,        // std::execution::seq: execution may not be parallelized.
        par,        // std::execution::par: execution may be parallelized.
        par_unseq,  // std::execution::par_unseq: execution may be parallelized, vectorized, or migrated across threads.
        unseq,      // std::execution::unseq: execution may be vectorized. [C++20]
    };

//#endif

//-------------------------------------
template <class T, class Allocator = std::allocator<T>>
class TVector : public std::vector<T, Allocator> {
public:
    using vector  = std::vector<T, Allocator>;
    using tvector = TVector<T, Allocator>;

    using std::vector<T, Allocator>::vector;        // Inherits constructors
    using std::vector<T, Allocator>::operator =;
    // Element access
    //using std::vector<T, Allocator>::at;
    //using std::vector<T, Allocator>::operator [];
    using std::vector<T, Allocator>::front;
    using std::vector<T, Allocator>::back;
    using std::vector<T, Allocator>::data;
    // Iterators
    using std::vector<T, Allocator>::begin;
    using std::vector<T, Allocator>::cbegin;
    using std::vector<T, Allocator>::end;
    using std::vector<T, Allocator>::cend;
    using std::vector<T, Allocator>::rbegin;
    using std::vector<T, Allocator>::crbegin;
    using std::vector<T, Allocator>::rend;
    using std::vector<T, Allocator>::crend;
    // Capacity
    using std::vector<T, Allocator>::empty;
    using std::vector<T, Allocator>::size;
    using std::vector<T, Allocator>::max_size;
    using std::vector<T, Allocator>::reserve;
    using std::vector<T, Allocator>::capacity;
    using std::vector<T, Allocator>::shrink_to_fit;
    // Modifiers
    //using std::vector<T, Allocator>::clear;
    using std::vector<T, Allocator>::insert;
    using std::vector<T, Allocator>::emplace;
    using std::vector<T, Allocator>::erase;
    using std::vector<T, Allocator>::push_back;
    using std::vector<T, Allocator>::emplace_back;
    using std::vector<T, Allocator>::pop_back;
    using std::vector<T, Allocator>::resize;
    using std::vector<T, Allocator>::swap;

    // GCC needs this
    using size_type       = typename vector::size_type;
    using difference_type = typename vector::difference_type;
    using iterator        = typename vector::iterator;
    using const_iterator  = typename vector::const_iterator;
    using reverse_iterator       = typename vector::reverse_iterator;
    using const_reverse_iterator = typename vector::const_reverse_iterator;

    // Short names
    using sizet           = size_type;
    using ptrdiff         = difference_type;
    using iter            = iterator;
    using citer           = const_iterator;
    using riter           = reverse_iterator;
    using criter          = const_reverse_iterator;

    // Functions
    using FuncLess        = std::function<bool(const T &, const T &)>;

    using pair            = std::pair<T &, T &>;
    using cpair           = const std::pair<const T &, const T &>;

public:
    constexpr TVector()                             = default;
    constexpr TVector(const vector &o) : vector(o) {}
    constexpr TVector(vector &&o) : vector(o) {}
    constexpr TVector(const tvector &)              = default;
    constexpr TVector(tvector &&)                   = default;

    constexpr TVector & operator=(const tvector &)  = default;
    constexpr TVector & operator=(tvector &&)       = default;

    constexpr operator       vector &()                                     { return *reinterpret_cast<vector *>(this);                     }
    constexpr operator const vector &() const                               { return *reinterpret_cast<const vector *>(this);               }

    constexpr const T & operator[](ptrdiff idx) const                       { return vector::operator[](correctInsideIdx(idx));             }
    constexpr       T & operator[](ptrdiff idx)                             { return vector::operator[](correctInsideIdx(idx));             }

    constexpr const T & at(ptrdiff idx) const                               { return vector::at(correctInsideIdx(idx));                     }
    constexpr       T & at(ptrdiff idx)                                     { return vector::at(correctInsideIdx(idx));                     }

    // Clear
    //---------------------------------
    constexpr tvector & clear()                                             { vector::clear(); return *this;                                }

    // Insert methods
    //---------------------------------
    // If the users wants to insert at end, it should use insert(-1, value)
    // Note: -1 == end(), -2 == end() - 1, ...
    constexpr iterator  insert(ptrdiff idx, const T &value)                 { return insert(correct(idx), value);                           }
    template <class Input>
    constexpr iterator  insert(ptrdiff idx, Input first, Input last)        { return insert(correct(idx), first, last);                     }
    constexpr iterator  insert(ptrdiff idx, std::initializer_list<T> list)  { return insert(correct(idx), list);                            }
    constexpr iterator  insert(ptrdiff idx, const vector &v)                { return insert(correct(idx), v.cbegin(), v.cend());            }

    // Emplace methods
    //---------------------------------
    // If the users wants to emplace at end, it should use emplace(-1, value)
    // Note: -1 == end(), -2 == end() - 1, ...
    template <class... Args>
    constexpr iterator  emplace(ptrdiff idx, Args&&... args)                { return emplace(correct(idx), std::forward<Args>(args)...);    }

    // Add new elements
    //---------------------------------
    constexpr bool push_back_if_new(const T &value) {
        auto it = std::find(cbegin(), cend(), value);
        if (it == cend()) {
            push_back(value);
            return true;
        }

        return false;
    }

    //---------------------------------
    constexpr bool push_back_if_new(T &&value) {
        auto it = std::find(cbegin(), cend(), value);
        if (it == cend()) {
            push_back(std::move(value));
            return true;
        }

        return false;
    }

    //---------------------------------
    template <class... Args>
    constexpr bool emplace_back_if_new(Args &&... args) {
        T    obj(std::forward<Args>(args)...);

        auto it = std::find(cbegin(), cend(), obj);
        if (it == cend()) {
            emplace_back(std::move(obj));
            return true;
        }

        return false;
    }

    // Erase methods
    //---------------------------------
    // If the users wants to erase the last element, it should use erase(-1)
    // Note: -1 == end() - 1, -2 == end() - 2, ...
    constexpr iterator  erase(ptrdiff idx)                                  { return erase(correctInside(idx));                             }
    // Removes the elements in the range [first, last)
    constexpr iterator  erase(ptrdiff first, ptrdiff last)                  { return erase(correctInside(first), correctInside(last));      }

    // Changes element order
    constexpr tvector & erase_quick(ptrdiff idx) {
        std::swap(data()[correctInsideIdx(idx)], data()[size() - 1]);
        pop_back();
        return *this;
    }

    // Find
    //---------------------------------
    constexpr const_iterator find(const T &value) const                     { return std::find(cbegin(), cend(), value);                    }
    constexpr iterator       find(const T &value)                           { return std::find( begin(),  end(), value);                    }

    constexpr const_iterator find_if(std::function<bool(const T &)> op) const {
        return std::find_if(cbegin(), cend(), op);
    }
    constexpr iterator       find_if(std::function<bool(const T &)> op)     { return std::find_if(begin(), end(), op);                      }

    constexpr const_iterator find_if_not(std::function<bool(const T &)> op) const {
        return std::find_if_not(cbegin(), cend(), op);
    }
    constexpr iterator       find_if_not(std::function<bool(const T &)> op) { return std::find_if_not(begin(), end(), op);                  }

    //-- C++23 --
    constexpr const_reverse_iterator find_last(const T &value) const        { return std::find(crbegin(), crend(), value);                  }
    constexpr reverse_iterator       find_last(const T &value)              { return std::find(rbegin(), rend(), value);                    }

    constexpr const_reverse_iterator find_last_if(std::function<bool(const T &)> op) const {
        return std::find_if(crbegin(), crend(), op);
    }
    constexpr reverse_iterator       find_last_if(std::function<bool(const T &)> op) {
        return std::find_if(rbegin(), rend(), op);
    }

    constexpr const_reverse_iterator find_last_if_not(std::function<bool(const T &)> op) const {
        return std::find_if_not(crbegin(), crend(), op);
    }
    constexpr reverse_iterator       find_last_if_not(std::function<bool(const T &)> op) {
        return std::find_if_not(rbegin(), rend(), op);
    }

    // Contains
    //---------------------------------
    constexpr bool contains(const T &value)                                 { return std::find(cbegin(), cend(), value) != cend();          }

    // Get Index
    //---------------------------------
    constexpr ptrdiff_t get_index(const T &value) {
        auto it = std::find(cbegin(), cend(), value);
        if (it != cend()) {
            return it - cbegin();
        }

        return -1;
    }

    // Count
    //---------------------------------
    constexpr size_type count(const T &value) const                        { return std::count(cbegin(), cend(), value);                    }
    constexpr size_type count_if(std::function<bool(const T &)> op) const  { return std::count_if(cbegin(), cend(), op);                    }

    // Check
    //---------------------------------
    constexpr bool all_of(std::function<bool(const T &)> op) const         { return std::all_of(cbegin(), cend(), op);                      }
    constexpr bool any_of(std::function<bool(const T &)> op) const         { return std::any_of(cbegin(), cend(), op);                      }
    constexpr bool none_of(std::function<bool(const T &)> op) const        { return std::none_of(cbegin(), cend(), op);                     }

    // Replace
    //---------------------------------
    constexpr tvector & replace(const T &oldValue, const T &newValue) {
        std::replace(begin(), end(), oldValue, newValue);
        return *this;
    }
    constexpr tvector & replace_if(std::function<bool(const T &)> op, const T &newValue) {
        std::replace_if(begin(), end(), op, newValue);
        return *this;
    }
    constexpr tvector & replace_if(std::function<bool(const T &)> op, std::function<T(const T &)> newValue) {
        for (auto current = begin(); current != end(); ++current) {
            if (op(*current)) {
                *current = newValue(*current);
            }
        }
        return *this;
    }

    // Replace and copy
    //---------------------------------
    constexpr tvector & replace_copy(tvector &output, const T &oldValue, const T &newValue) {
        std::replace_copy(cbegin(), cend(), std::back_inserter(output), oldValue, newValue);
        return *this;
    }

    constexpr tvector & replace_copy_if(tvector &output, std::function<bool(const T &)> op, const T &newValue) {
        std::replace_copy_if(cbegin(), cend(), std::back_inserter(output), op, newValue);
        return *this;
    }

    constexpr tvector & replace_copy_if(tvector &output, std::function<bool(const T &)> op, std::function<T(const T &)> newValue) {
        for (auto current = cbegin(); current != cend(); ++current) {
            if (op(*current)) {
                output.push_back(newValue(*current));
            } else {
                output.push_back(*current);
            }
        }
        return *this;
    }

    // Copy
    //---------------------------------
    constexpr tvector & copy(tvector &output) {
        output.reserve(size());
        std::copy(cbegin(), cend(), std::back_inserter(output));
        return *this;
    }

    constexpr const tvector & copy(tvector &output) const {
        output.reserve(size());
        std::copy(cbegin(), cend(), std::back_inserter(output));
        return *this;
    }

    constexpr tvector & copy_if(tvector &output, std::function<bool(const T &)> op) {
        std::copy_if(cbegin(), cend(), std::back_inserter(output), op);
        return *this;
    }

    constexpr const tvector & copy_if(tvector &output, std::function<bool(const T &)> op) const {
        std::copy_if(cbegin(), cend(), std::back_inserter(output), op);
        return *this;
    }

    // Filter
    //---------------------------------
    constexpr tvector filter(std::function<bool(const T &)> op) {
        tvector output;
        std::copy_if(cbegin(), cend(), std::back_inserter(output), op);
        return output;
    }

    constexpr const tvector filter(std::function<bool(const T &)> op) const {
        tvector output;
        std::copy_if(cbegin(), cend(), std::back_inserter(output), op);
        return output;
    }

    // For each
    //---------------------------------
    constexpr const tvector & for_each(std::function<void(const T &)> op) const {
        std::for_each(cbegin(), cend(), op);
        return *this;
    }
    constexpr tvector & for_each(std::function<void(T &)> op) {
        std::for_each(begin(), end(), op);
        return *this;
    }

    // Sort
    //---------------------------------
    constexpr tvector & sort()                                              { std::sort(begin(), end()); return *this;              }
    constexpr tvector & sort(FuncLess less)                                 { std::sort(begin(), end(), less); return *this;        }

    constexpr tvector & stable_sort()                                       { std::stable_sort(begin(), end()); return *this;       }
    constexpr tvector & stable_sort(FuncLess less)                          { std::stable_sort(begin(), end(), less); return *this; }

    constexpr bool is_sorted() const                                        { return std::is_sorted(cbegin(), cend());              }
    constexpr bool is_sorted(FuncLess less) const                           { return std::is_sorted(cbegin(), cend(), less);        }

    // Search on sorted vectors
    constexpr bool binary_search(const T &value) const                      { return std::binary_search(cbegin(), cend(), value);   }
    constexpr bool binary_search(const T &value, FuncLess less) const       { return std::binary_search(cbegin(), cend(), value, less); }

    constexpr citer binary_search_it(const T &value) const {
        if (empty() == false) {
            auto it = std::lower_bound(cbegin(), cend(), value);
            if ((it != cend()) && (value >= *it))
                return it;
        }

        return cend();
    }
    constexpr citer binary_search_it(const T &value, FuncLess less) const {
        if (empty() == false) {
            auto it = std::lower_bound(cbegin(), cend(), value, less);
            if ((it != cend()) && (value >= *it))
                return it;
        }

        return cend();
    }

    // Unique
    //---------------------------------
    constexpr tvector & unique()                                            { erase(std::unique(begin(), end()), end()); return *this; }

    // Reverse
    //---------------------------------
    constexpr tvector & reverse()                                           { std::reverse(begin(), end()); return *this;           }

    // Rotate
    //---------------------------------
    constexpr tvector & rotate(ptrdiff idx) {
        if (idx > 0)
            std::rotate(begin(), begin() + idx, end());
        else if (idx < 0)
            std::rotate(rbegin(), rbegin() - idx, rend());

        return *this;
    }

    // Shuffle
    //---------------------------------
#if __cplusplus >= 201703L || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
    static inline std::random_device   rd;
    static inline std::mt19937         g { rd() };

    constexpr tvector &shuffle()                                            { std::shuffle(begin(), end(), g); return *this; }
#else
    constexpr tvector & shuffle()                                           { std::random_shuffle(begin(), end()); return *this;    }
#endif

    // Min/Max
    //---------------------------------
  protected:
    enum class MinMax { Min, Max};

    // To avoid repeating the code
    template <typename Iterator, typename FuncLess = std::less<T>>
    const T &find_extremum(Iterator begin, Iterator end, const MinMax &minmax, const FuncLess &less = FuncLess {}) const {
        if (empty() == false) {
            return (minmax == MinMax::Min) ? *std::min_element(begin, end, less) : *std::max_element(begin, end, less);

        }

        static const T empty {};
        return empty;
    }

  public:
    constexpr const T & min() const                                          { return find_extremum(cbegin(), cend(), MinMax::Min);                 }
    constexpr       T & min()                                                { return const_cast<T &>(find_extremum(begin(), end(), MinMax::Min));  }

    constexpr const T & min(FuncLess less) const                             { return find_extremum(cbegin(), cend(), MinMax::Min, less);           }
    constexpr       T & min(FuncLess less)                                   { return const_cast<T &>(find_extremum(begin(), end(), MinMax::Min, less));    }

    constexpr const T & max() const                                          { return find_extremum(cbegin(), cend(), MinMax::Max);                 }
    constexpr       T & max()                                                { return const_cast<T &>(find_extremum(begin(), end(), MinMax::Max));  }

    constexpr const T & max(FuncLess less) const                             { return find_extremum(cbegin(), cend(), MinMax::Max, less);           }
    constexpr       T & max(FuncLess less)                                   { return const_cast<T &>(find_extremum(begin(), end(), MinMax::Max, less));    }

    // TODO: Try to do not repeat this code
    pair minmax(FuncLess less = std::less<T> {}) {
        if (empty() == false) {
            auto p = std::minmax_element(begin(), end(), less);
            return std::make_pair(std::ref(*p.first), std::ref(*p.second));
        }

        static T empty {};
        return std::make_pair(std::ref(empty), std::ref(empty));
    }
    cpair minmax(FuncLess less = std::less<T> {}) const {
        if (empty() == false) {
            auto p = std::minmax_element(cbegin(), cend(), less);
            return std::make_pair(std::cref(*p.first), std::cref(*p.second));
        }

        static const T empty {};
        return std::make_pair(std::cref(empty), std::cref(empty));
    }

    // Min/Max returning iterators
    //---------------------------------
    constexpr const_iterator min_it() const                                 { return std::min_element(cbegin(), cend());            }
    constexpr iterator       min_it()                                       { return std::min_element( begin(),  end());            }
    constexpr const_iterator min_it(FuncLess less) const                    { return std::min_element(cbegin(), cend(), less);      }
    constexpr iterator       min_it(FuncLess less)                          { return std::min_element( begin(),  end(), less);      }

    constexpr const_iterator max_it() const                                 { return std::max_element(cbegin(), cend());            }
    constexpr iterator       max_it()                                       { return std::max_element( begin(),  end());            }
    constexpr const_iterator max_it(FuncLess less) const                    { return std::max_element(cbegin(), cend(), less);      }
    constexpr iterator       max_it(FuncLess less)                          { return std::max_element( begin(),  end(), less);      }

    constexpr std::pair<citer, citer> minmax_it() const                     { return std::minmax_element(cbegin(), cend());         }
    constexpr std::pair< iter,  iter> minmax_it()                           { return std::minmax_element( begin(),  end());         }
    constexpr std::pair<citer, citer> minmax_it(FuncLess less) const        { return std::minmax_element(cbegin(), cend(), less);   }
    constexpr std::pair< iter,  iter> minmax_it(FuncLess less)              { return std::minmax_element( begin(),  end(), less);   }

    // Transform
    //---------------------------------
    // Transforms and stores each component of the vector using the op function into the output iterator.
    // output[i] = op(this[i])
    // Usually: std::function<O(const T &)>
    //---------------------------------
    template <class Output, class UnaryOperation>
    constexpr const tvector & transform(Output firstOutput, UnaryOperation op) const {
        std::transform(cbegin(), cend(), firstOutput, op);
        return *this;
    }
    template <class Output, class UnaryOperation>
    constexpr tvector & transform(Output firstOutput, UnaryOperation op) {
        std::transform(cbegin(), cend(), firstOutput, op);
        return *this;
    }

    // Execution policy must be ne of:
    //   - std::execution::seq: execution may not be parallelized.
    //   - std::execution::par: execution may be parallelized.
    //   - std::execution::par_unseq: execution may be parallelized, vectorized, or migrated across threads.
    //   - std::execution::unseq: execution may be vectorized. [C++20]

    template <class Output, class UnaryOperation>
    constexpr const tvector & transform(ExecutionPolicy policy, Output firstOutput, UnaryOperation op) const {
        unconst().transform(policy, firstOutput, op);
        return *this;
    }
    template <class Output, class UnaryOperation>
    constexpr tvector & transform(ExecutionPolicy policy, Output firstOutput, UnaryOperation op) {
        switch (policy) {
#if __cplusplus >= 201703L || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
            case ExecutionPolicy::seq:
                std::transform(std::execution::seq, cbegin(), cend(), firstOutput, op);
                break;

            case ExecutionPolicy::par:
                std::transform(std::execution::par, cbegin(), cend(), firstOutput, op);
                break;

            case ExecutionPolicy::par_unseq:
                std::transform(std::execution::par_unseq, cbegin(), cend(), firstOutput, op);
                break;

    #if __cplusplus >= 202002L || (defined(_MSVC_LANG) && _MSVC_LANG >= 202002L)
            case ExecutionPolicy::unseq:
                std::transform(std::execution::unseq, cbegin(), cend(), firstOutput, op);
                break;
    #endif

            default:
                std::transform(cbegin(), cend(), firstOutput, op);
                break;
#endif
        }
        
        return *this;
    }

    // Transforms and stores in output each component of the vector combined with the firstInput iterator elements using the op function.
    // output[i] = op(this[i], input[i])
    // Usually: std::function<O(const T &, const I &)>
    //---------------------------------
    template <class Input, class Output, class BinaryOperation>
    constexpr const tvector & transform(Input firstInput, Output firstOutput, BinaryOperation op) const {
        std::transform(cbegin(), cend(), firstInput, firstOutput, op);
        return *this;
    }
    template <class Input, class Output, class BinaryOperation>
    constexpr tvector & transform(Input firstInput, Output firstOutput, BinaryOperation op) {
        std::transform(cbegin(), cend(), firstInput, firstOutput, op);
        return *this;
    }

    // Execution policy must be ne of:
    //   - std::execution::seq: execution may not be parallelized.
    //   - std::execution::par: execution may be parallelized.
    //   - std::execution::par_unseq: execution may be parallelized, vectorized, or migrated across threads.
    //   - std::execution::unseq: execution may be vectorized. [C++20]
    template <class Input, class Output, class BinaryOperation>
    constexpr const tvector & transform(ExecutionPolicy policy, Input firstInput, Output firstOutput, BinaryOperation op) const {
        unconst().transform(policy, firstInput, firstOutput, op);
        return *this;
    }
    template <class Input, class Output, class BinaryOperation>
    constexpr tvector & transform(ExecutionPolicy policy, Input firstInput, Output firstOutput, BinaryOperation op) {
        switch (policy) {
#if __cplusplus >= 201703L || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
            case ExecutionPolicy::seq:
                std::transform(std::execution::seq, cbegin(), cend(), firstInput, firstOutput, op);
                break;

            case ExecutionPolicy::par:
                std::transform(std::execution::par, cbegin(), cend(), firstInput, firstOutput, op);
                break;

            case ExecutionPolicy::par_unseq:
                std::transform(std::execution::par_unseq, cbegin(), cend(), firstInput, firstOutput, op);
                break;

    #if __cplusplus >= 202002L || (defined(_MSVC_LANG) && _MSVC_LANG >= 202002L)
            case ExecutionPolicy::unseq:
                std::transform(std::execution::unseq, cbegin(), cend(), firstInput, firstOutput, op);
                break;
    #endif

            default:
                std::transform(cbegin(), cend(), firstInput, firstOutput, op);
                break;

#endif
        }
        return *this;
    }

    // Transforms and stores each component of the vector using the op function into the output.
    // Output will be resized to the same size as this vector.
    // output[i] = op(this[i])
    // Usually: std::function<O(const T &)>
    //---------------------------------
    template <template <typename...> class OutputClass, typename... Args, class UnaryOperation>
    constexpr const tvector & transform(OutputClass<Args...> &output, UnaryOperation op) const {
        if (output.size() < size())
            output.resize(size());

        std::transform(cbegin(), cend(), output.begin(), op);
        return *this;
    }

    template <template <typename...> class OutputClass, typename... Args, class UnaryOperation>
    constexpr tvector & transform(OutputClass<Args...> &output, UnaryOperation op) {
        if (output.size() < size())
            output.resize(size());

        std::transform(cbegin(), cend(), output.begin(), op);
        return *this;
    }

    // Execution policy must be ne of:
    //   - std::execution::seq: execution may not be parallelized.
    //   - std::execution::par: execution may be parallelized.
    //   - std::execution::par_unseq: execution may be parallelized, vectorized, or migrated across threads.
    //   - std::execution::unseq: execution may be vectorized. [C++20]
    template <template <typename...> class OutputClass, typename... Args, class UnaryOperation>
    constexpr const tvector & transform(ExecutionPolicy policy, OutputClass<Args...> &output, UnaryOperation op) const {
        unconst().transform(policy, output, op);
        return *this;
    }

    template <template <typename...> class OutputClass, typename... Args, class UnaryOperation>
    constexpr tvector & transform(ExecutionPolicy policy, OutputClass<Args...> &output, UnaryOperation op) {
        if (output.size() < size())
            output.resize(size());

        switch (policy) {
#if __cplusplus >= 201703L || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
            case ExecutionPolicy::seq:
                std::transform(std::execution::seq, cbegin(), cend(), output.begin(), op);
                break;

            case ExecutionPolicy::par:
                std::transform(std::execution::par, cbegin(), cend(), output.begin(), op);
                break;

            case ExecutionPolicy::par_unseq:
                std::transform(std::execution::par_unseq, cbegin(), cend(), output.begin(), op);
                break;

    #if __cplusplus >= 202002L || (defined(_MSVC_LANG) && _MSVC_LANG >= 202002L)
            case ExecutionPolicy::unseq:
                std::transform(std::execution::unseq, cbegin(), cend(), output.begin(), op);
                break;
    #endif

            default:
                std::transform(cbegin(), cend(), output.begin(), op);
                break;
#endif
        }
        return *this;
    }

    // Accumulate
    //---------------------------------
    // The order of operations are left to right
    template <class U, class BinaryOperation>
    constexpr U accumulate(U init, BinaryOperation op) const {
        return std::accumulate(cbegin(), cend(), init, op);
    }


    // Reduce / transform_reduce
    //---------------------------------
    // The order of operations is not guaranteed
    // Note: Requires that the BinaryOperation satisfy the commutative: "A op B == B op A" and
    // associative: "(A op B) op C == A op (B op C)" properties
    template <class U, class BinaryOperation>
    constexpr U reduce(U init, BinaryOperation op) const {
#if __cplusplus >= 201703L || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
        return std::reduce(cbegin(), cend(), init, op);
#else
        return std::accumulate(cbegin(), cend(), init, op);
#endif
    }

#if __cplusplus >= 201703L || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
    template <class U, class BinaryReductionOp, class UnaryTransformOp>
    constexpr U transform_reduce(U init, BinaryReductionOp reduce, UnaryTransformOp transform) const {
        return std::transform_reduce(cbegin(), cend(), init, reduce, transform);
    }
#else
    template<class U, typename BinaryReductionOp, typename UnaryTransformOp>
    auto transform_reduce(U init, BinaryReductionOp reduce, UnaryTransformOp transform) {
        return std::accumulate(cbegin(), cend(), init, [&](const auto& acc, const auto& val) {
            return reduce(acc, transform(val));
        });
    }
#endif

    // The order of operations is not guaranteed
    // Execution policy must be ne of:
    //   - std::execution::seq: execution may not be parallelized.
    //   - std::execution::par: execution may be parallelized.
    //   - std::execution::par_unseq: execution may be parallelized, vectorized, or migrated across threads.
    //   - std::execution::unseq: execution may be vectorized. [C++20]
    template <class U, class BinaryOperation>
    constexpr U reduce(ExecutionPolicy policy, U init, BinaryOperation op) const {
#if __cplusplus >= 201703L || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
        switch (policy) {
            case ExecutionPolicy::seq:
                return std::reduce(std::execution::seq, cbegin(), cend(), init, op);

            case ExecutionPolicy::par:
                return std::reduce(std::execution::par, cbegin(), cend(), init, op);

            case ExecutionPolicy::par_unseq:
                return std::reduce(std::execution::par_unseq, cbegin(), cend(), init, op);

    #if __cplusplus >= 202002L || (defined(_MSVC_LANG) && _MSVC_LANG >= 202002L)
            case ExecutionPolicy::unseq:
                return std::reduce(std::execution::unseq, cbegin(), cend(), init, op);
    #endif
            default:
                return std::reduce(cbegin(), cend(), init, op);
        }
#else
        return std::accumulate(cbegin(), cend(), init, op);
#endif

        return {};
    }

    template <class U, class BinaryReductionOp, class UnaryTransformOp>
    constexpr U transform_reduce(ExecutionPolicy policy, U init, BinaryReductionOp reduce, UnaryTransformOp transform) const {
#if __cplusplus >= 201703L || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
        switch (policy) {
            case ExecutionPolicy::seq:
                return std::transform_reduce(std::execution::seq, cbegin(), cend(), init, reduce, transform);

            case ExecutionPolicy::par:
                return std::transform_reduce(std::execution::par, cbegin(), cend(), init, reduce, transform);

            case ExecutionPolicy::par_unseq:
                return std::transform_reduce(std::execution::par_unseq, cbegin(), cend(), init, reduce, transform);

    #if __cplusplus >= 202002L || (defined(_MSVC_LANG) && _MSVC_LANG >= 202002L)
            case ExecutionPolicy::unseq:
                return std::transform_reduce(std::execution::unseq, cbegin(), cend(), init, reduce, transform);
    #endif
            default:
                return std::transform_reduce(cbegin(), cend(), init, reduce, transform);
        }
#else
        return std::accumulate(cbegin(), cend(), init, [&](const auto &acc, const auto &val) {
            return reduce(acc, transform(val));
        });
#endif
    }

protected:
    constexpr iterator       correct(ptrdiff idx)                           { return (idx >= 0) ?  begin() + idx :  end() + idx + 1; }
    constexpr const_iterator correct(ptrdiff idx) const                     { return (idx >= 0) ? cbegin() + idx : cend() + idx + 1; }
    constexpr ptrdiff        correctIdx(ptrdiff idx) const                  { return (idx >= 0) ? idx : size() + idx + 1; }

    constexpr iterator       correctInside(ptrdiff idx)                     { return (idx >= 0) ?  begin() + idx :  end() + idx; }
    constexpr const_iterator correctInside(ptrdiff idx) const               { return (idx >= 0) ? cbegin() + idx : cend() + idx; }
    constexpr ptrdiff        correctInsideIdx(ptrdiff idx) const            { return (idx >= 0) ? idx : size() + idx; }

    // Avoid duplicate some functions
    constexpr tvector &      unconst() const                                { return *const_cast<tvector *>(this); }
};

//-------------------------------------
template <class T, class Allocator = std::allocator<T>>
constexpr TVector<T, Allocator> &
asTVector(std::vector<T, Allocator> &vec) {
    //return *reinterpret_cast<TVector<T, Allocator> *>(&vec);
    return static_cast<TVector<T, Allocator> &>(vec);
}

//-------------------------------------
template <class T, class Allocator = std::allocator<T>>
constexpr const TVector<T, Allocator> &
asTVector(const std::vector<T, Allocator> &vec) {
    //return *reinterpret_cast<const TVector<T, Allocator> *>(&vec);
    return static_cast<const TVector<T, Allocator> &>(vec);
}

} // end of namespace
