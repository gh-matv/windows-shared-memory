#include <iostream>

#include "SharedMemory.hpp"
#include "Windows.h" // For Sleep()

typedef struct A
{
	int a;
	int b;
} A;

int main()
{
	SharedMemory<A> memory("test");
	if (!memory.isValid())
	{
		std::cout << "Failed to create shared memory: " << getLastErrorAsString() << std::endl;
		return 1;
	}

	if (memory.isOriginator())
	{
		A* data = memory.get();
		data->a = 1;
		data->b = 2;
		printf("Originator: Set data to %d, %d\n", data->a, data->b);
		printf("Waiting for accessor to set data to 3, 4...\n");
		while (data->a != 3 || data->b != 4) Sleep(100);
		printf("Originator: Accessor set data to %d, %d\n", data->a, data->b);
	}
	else
	{
		A* data = memory.get();
		printf("Accessor: Got data %d, %d\n", data->a, data->b);
		A new_data = {3, 4};
		*data = new_data; // other way to set data
		printf("Accessor: Set data to %d, %d\n", data->a, data->b);
	}

	return 0;
}
