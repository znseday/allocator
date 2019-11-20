#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <iterator>
#include <algorithm>
#include <utility>
#include <memory>

#include "my_alloc.h"

// __FUNCSIG__ is for VS, but Qt (mingw) works with __PRETTY_FUNCTION__
#if ((defined WIN32) || (defined WIN64)) && (defined _MSC_VER)
//#define MY_P_FUNC __FUNCSIG__
#else
#define MY_P_FUNC __PRETTY_FUNCTION__
#endif

#if (defined NDEBUG)
#define MY_DEBUG_ONLY(x)
#else
#define MY_DEBUG_ONLY(x) (x)
#endif


struct hard
{
	int fa;
	int fi;
    hard(int _fa, int _fi) : fa(_fa), fi(_fi)
    {
        MY_DEBUG_ONLY(std::cout << MY_P_FUNC << std::endl);
    }

	//hard() = default;
    //hard(const hard &) = default;     // It's needed for ActualAlloc.construct(&pHead->data, ob.pHead->data);
    //hard(hard &&) noexcept = default; // for std::vector

    hard() : fa(0), fi(0) // for temp object
    {
        MY_DEBUG_ONLY(std::cout << MY_P_FUNC << std::endl);
    }

    hard & operator=(const hard &ob) = default; // for MyListClass

    //hard(const hard &ob) : fa(ob.fa), fi(ob.fi) // It's needed for ActualAlloc.construct(&pHead->data, ob.pHead->data);
    //{
        //MY_DEBUG_ONLY(std::cout << MY_P_FUNC << std::endl);
    //}

//    hard(hard &&ob) noexcept : fa(ob.fa), fi(ob.fi) // for std::vector
//    {
//        //MY_DEBUG_ONLY(std::cout << MY_P_FUNC << std::endl);
//    }

    ~hard()
    {
        MY_DEBUG_ONLY(std::cout << MY_P_FUNC << std::endl);
    }

    hard(hard &&) noexcept = delete;
    hard(const hard &) = delete;

};
std::ostream& operator<<(std::ostream& s, const hard& ob);

//---------------------------------------------------------------------------

int MyFactorial(int n);    // I'd use unsigned but "hard" requires int anyway
int MyFibonacci(int n);    // I'd use unsigned but "hard" requires int anyway

//---------------------------------------------------------------------------


template <typename T>
struct MyNodeStruct
{
	T data;
	MyNodeStruct<T> *next;
};

template <typename T>
class MyIterator
{
private:
    MyNodeStruct<T> *pCur = nullptr;
public:
    MyIterator() = default;
    MyIterator(MyNodeStruct<T> *p) : pCur(p) {}
    bool operator!=(MyIterator it) const      // or (const MyIterator &it) as usual ???
    {
        return pCur != it.pCur;
    }
    MyIterator operator++()  // or MyIterator & operator++() ?
    {
        pCur = pCur->next;
        return *this;
    }
    T & operator*() {return pCur->data;}
    //const T & operator*() const  {return pCur->data;}
};

template <typename T, typename Allocator = std::allocator<T> >
class MyListClass
{
private:
    MyNodeStruct<T> *pHead = nullptr;
    MyNodeStruct<T> *pCur  = nullptr;

    // It's still unclear a bit
    typename Allocator::template rebind<MyNodeStruct<T>>::other ActualAlloc;

public:

    //template <typename ObAlloc>
    //friend MyListClass<T,ObAlloc>; // Why doesn't it work?

    MyListClass();
    MyListClass(const MyListClass &ob);

    MyListClass(MyListClass &&ob) noexcept;

    template <typename ObAlloc>
    MyListClass(const MyListClass<T,ObAlloc> &ob);

    template <typename ObAlloc>
    MyListClass(MyListClass<T,ObAlloc> &&ob);

	~MyListClass();

	void Add(T _data);

    template <typename ...Args>
    void Emplace(Args ...args);

	void PrepareToRead();
	bool GetNext(T &_data);

