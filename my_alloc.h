#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <iterator>
#include <algorithm>
#include <utility>
#include <memory>

// __FUNCSIG__ is for VS, but Qt (mingw) works with __PRETTY_FUNCTION__
#if ((defined WIN32) || (defined WIN64)) && (defined _MSC_VER)
//#define MY_P_FUNC __FUNCSIG__
#else
#define MY_P_FUNC __PRETTY_FUNCTION__
#endif


template <typename T, size_t BLOCKS>
class MyAllocatorClass
{
private:
    T *pMemory = nullptr;
    unsigned takens = 0;

public:
    using value_type = T;

    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;

    template<typename U>
    struct rebind
    {
        using other = MyAllocatorClass<U,BLOCKS>;
    };

    MyAllocatorClass(const MyAllocatorClass &) = default;

    template<typename U>
    MyAllocatorClass(const MyAllocatorClass<U,BLOCKS> &)  // ???  Added for C++Builder
    {
        //std::cout << MY_P_FUNC << " (Added for C++Builder)" << std::endl;
    }

    T* allocate( /*[[maybe_unused]]*/ std::size_t n )
    {
        //std::cout << MY_P_FUNC << "[n = " << n << "]" << std::endl;

        //if (n != 1)              // Can we solve the problem like this?
        //    throw std::bad_alloc();

        T *res = pMemory+takens;
        takens += n;
        if (takens <= BLOCKS)
            return res;
        else
        {
            throw std::bad_alloc();
            return nullptr;         // just in case. Is it needed???
        }
    }

    void deallocate( [[maybe_unused]] T *p, [[maybe_unused]] std::size_t n ) const
    {
        //std::cout << MY_P_FUNC << "[n = " << n << "]" << std::endl;
        //std::free(p); // not necessary cos we dealloacate whole memory at once
    }

    template<typename U, typename ...Args>
    void construct(U *p, Args&& ...args) const
    {
        //std::cout << MY_P_FUNC << std::endl;
        new(p) U(std::forward<Args>(args)...);
    }

    template<typename U>
    void destroy(U *p) const
    {
        //std::cout << MY_P_FUNC << std::endl;
        p->~U();
    }

    MyAllocatorClass()
    {
        //std::cout << MY_P_FUNC << std::endl;
        pMemory = (T*)std::malloc(BLOCKS * sizeof(T));
    }

    ~MyAllocatorClass()
    {
        //std::cout << MY_P_FUNC << std::endl;
        if (pMemory)
            std::free(pMemory);
    }
};
//---------------------------------------------------------------------------

template <typename T, size_t BLOCKS>
class MyAllocatorProClass
{
protected:
    //T *pMemory = nullptr;
    unsigned takens = 0;

    std::vector<T*> ps;

public:
    using value_type = T;

    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;

    template<typename U>
    struct rebind
    {
        using other = MyAllocatorProClass<U,BLOCKS>;
    };


    MyAllocatorProClass(const MyAllocatorProClass &) = default;

    template<typename U>
    MyAllocatorProClass(const MyAllocatorProClass<U,BLOCKS> &) {} // ???


    MyAllocatorProClass()
    {
        //std::cout << MY_P_FUNC << std::endl;
        T *pMemory = (T*)std::malloc(BLOCKS * sizeof(T));
        ps.push_back(pMemory);
    }

    ~MyAllocatorProClass()
    {
        //std::cout << MY_P_FUNC << std::endl;
        for (auto p : ps)
            std::free(p);

        //std::cout << "size = " << ps.size() << std::endl; // for debugging

        //for (auto it = ps.cbegin(); it != ps.cend(); ++it)
        //    std::free((void*)(*it));                      // for debugging
    }

    T* allocate( /*[[maybe_unused]]*/ std::size_t n)
    {
        //std::cout << MY_P_FUNC << "[n = " << n << "]" << std::endl;

        //if (n != 1)              // Can we solve the problem like this?
        //    throw std::bad_alloc();

        unsigned cur_b = takens / BLOCKS;
        unsigned cur_i = takens % BLOCKS;

        unsigned future_b = (takens+n-1) / BLOCKS; // if n == 1 then cur_b == future_b
        //size_t ps_size = ps.size();

        while (future_b >= ps.size())
        {
            T *pMemory = (T*)std::malloc(BLOCKS * sizeof(T));
            ps.push_back(pMemory);
        }

//        if (cur_b == ps.size()) // old version
//        {
//            T *pMemory = (T*)std::malloc(BLOCKS * sizeof(T));
//            ps.push_back(pMemory);
//        }

        //T *res = ps.back() + cur_i;  // old version
        T *res = ps[cur_b] + cur_i;

//        std::cout << "res = " << res << std::endl;        // for debugging

        //takens++; // old version
        takens += n;

//        std::cout << "size = " << ps.size() << std::endl; // for debugging
        return res;
    }

    void deallocate( [[maybe_unused]]T *p, [[maybe_unused]] std::size_t n ) const
    {
        //std::cout << MY_P_FUNC << "[n = " << n << "]" << std::endl;
        //std::free(p); // not necessary cos we dealloacate whole memory at once
    }

    template<typename U, typename ...Args>
    void construct(U *p, Args&& ...args) const
    {
        //std::cout << MY_P_FUNC << std::endl;
        new(p) U(std::forward<Args>(args)...);
    }

    template<typename U>
    void destroy(U *p) const
    //void destroy(T *p) const
    {
        //std::cout << MY_P_FUNC << std::endl;
        p->~U();
    }

};
