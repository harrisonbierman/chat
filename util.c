#include <stdio.h>

void add_item(int *arr, int item, int length) {
	for(int i = 0; i < length; i++) {
		if (arr[i] == 0) {
			arr[i] = item;
			break;
		} 
	}
}

void remove_item(int *arr, int item, int length) {
	for(int i = 0; i < length; i++) {
		if(arr[i] == item) {
			arr[i] = 0;
			break;
		}
	}
}

void print_array(int *arr, int length) {
	printf("{");
	for (int i = 0; i < length; i++) {
		printf("%d", *(arr + i));
		if (i < length - 1) printf(", ");
	}
	printf("}\n");
}

#define ARR_BUF 5

int main() {
	int array[ARR_BUF] = {0};
	printf("array size: %d\n", ARR_BUF);
	print_array(array, ARR_BUF);
	add_item(array, 7, ARR_BUF);
	add_item(array, 35, ARR_BUF);
	print_array(array, ARR_BUF);
	remove_item(array, 7, ARR_BUF);
	print_array(array, ARR_BUF);
	add_item(array, 71, ARR_BUF);
	add_item(array, 982, ARR_BUF);
	print_array(array, ARR_BUF);
}