    MyIterator<T> begin() {return MyIterator<T>(pHead);}
    MyIterator<T> end()   {return MyIterator<T>();}

    MyNodeStruct<T>* GetHead() const {return pHead;}
    void HeadToNull() {pHead = nullptr;}
};


template <typename T, typename Allocator /*= std::allocator<T>*/ >
MyListClass<T,Allocator>::MyListClass(const MyListClass<T,Allocator> &ob)
{
    if (!ob.pHead)
        return;

    pHead = ActualAlloc.allocate(1);
    ActualAlloc.construct(&pHead->data, ob.pHead->data);
    pHead->next = nullptr;
    pCur = pHead;

    MyNodeStruct<T> *newNode;
    MyNodeStruct<T> *pCurOb = ob.pHead->next;
    while (pCurOb)
    {
        newNode = ActualAlloc.allocate(1);
        ActualAlloc.construct(&newNode->data, pCurOb->data);
        newNode->next = nullptr;

        pCur->next = newNode;
        pCur = pCur->next;

        pCurOb = pCurOb->next;
    }
}

template <typename T, typename Allocator /*= std::allocator<T>*/ >
template <typename ObAlloc>
MyListClass<T,Allocator>::MyListClass(const MyListClass<T,ObAlloc> &ob)
{
    if (!ob.GetHead())
        return;

    pHead = ActualAlloc.allocate(1);
    ActualAlloc.construct(&pHead->data, ob.GetHead()->data);
    pHead->next = nullptr;
    pCur = pHead;

    MyNodeStruct<T> *newNode;
    MyNodeStruct<T> *pCurOb = ob.GetHead()->next;

    while (pCurOb)
    {
        newNode = ActualAlloc.allocate(1);
        ActualAlloc.construct(&newNode->data, pCurOb->data);
        newNode->next = nullptr;

        pCur->next = newNode;
        pCur = pCur->next;

        pCurOb = pCurOb->next;
    }
}

template <typename T, typename Allocator /*= std::allocator<T>*/ >
template <typename ObAlloc>
MyListClass<T,Allocator>::MyListClass(MyListClass<T,ObAlloc> &&ob)
{
    if (!ob.GetHead())
        return;

    pHead = ActualAlloc.allocate(1);
    ActualAlloc.construct(&pHead->data, std::move(ob.GetHead()->data));
    pHead->next = nullptr;
    pCur = pHead;

    MyNodeStruct<T> *newNode;
    MyNodeStruct<T> *pCurOb = ob.GetHead()->next;

    while (pCurOb)
    {
        newNode = ActualAlloc.allocate(1);
        ActualAlloc.construct(&newNode->data, std::move(pCurOb->data));
        newNode->next = nullptr;

        pCur->next = newNode;
        pCur = pCur->next;

        pCurOb = pCurOb->next;
    }

    ob.HeadToNull();
}

template <typename T, typename Allocator /*= std::allocator<T>*/ >
MyListClass<T,Allocator>::MyListClass(MyListClass<T,Allocator> &&ob) noexcept
    : pHead(ob.pHead), ActualAlloc(std::move(ob.ActualAlloc))
{
    ob.pHead = nullptr;
}

template <typename T, typename Allocator>
MyListClass<T,Allocator>::MyListClass()
{
    MY_DEBUG_ONLY(std::cout << MY_P_FUNC << std::endl);
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
        ActualAlloc.destroy(&temp->data); // temp or data? I think 'data' cos we constructed 'data' before
        ActualAlloc.deallocate(temp, 1);  // temp or data? I think 'temp' cos we allocated MyNodeStruct<T>
	}
}

template <typename T, typename Allocator>
void MyListClass<T,Allocator>::Add(T _data)
{
    MyNodeStruct<T> *temp;
    MyNodeStruct<T> *newNode = ActualAlloc.allocate(1);
    ActualAlloc.construct(&newNode->data, std::forward<hard>(_data));
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


template <typename T, typename Allocator  /*= std::allocator<T>*/ >
template <typename ...Args>
void MyListClass<T,Allocator>::Emplace(Args ...args)
{
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


