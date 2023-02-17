# An implementation for a function cache

## Task description

This project presents one possible solution to the following problem:

Suppose there exists a pure function
    struct Background;
    struct State;
    template<typename In>
      auto f (State&, In&&, std::shared_ptr<Background const>);
So `f` modifies a state based on some input and some background
knowledge.
The customer calls `f` multiple times with the same parameters for
`Background`, `State` and `In`. The running time of `f` is quite high,
so the customer got the idea to implement a "cache" within the
`Background`. The goal of the cache is to speed up calls to `f` by
re-using results that have been computed in earlier calls to `f`.
Please describe how you would approach that task. In your solution:
- What changes to the interface are required?
- What requirements on the types `Background`, `State` and `In` are implied?
Prefer to formulate your answer in terms of code.
Certainly the customer will ask for resource consumption (time,
memory) of the cached version in comparision with the unchached
version. Please also take into account that the background knowledge
exists for a long time and across multiple sessions and must not
accumulate memory indefinitely.

## Solution

### Overview

The core idea is to use a map, that maps the input arguments to the computed solution. This reduces the complexity to be logarithmic in the amount of cached function calls, in the case where the function call was cached.
This is implemented by the class `Function_cache`.

### Details

#### Required changes to the interface

With this implementation no changes to the existing interface are necessary. This enables an update without the need for the customer to adapt the code that has already been written.

Note that this implementation does not take up the customers' idea to implement the cache within the `Background`. This decision was made based on the following:
* The `Background` is part of the function call, implementing the cache within it would entail the storing of the cache itself for every cached function call. This would increase the memory usage drastically.
* The functionality of a function cache can easily be encapsulated. This improves code maintainability and allowes it to be used for other functions as well.
* It enables the unchanged interface.

The changes to the existing code are all within the function `f`:
* The costly computation was moved into a local lambda, which translates the pure function into a returning function. This makes the handling of the cache easier since the `state` is input and output. Both values has to be stored separately.
* The type of `In` is converted to `std::any`. This ensures that the type of the map keys are the same.
* The cache object is a static object within `f`.

See bin/function_cache.cpp


#### Implied requirements on `Background`, `State` and `In`

Implied requirements for these types can be deduced from the usage:

* The are used as a key in a `std::map`, so they have to be:
  * copyable
  * assignable
  * comparable
  * `In` has to be convertible into a `std::any` object.

* The cache is stored to memory, so they have to be:
  * serializable


#### Performance and Memory usage

If the function call was cached the complexity is logarithmic in the amount of cached entries. One entry consists of the input and output parameter and two map iterators. Otherwise it is in the same complexity class as the function `f` on its own.

The memory usage for the cache amounts to the following:

`cache_.size() * (2*sizeof(State) + sizeof(In) + sizeof(Background)) + 2*sizeof(Cache_t::iterator)`

The allowed data usage can be adapted by setting the constant `FC_MAX_CACHE_SIZE` at compile time.

The location where the persistent cache file is stored can be set by `FC_CACHE_FILEPATH` at compile time.

#### Persistence

Since the cache should be used across multiple sessions, the `Function_cache` loads an existing cache file from disk when constructed. The destructor updates the same file with the updated chache.

#### Data usage limitation

To prevent unlimited memory usage the amount of cached function calls is limited by a specified amount. Each time a new function call is cashed, the size of the map is checked and if it exceeds the specified maximal amount, the oldest cashed entry is removed from the map. See `Function_cache::check_cache_size()`

#### Maximizing cashe hit chances

In order to maximize the chance of a cache hit the cache is updated with every call to the function. It is ordered from most frequent function call to the function call that was called the longest time ago. This is implemented in the function `Function_cache::make_newest_entry`. Start and end are saved as member variables `Function_cache::newest_cache_entry_` & `Function_cache::newest_cache_entry_`). To facilitate the correct order of all cached entries each of these points to its immediate predecessor and successor. See `Function_cache::Cache_entry::prev_` & `Function_cache::Cache_entry::next_`. This adds the functionality of a doubly linked list to the map.

This seemed to be an acceptable increase in memory usage and complexity, since it does not change the complexity class of the function call (it is still O(ln)).

### What has been left out

Some things has been left out, that should be part of a complete project, since it seems they are not at the core of the Task:
* The reading and writing functions are left as placeholders.
* This project does not contain any tests.
* Proper exception handling was left out.
