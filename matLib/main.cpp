#include <assert.h>
#include "mat.h"

using namespace std;

template <typename T>
void test(int x, int y)
{
	auto zero = mat<T>::zero(x, x);
	auto ident = mat<T>::identity(x);

	assert(zero == mat<T>::filled(x, x, 0));
	assert(ident == mat<T>::diagonal(x, 1));

	assert(zero.width() == x);
	assert(zero.height() == x);
	assert(ident.width() == x);
	assert(ident.height() == x);

	assert(ident * zero == zero);
	assert(zero * zero == zero);
	assert(ident + zero == ident);
	assert(ident - zero == ident);

	assert((zero *= ident) == zero);
	assert((zero *= zero) == zero);
	assert((ident += zero) == ident);
	assert((ident -= zero) == ident);

	assert(ident.transposition() == ident);

	mat<T> m1(x, y), m2(y, x);

	assert(m1.height() == x);
	assert(m1.width() == y);
	assert(m2.height() == y);
	assert(m2.width() == x);

	for (int i = 0; i < x; i++)
	{
		for (int j = 0; j < y; j++)
		{
			T val = (i * y) + j + 1;
			m1.set(i, j, val);
			m2.set(j, i, m1.get(i, j));
		}
	}

	assert(m1.transposition() == m2);
	assert(m1 == m2.transposition());

	auto m1m2 = m1 * m2;
	auto m2m1(m2 * m1);

	assert(m1m2.height() == m1.height());
	assert(m1m2.width() == m2.width());
	assert(m2m1.height() == m2.height());
	assert(m2m1.width() == m1.width());

	assert(m1m2 != m2m1);

	assert(m1m2 * ident == m1m2);
	assert(m1m2 * zero == zero);

	assert(m1m2.transposition().transposition() == m1m2);
}

int main()
{
	test<int>(1, 2);
	test<int>(2, 3);
	test<int>(3, 4);
	test<int>(3, 5);
	test<int>(3, 6);
	test<int>(8, 8);

	test<long long>(1, 2);
	test<long long>(2, 3);
	test<long long>(3, 4);
	test<long long>(3, 5);
	test<long long>(3, 6);
	test<long long>(8, 8);

	test<float>(1, 2);
	test<float>(2, 3);
	test<float>(3, 4);
	test<float>(3, 5);
	test<float>(3, 6);
	test<float>(8, 8);

	test<double>(1, 2);
	test<double>(2, 3);
	test<double>(3, 4);
	test<double>(3, 5);
	test<double>(3, 6);
	test<double>(8, 8);

	return 0;
}