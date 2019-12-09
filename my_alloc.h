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
#define MY_P_FUNC __FUNCSIG__
#else
#define MY_P_FUNC __PRETTY_FUNCTION__
#endif

#if (defined NDEBUG)
#define MY_DEBUG_ONLY(x)
#else
#define MY_DEBUG_ONLY(x) (x)
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

    //MyAllocatorClass(const MyAllocatorClass &) = default;
    MyAllocatorClass(const MyAllocatorClass &) = delete;

    MyAllocatorClass(MyAllocatorClass &&ob) noexcept
    {
        MY_DEBUG_ONLY(std::cout << MY_P_FUNC << std::endl);
        pMemory = ob.pMemory;
        takens = ob.takens;
        ob.pMemory = nullptr;
        ob.takens = 0;
    }

    template<typename U>
    MyAllocatorClass(const MyAllocatorClass<U,BLOCKS> &)  // ???  Added for C++Builder
    {
        std::cout << MY_P_FUNC << " (Added for C++Builder)" << std::endl; // never called
    }

    T* allocate( /*[[maybe_unused]]*/ std::size_t n )
    {
        MY_DEBUG_ONLY(std::cout << MY_P_FUNC << "[n = " << n << "]" << std::endl);

        //if (n != 1)              // Not really necessary for plain allocator
        //    throw std::bad_alloc();

        T *res = pMemory+takens;
        takens += n;
        if (takens <= BLOCKS)
            return res;
        else
        {
            throw std::bad_alloc();
            //return nullptr;
        }
    }

    void deallocate( [[maybe_unused]] T *p, [[maybe_unused]] std::size_t n ) const
    {
        MY_DEBUG_ONLY(std::cout << MY_P_FUNC << "[n = " << n << "]" << std::endl);
        //std::free(p); // not necessary cos we dealloacate whole memory at once
    }

    template<typename U, typename ...Args>
    void construct(U *p, Args&& ...args) const
    {
        MY_DEBUG_ONLY(std::cout << MY_P_FUNC << std::endl);
        new(p) U(std::forward<Args>(args)...);
    }

    template<typename U>
    void destroy(U *p) const
    {
        MY_DEBUG_ONLY(std::cout << MY_P_FUNC << std::endl);
        p->~U();
    }

    MyAllocatorClass()
    {
        MY_DEBUG_ONLY(std::cout << MY_P_FUNC << std::endl);
        pMemory = (T*)std::malloc(BLOCKS * sizeof(T));
    }

    ~MyAllocatorClass()
    {
        MY_DEBUG_ONLY(std::cout << MY_P_FUNC << std::endl);
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

    //MyAllocatorProClass(const MyAllocatorProClass &) = default;
    MyAllocatorProClass(const MyAllocatorProClass &) = delete;

    template<typename U>
    MyAllocatorProClass(const MyAllocatorProClass<U,BLOCKS> &)// ??? Added for C++Builder
    {
        std::cout << MY_P_FUNC << " (Added for C++Builder)" << std::endl; // never called
    }

    MyAllocatorProClass(MyAllocatorProClass &&ob) noexcept
    {
        MY_DEBUG_ONLY(std::cout << MY_P_FUNC << std::endl);
        ps = std::move(ob.ps); // is that ok?
        takens = ob.takens;
        ob.takens = 0;
    }

    MyAllocatorProClass()
    {
        MY_DEBUG_ONLY(std::cout << MY_P_FUNC << std::endl);
        T *pMemory = (T*)std::malloc(BLOCKS * sizeof(T));
        ps.push_back(pMemory);
    }

    ~MyAllocatorProClass()
    {
        MY_DEBUG_ONLY(std::cout << MY_P_FUNC << std::endl);
        for (auto p : ps)
            std::free(p);

        //std::cout << "size = " << ps.size() << std::endl; // for debugging

        //for (auto it = ps.cbegin(); it != ps.cend(); ++it)
        //    std::free((void*)(*it));                      // for debugging
    }

    T* allocate( /*[[maybe_unused]]*/ std::size_t n)
    {
        MY_DEBUG_ONLY(std::cout << MY_P_FUNC << "[n = " << n << "]" << std::endl);

        if (n != 1)
            throw std::bad_alloc();

        unsigned cur_b = takens / BLOCKS;
        unsigned cur_i = takens % BLOCKS;

//        unsigned future_b = (takens+n-1) / BLOCKS; // if n == 1 then cur_b == future_b
//        while (future_b >= ps.size())
//        {
//            T *pMemory = (T*)std::malloc(BLOCKS * sizeof(T));
//            ps.push_back(pMemory);
//        }

        if (cur_b == ps.size())
        {
            T *pMemory = (T*)std::malloc(BLOCKS * sizeof(T));
            ps.push_back(pMemory);
        }

        T *res = ps.back() + cur_i;
        //T *res = ps[cur_b] + cur_i;

//        std::cout << "res = " << res << std::endl;        // for debugging

        takens++; // (n == 1)
        //takens += n;

//        std::cout << "size = " << ps.size() << std::endl; // for debugging
        return res;
    }

    void deallocate( [[maybe_unused]]T *p, [[maybe_unused]] std::size_t n ) const
    {
        MY_DEBUG_ONLY(std::cout << MY_P_FUNC << "[n = " << n << "]" << std::endl);
        //std::free(p); // not necessary cos we dealloacate whole memory at once
    }

    template<typename U, typename ...Args>
    void construct(U *p, Args&& ...args) const
    {
        MY_DEBUG_ONLY(std::cout << MY_P_FUNC << std::endl);
        new(p) U(std::forward<Args>(args)...);
    }

    template<typename U>
    void destroy(U *p) const
    //void destroy(T *p) const
    {
        MY_DEBUG_ONLY(std::cout << MY_P_FUNC << std::endl);
        p->~U();
    }

};
