# windows shared memory

A simple and fast shared memory library for Windows.

### Usage
```cpp
int main_1()
{
	test x { 1,2 };
	
	// Program 1
    // Same as bsm<test> mem("youpi"); mem=x;
	bound_shared_memory<test> mem("youpi", x);
    mem->a = 10;
}

int main_2()
{
    // Program 2
	bound_shared_memory<test> men("youpi");	
	std::cout << men->a << std::endl;
}
```

There is no difference between "server" and "client", both can access and edit
