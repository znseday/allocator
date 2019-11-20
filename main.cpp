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

void VectorTest();    // std::vector + MyAllocatorClass
//void VectorCrashTestPro();  // std::vector + MyAllocatorProClass

void CopyTheSameConstructorsTest();
void CopyDifferConstructorsTest();

void MoveTheSameConstructorsTest();
void MoveDifferConstructorsTest();

int main()
{
#if (defined WIN32) || (defined WIN64)
	cout << "Homework allocator" << endl << endl; // for debugging
    cout << MY_P_FUNC << endl;                    // for debugging
#else
    // some
#endif

    MY_DEBUG_ONLY(std::cout << "Homework allocator (DEBUG)" << std::endl); // doesn't work ((( why?

    FirstTest();  // std::map    + std::allocator
    SecondTest(); // std::map    + MyAllocatorClass

    ThirdTest();  // MyListClass + std::allocator
    FourthTest(); // MyListClass + MyAllocatorClass

    SecondTestPro(); // std::map    + MyAllocatorProClass
    FourthTestPro(); // MyListClass + MyAllocatorProClass

    // It works only when we have hard::hard(const hard&)
    VectorTest();    // std::vector + MyAllocatorClass

    // Doesn't work. Causes Heap problems
    //VectorCrashTestPro();  // std::vector + MyAllocatorProClass

    CopyTheSameConstructorsTest();
    CopyDifferConstructorsTest();

    MoveTheSameConstructorsTest();
    MoveDifferConstructorsTest();

    return 0;
}

void FirstTest() // std::map + std::allocator
{
    cout << endl << "First (std::map + std::allocator):" << endl;
    map<int,hard> First;
    for (int i = 0; i < N; i++)
    {
//        First.emplace(piecewise_construct,
//                      forward_as_tuple(i),
//                      forward_as_tuple(MyFactorial(i), MyFibonacci(i)) );
        First.try_emplace(i, MyFactorial(i), MyFibonacci(i));
    }

    for (const auto & [i, v] : First)   // Trying to use a brand new 'for'
        cout << i << " " << v << endl;

    cout << "//----------------------" << endl;
}

void SecondTest() // std::map + MyAllocatorClass
{
    cout << endl << "Second (std::map + MyAllocatorClass):" << endl;

    auto Second = map<int, hard, less<int>, MyAllocatorClass<pair<const int, hard>, nBlocks > >{};

    for (int i = 0; i < N; i++)
        Second.try_emplace(i, MyFactorial(i), MyFibonacci(i));

    for (const auto & [i, v] : Second)   // Trying to use a brand new 'for'
        cout << i << " " << v << endl;

    cout << "//----------------------" << endl;
}

void SecondTestPro() // std::map + MyAllocatorProClass
{
    cout << endl << "Second Pro (std::map + MyAllocator):" << endl;

    auto Second = map<int, hard, less<int>, MyAllocatorProClass<pair<const int, hard>, nBlocks > >{};

    for (int i = 0; i < N_Pro; i++)
        Second.try_emplace(i, MyFactorial(i), MyFibonacci(i));

    for (const auto & [i, v] : Second)
        cout << i << " " << v << endl;

    cout << "//----------------------" << endl;
}

void ThirdTest() // MyListClass + std::allocator
{
    cout << endl << "Third (MyListClass + std::allocator):" << endl;

    MyListClass<hard> Third;
    for (int i = 0; i < N; i++)
    {
        //Third.Add(hard(MyFactorial(i), MyFibonacci(i)));
        Third.Emplace(MyFactorial(i), MyFibonacci(i));
    }

    for(auto it = Third.begin(); it != Third.end(); ++it)  // classic
        cout << *it << endl;

    cout << "//----------------------" << endl;
}


