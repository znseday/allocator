#define BOOST_TEST_MODULE ip_filter_test_module

#include "my_aux.h"

#include <boost/test/unit_test.hpp>

using namespace std;

BOOST_AUTO_TEST_SUITE(allocator_test_suite)

BOOST_AUTO_TEST_CASE(allocator_test_Factorial)
{
	for (int i = 0; i < 10; i++)
		cout << MyFactorial(i) << "  ";
	cout << endl;
	//BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(allocator_test_Fibonacci)
{
	for (int i = 0; i < 10; i++)
		cout << MyFibonacci(i) << "  ";
	cout << endl;
	//BOOST_REQUIRE_EQUAL(TestString, test);
}

BOOST_AUTO_TEST_CASE(allocator_test_MyList)
{
	MyListClass<hard> MyList;
	for (int i = 0; i < 10; i++)
		MyList.Add(hard(i, MyFactorial(i)));

	MyList.PrepareToRead();
	while(1)
	{
		hard t;
		if (MyList.GetNext(t))
			cout << t << endl;
		else
			break;
	}

	//BOOST_REQUIRE_NE(TestString, test);
}

BOOST_AUTO_TEST_SUITE_END()
