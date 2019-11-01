#include <iostream>

#include "my_aux.h"

using namespace std;

ostream& operator<<(ostream& s, const hard& ob)
{
	s << ob.fa << " " << ob.fi;
	return s;
}

int MyFactorial(int n)
{
	int res = 1;
    for (int i = 2; i <= n; i++)
		res *= i;
	return res;
}

int MyFibonacci(int n)
{
	int a = 0, b = 1;
	while (n > 0)
	{
		a += b;
		b = a - b;
		n--;
	}
	return a;
}
