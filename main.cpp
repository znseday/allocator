#include <iostream>
#include <vector>
#include <map>
#include <iterator>
#include <algorithm>

#include "my_aux.h"

using namespace std;

static const int nBlocks = 10;
static const int N = nBlocks;        // One day it's gonna be N+1
static const int N_Pro = nBlocks+1;

void FirstTest();  // std::map    + std::allocator
void SecondTest(); // std::map    + MyAllocatorClass
void ThirdTest();  // MyListClass + std::allocator
void FourthTest(); // MyListClass + MyAllocatorClass


void SecondTestPro(); // std::map    + MyAllocatorProClass
void FourthTestPro(); // MyListClass + MyAllocatorProClass


int main()
{
#if (defined WIN32) || (defined WIN64)
	cout << "Homework allocator" << endl << endl; // for debugging
    cout << MY_P_FUNC << endl;                    // for debugging
#else
    // some
#endif

    FirstTest();  // std::map    + std::allocator
    SecondTest(); // std::map    + MyAllocatorClass

    ThirdTest();  // MyListClass + std::allocator
    FourthTest(); // MyListClass + MyAllocatorClass

    SecondTestPro(); // std::map    + MyAllocatorProClass
    FourthTestPro(); // MyListClass + MyAllocatorProClass

    return 0;
}

void FirstTest() // std::map + std::allocator
{
    cout << endl << "First:" << endl;
    map<int,hard> First;
    for (int i = 0; i < N; i++)
    {
        First.emplace(i, hard(MyFactorial(i), MyFibonacci(i)));

//        First.emplace(piecewise_construct, // isn't compiled! Why???
//                      forward_as_tuple(i),
//                      forward_as_tuple(MyFactorial(i), MyFibonacci(i)) );
        //First.try_emplace(i, MyFactorial(i), MyFibonacci(i)); // isn't compiled! Why???
    }

//	for (int i = 0; i < N; i++)          // Classic 'for', just to be sure
//		cout << i << " " << First[i] << endl;

    for (const auto & [i, v] : First)   // Trying to use a brand new 'for'
        cout << i << " " << v << endl;

    cout << "//----------------------" << endl;
}

void SecondTest() // std::map + MyAllocatorClass
{
    cout << endl << "Second:" << endl;   // std::map + MyAllocator

    auto Second = map<int, hard, less<int>, MyAllocatorClass<pair<const int, hard>, nBlocks > >{};

    for (int i = 0; i < N; i++)
    {
        Second.emplace(i, hard(MyFactorial(i), MyFibonacci(i)));
        //Second.try_emplace(i, MyFactorial(i), MyFibonacci(i)); // isn't compiled! Why???
    }

    for (const auto & [i, v] : Second)   // Trying to use a brand new 'for'
        cout << i << " " << v << endl;

    cout << "//----------------------" << endl;
}

void SecondTestPro() // std::map + MyAllocatorProClass
{
    cout << endl << "Second Pro:" << endl;   // std::map + MyAllocator

    auto Second = map<int, hard, less<int>, MyAllocatorProClass<pair<const int, hard>, nBlocks > >{};

    for (int i = 0; i < N_Pro; i++)
    {
        Second.emplace(i, hard(MyFactorial(i), MyFibonacci(i)));
        //Second.try_emplace(i, MyFactorial(i), MyFibonacci(i)); // isn't compiled! Why???
    }

    for (const auto & [i, v] : Second)
        cout << i << " " << v << endl;

    cout << "//----------------------" << endl;
}

void ThirdTest() // MyListClass + std::allocator
{
    cout << endl << "Third:" << endl;

    MyListClass<hard> Third;
    for (int i = 0; i < N; i++)
    {
        Third.Add(hard(MyFactorial(i), MyFibonacci(i)));
        //Third.Emplace()
    }
    Third.PrepareToRead();
    hard t;
    while( Third.GetNext(t) )
        cout << t << endl;

    cout << "//----------------------" << endl;
}


void FourthTest() // MyListClass + MyAllocatorClass
{
    cout << endl << "Fourth:" << endl;

    MyListClass<hard, MyAllocatorClass<pair<const int, hard>, nBlocks>> Fourth;
    for (int i = 0; i < N; i++)
    {
        Fourth.Add(hard(MyFactorial(i), MyFibonacci(i)));
        //Fourth.Emplace()
    }
    Fourth.PrepareToRead();
    hard t;
    while( Fourth.GetNext(t) )
        cout << t << endl;

    cout << "//----------------------" << endl;
}

void FourthTestPro() // MyListClass + MyAllocatorProClass
{
    cout << endl << "Fourth Pro:" << endl;

    MyListClass<hard, MyAllocatorProClass<pair<const int, hard>, nBlocks>> Fourth;
    for (int i = 0; i < N_Pro; i++)
    {
        Fourth.Add(hard(MyFactorial(i), MyFibonacci(i)));
        //Fourth.Emplace()
    }
    Fourth.PrepareToRead();
    hard t;
    while( Fourth.GetNext(t) )
        cout << t << endl;

    cout << "//----------------------" << endl;
}