void FourthTest() // MyListClass + MyAllocatorClass
{
    cout << endl << "Fourth (MyListClass + MyAllocatorClass):" << endl;

    MyListClass<hard, MyAllocatorClass<hard, nBlocks>> Fourth;
    for (int i = 0; i < N; i++)
    {
        //Fourth.Add(hard(MyFactorial(i), MyFibonacci(i)));
        Fourth.Emplace(MyFactorial(i), MyFibonacci(i));
    }

    //Fourth.PrepareToRead();
    //for(hard t; Fourth.GetNext(t); cout << t << endl) {} // foor-loop ))) And iterators aren't needed)

    for (const auto &t : Fourth)  // modern
        cout << t << endl;

    cout << "//----------------------" << endl;
}

void FourthTestPro() // MyListClass + MyAllocatorProClass
{
    cout << endl << "Fourth Pro (MyListClass + MyAllocatorProClass):" << endl;

    MyListClass<hard, MyAllocatorProClass<hard, nBlocks>> Fourth;
    for (int i = 0; i < N_Pro; i++)
    {
        //Fourth.Add(hard(MyFactorial(i), MyFibonacci(i)));
        Fourth.Emplace(MyFactorial(i), MyFibonacci(i));
    }

    for (const auto &t : Fourth)  // modern
        cout << t << endl;

    cout << "//----------------------" << endl;
}

void VectorTest()  // std::vector + MyAllocatorClass
{
    cout << endl << "VectorTest (std::vector + MyAllocatorClass):" << endl;

    // if N == 10 them 31 = 1+2+4+8+16 ('n' for vectors)
    auto Vector = vector<int, MyAllocatorClass<int, 31> >{};

    for (int i = 0; i < N; i++)
        Vector.emplace_back(MyFactorial(i));

    for (const auto &v : Vector)
        cout << v << endl;

    cout << "//----------------------" << endl;
}

//void VectorCrashTestPro()  // std::vector + MyAllocatorProClass
//{
//    cout << endl << "CrashTestPro (std::vector + MyAllocatorProClass):" << endl;

//    auto Crash = vector<hard, MyAllocatorProClass<hard,nBlocks> >{};

//    for (int i = 0; i < N_Pro*100; i++)
//        Crash.emplace_back(MyFactorial(i), MyFibonacci(i) );

//    for (const auto &v : Crash)
//        cout << v << endl;

//    cout << "//----------------------" << endl;
//}

void CopyTheSameConstructorsTest()
{
    cout << endl << "CopyConstructorTest (allocators are the same):" << endl;

    MyListClass<int> SrcList;
    for (int i = 0; i < N; i++)
        SrcList.Emplace(MyFactorial(i));

    MyListClass<int> DstList = SrcList;

    for (const auto &t : DstList)
        cout << t << endl;

    cout << "//----------------------" << endl;
}

void CopyDifferConstructorsTest()
{
    cout << endl << "CopyConstructorTest (allocators are different):" << endl;

    MyListClass<int> SrcList;
    for (int i = 0; i < N; i++)
        SrcList.Emplace(MyFactorial(i));

    MyListClass<int, MyAllocatorClass<int, nBlocks>> DstList = SrcList;

    for (const auto &t : DstList)
        cout << t << endl;

    cout << "//----------------------" << endl;
}

void MoveTheSameConstructorsTest()
{
    cout << endl << "MoveConstructorTest (allocators are the same):" << endl;

    MyListClass<int> SrcList;
    for (int i = 0; i < N; i++)
        SrcList.Emplace(MyFactorial(i));

    MyListClass<int> DstList = std::move(SrcList);

    for (const auto &t : DstList)
        cout << t << endl;

    cout << "//----------------------" << endl;
}

void MoveDifferConstructorsTest()
{
    cout << endl << "MoveConstructorTest (allocators are different):" << endl;

    MyListClass<int> SrcList;
    for (int i = 0; i < N; i++)
        SrcList.Emplace(MyFactorial(i));

    MyListClass<int, MyAllocatorClass<int, nBlocks>> DstList = std::move(SrcList);

    for (const auto &t : DstList)
        cout << t << endl;

    cout << "//----------------------" << endl;
}


