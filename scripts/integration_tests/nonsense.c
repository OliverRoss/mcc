#include <stdbool.h>
#include "mc_builtins.c"
typedef const char* string;
int returnTwo() {
	int value = 2;
	return value;
}

bool isOne(int in) {
	if (in == 1)
		return true;
	else
		return false;
}

int main() {
	int array[42];
	int arrayTwo[21];
	int i = 0;
	while (i < 21) {
		array[i] = i*i;
		i = i + 1;
	}
	i = 0;
	while (i < 21) {
		arrayTwo[i] = array[i];
		i = i + 1;
	}

	print_int(array[20]);
	print_nl();

	if (array[0] == 0) {
		print_int(array[0]);
		print_nl();
    }

	if (array[1] == 2) {
		print_int(0);
		print_nl();
	} else {
		print_int(array[1]);
		print_nl();
	}

	if (!isOne(2)) {
		print_int(returnTwo());
		print_nl();
	}

	int b;
	int a;
	b = 2 + 3;
	b = -b;
	print_int(b);
	print_nl();
	a = b;
	b = b * b;
	if (a == (b/a)) {
		print_int(b);
		print_nl();
	}
	return 0;
}