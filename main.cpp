// WSM.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>

#include "shared_memory.hpp"

struct test
{
	int a;
	int b;
};

int main()
{
	test x { 1,2 };
	
	// P1
	bound_shared_memory<test> mem("youpi", x);
	
	// P2
	bound_shared_memory<test> men("youpi");


	mem->a = 10;
	std::cout << men->a << std::endl;

}

