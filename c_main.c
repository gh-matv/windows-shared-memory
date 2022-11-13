#include <stdio.h>

#include "Windows.h" // For Sleep()
#include "c_shared_memory_header_only.h"

typedef struct A
{
	int a;
	int b;
} A;

int main()
{
	c_shared_memory* memory = c_shared_memory_create("test", sizeof(A));
	if (memory == NULL)
	{
		char buffer[1024];
		get_last_error_as_string(buffer, sizeof(buffer));
		printf("Failed to create shared memory: %s", buffer);
		return 1;
	}

	if (c_shared_memory_is_originator(memory))
	{
		A* data = (A*)c_shared_memory_get(memory);
		data->a = 1;
		data->b = 2;
		printf("Originator: Set data to %d, %d\n", data->a, data->b);
		printf("Waiting for accessor to set data to 3, 4...\n");
		while (data->a != 3 || data->b != 4) Sleep(100);
		printf("Originator: Accessor set data to %d, %d\n", data->a, data->b);
	}
	else
	{
		A* data = (A*)c_shared_memory_get(memory);
		printf("Accessor: Got data %d, %d\n", data->a, data->b);
		A new_data = {3, 4};
		*data = new_data; // other way to set data
		printf("Accessor: Set data to %d, %d\n", data->a, data->b);
	}

	c_shared_memory_destroy(memory);

	return 0;
}
