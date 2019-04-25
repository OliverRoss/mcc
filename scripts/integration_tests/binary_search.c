#include "mc_builtins.c"
#include <stdbool.h>
typedef const char *string;
bool binary_search(int arr[20], int val)
{
	int i;
	i = 0;

	while (i < 20) {

		if (arr[i] == val) {
			return true;
		}
		i = i + 1;
	}

	return false;
}

int main()
{
	int arr[20];

	int i;
	i = 0;

	while (i < 20) {
		arr[i] = i;
		i = i + 1;
	}

	print("Please enter a number to search for: ");

	int n;
	n = read_int();
	print_nl();

	if (binary_search(arr, n)) {
		print("Value was found!");
	} else {
		print("Value was not found!");
	}
	print_nl();

	return 0;
}
