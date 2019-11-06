#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <iterator>
#include <algorithm>
#include <utility>
#include <memory>

// __FUNCSIG__ is for VS, but Qt (mingw) works with __PRETTY_FUNCTION__
#if (defined WIN32) || (defined WIN64)
//#define MY_P_FUNC __FUNCSIG__
#define MY_P_FUNC __PRETTY_FUNCTION__
#else
#define MY_P_FUNC __PRETTY_FUNCTION__
#endif


// !!! I intentionally use "std::" because it's a header !!!

struct hard
{
	int fa;
	int fi;
    hard(int _fa, int _fi) : fa(_fa), fi(_fi)
    {
        //std::cout << MY_P_FUNC << std::endl;
    }

	//hard() = default;
    //hard(const hard &) = default;
    //hard(hard &&) = default;

    hard() : fa(0), fi(0) // for temp object
    {
        //std::cout << MY_P_FUNC << std::endl;
    }

    hard & operator=(const hard &ob) = default; // for MyListClass

//    hard(const hard &ob) : fa(ob.fa), fi(ob.fi)
//    {
//        //std::cout << MY_P_FUNC << std::endl;
//    }
//    hard(hard &&ob) noexcept : fa(ob.fa), fi(ob.fi)
//    {
//        //std::cout << MY_P_FUNC << std::endl;
//    }

    ~hard()
    {
        //std::cout << MY_P_FUNC << std::endl;
    }

    hard(hard &&) noexcept = delete;
    hard(const hard &) = delete;
    //hard(hard &&) = delete; // error: 'hard::hard(hard&&)' cannot be overloaded

};
std::ostream& operator<<(std::ostream& s, const hard& ob);

//---------------------------------------------------------------------------

int MyFactorial(int n);    // I'd use unsigned but "hard" requires int anyway
int MyFibonacci(int n);    // I'd use unsigned but "hard" requires int anyway

//---------------------------------------------------------------------------


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

    T* allocate(std::size_t n) // const // is it ok to remove 'const' ???
	{
        (void)n;
        //std::cout << MY_P_FUNC << "[n = " << n << "]" << std::endl;

        T *res = pMemory+takens;
        takens++; // to use it I had to remove 'const' above
        if (takens <= BLOCKS)
            return res;
        else
        {
            throw std::bad_alloc(); // is'it ok to throw it there???
            return nullptr;         // just in case
        }
	}

    void deallocate(T *p, std::size_t n) const
	{
        //std::cout << MY_P_FUNC << "[n = " << n << "]" << std::endl;
        //std::free(p); // not necessary cos we dealloacate whole memory at once
        (void)p;
        (void)n;
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

    MyAllocatorClass()
    {
        //std::cout << MY_P_FUNC << std::endl;
        pMemory = (T*)std::malloc(BLOCKS * sizeof(T));
    }

    ~MyAllocatorClass()
    {
//        std::cout << MY_P_FUNC << std::endl;
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
    //std::vector<void*> ps; // works, but what is better in this case: <void*> or <T*> ?

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

    MyAllocatorProClass()
    {
        //std::cout << MY_P_FUNC << std::endl;

        T *pMemory = (T*)std::malloc(BLOCKS * sizeof(T));
        //void *pMemory = (void*)std::malloc(BLOCKS * sizeof(T));
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

    T* allocate(std::size_t n) // const // is it ok to remove 'const' ???
    {
        (void)n;
        //std::cout << MY_P_FUNC << "[n = " << n << "]" << std::endl;

        unsigned cur_b = takens / BLOCKS;
        unsigned cur_i = takens % BLOCKS;

        if (cur_b == ps.size())
        {
            T *pMemory = (T*)std::malloc(BLOCKS * sizeof(T));
            //void *pMemory = (void*)std::malloc(BLOCKS * sizeof(T));
            ps.push_back(pMemory);
        }

        T *res = ps.back() + cur_i;
//        std::cout << "res = " << res << std::endl;        // for debugging

        takens++;

//        std::cout << "size = " << ps.size() << std::endl; // for debugging

        return res;
    }

    void deallocate(T *p, std::size_t n) const
    {
        //std::cout << MY_P_FUNC << "[n = " << n << "]" << std::endl;
        //std::free(p); // not necessary cos we dealloacate whole memory at once
        (void)p; (void)n;
    }

    template<typename U, typename ...Args>
    void construct(U *p, Args&& ...args) const
    {
        //std::cout << MY_P_FUNC << std::endl;
        new(p) U(std::forward<Args>(args)...);
    };

    template<typename U>
    void destroy(U *p) const // U or T ??????????
    //void destroy(T *p) const
    {
        //std::cout << MY_P_FUNC << std::endl;
        p->~U();
    }

};

//---------------------------------------------------------------------------

template <typename T>
struct MyNodeStruct
{
	T data;
	MyNodeStruct<T> *next;
};

template <typename T, typename Allocator = std::allocator<T> >
class MyListClass
{
private:
    MyNodeStruct<T> *pHead = nullptr;
    MyNodeStruct<T> *pCur  = nullptr;

    // I don't comprehend this 'rebind'
    typename Allocator::template rebind<MyNodeStruct<T>>::other ActualAlloc;

public:
	MyListClass();
	~MyListClass();

	void Add(T _data);

    template <typename ...Args>
    void Emplace(Args ...args)
    {
        // how to replace this code out of declaration?
        MyNodeStruct<T> *temp;
        MyNodeStruct<T> *newNode = ActualAlloc.allocate(1);
        ActualAlloc.construct(&newNode->data, std::forward<Args>(args)...);
        newNode->next = nullptr;

        if (!pHead)
            pHead = pCur = newNode;
        else
        {
            pCur = pHead;
            while (pCur)
            {
                temp = pCur;
                pCur = pCur->next;
            }
            temp->next = newNode;
        }

    }

	void PrepareToRead();
	bool GetNext(T &_data);
};

template <typename T, typename Allocator>
MyListClass<T,Allocator>::MyListClass()
{
    //std::cout << MY_P_FUNC << std::endl;
}

template <typename T, typename Allocator>
MyListClass<T,Allocator>::~MyListClass()
{
    //std::cout << MY_P_FUNC << std::endl;
	MyNodeStruct<T> *temp;
	pCur = pHead;
	while (pCur)
	{
		temp = pCur;
		pCur = pCur->next;
        //delete temp;
        ActualAlloc.destroy(temp);       // temp or data? What is better? 'temp' calls destructor of data anyway
        ActualAlloc.deallocate(temp, 1); // temp or data? I think 'temp' cos we allocated MyNodeStruct<T>
	}
}

template <typename T, typename Allocator>
void MyListClass<T,Allocator>::Add(T _data)
{
    MyNodeStruct<T> *temp;
    //MyNodeStruct<T>* newNode = new MyNodeStruct<T>; // into construct
    MyNodeStruct<T> *newNode = ActualAlloc.allocate(1);
    ActualAlloc.construct(&newNode->data, std::forward<hard>(_data));
    //newNode->data = _data; // into construct
	newNode->next = nullptr;

	if (!pHead)
		pHead = pCur = newNode;
	else
	{
		pCur = pHead;
		while (pCur)
		{
			temp = pCur;
			pCur = pCur->next;
		}
		temp->next = newNode;
	}
}

// How to write it correct?
//template <typename T, typename Allocator, typename ...Args>
//void MyListClass<T,Allocator>::Emplace(Args ...args)
//{
//}

template <typename T, typename Allocator>
void MyListClass<T,Allocator>::PrepareToRead()
{
	pCur = pHead;
}

template <typename T, typename Allocator>
bool MyListClass<T,Allocator>::GetNext(T& _data)
{
    MyNodeStruct<T> *temp;
	if (pCur)
	{
		temp = pCur;
		pCur = pCur->next;
		_data = temp->data;
		return true;
	}
	else
		return false;
}


