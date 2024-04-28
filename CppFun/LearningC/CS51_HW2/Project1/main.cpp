/*	Reverse the order of a n-element array (20pt)

		Example:

		input n: 5

		Please input these numbers:

		1 2 3 4 5

		Now, the sequence is:

		5 4 3 2 1

 

		#include <stdio.h>

		sort(int *p, int m)

		{

		//Write your code here

		}

		main(){

			int i, n;

			int *p, num[20];

			printf("input n:");

			scanf("%d", &n);

			printf("please input these numbers:\n");

			for (i = 0; i < n; i++)

				scanf("%d", &num[i]);

			p = &num[0];

			sort(p, n);

			printf("Now, the sequence is:\n");

			for (i = 0; i < n; i++)

				printf("%d ", num[i]);

		}

*/
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

//void sort(int* p, int m) {
//	if (m <= 1) {
//		return;
//	}
//
//	printf("Unsorted: ");
//	for (int i = 0; i < m; i++) {
//		if (i + 1 < m) {
//			printf("%d, ", *(p+i));
//		}
//		else {
//			printf("%d\n", *(p+i));
//		}
//	}
//
//	for (int i = 0; i < m; i++) {
//		
//		for (int j = i+1; j < m; j++) {
//			if (*(p + i) > *(p + j)) {
//				
//				int lower = *(p + j);
//				*(p + j) = *(p + i);
//				*(p + i) = lower;
//
//				printf("Partly Sorted: ");
//				for (int i = 0; i < m; i++) {
//					if (i + 1 < m) {
//						printf("%d, ", *(p + i));
//					}
//					else {
//						printf("%d\n", *(p + i));
//					}
//				}
//			}
//		}
//			
//	}
//	
//	printf("Sorted: ");
//	for (int i = 0; i < m; i++) {
//		if (i + 1 < m) {
//			printf("%d, ", *(p + i));
//		}
//		else {
//			printf("%d\n", *(p + i));
//		}
//	}
//}

/*
void sort(int* p, int m) {
	if (m <= 1) {
		return;
	}
	
	for (int i = 0; i < m / 2; i++)
	{
		int swap = *(p + m - 1 - i);
		*(p + m - 1 - i) = *(p + i);
		*(p + i) = swap;

	}
}

int main() {

	int i, n;

	int* p, num[20];

	printf("input n:");

	scanf("%d", &n);

	printf("please input these numbers:\n");

	for (i = 0; i < n; i++) {
		scanf("%d", &num[i]);
	}
	p = &num[0];

	sort(p, n);

	printf("Now, the sequence is:\n");

	for (i = 0; i < n; i++) {
		printf("%d ", num[i]);
	}
}
*/


/* Write a function named bitCount() in bitcount.c that returns 
	the number of 1 - bits in the binary representation of its 
	unsigned integer argument.
	For example, 59 = 0b011 1011. The number of 1 - bits of 59 is 5.
	Given code: 
		  #include <stdio.h>

		  int bitCount (unsigned int n);

		  int main ( )
		  {
			printf ("# 1-bits in base 2 representation of %u = %d, should be 0\n",
			  0, bitCount (0));
			printf ("# 1-bits in base 2 representation of %u = %d, should be 1\n",
			  1, bitCount (1));
			printf ("# 1-bits in base 2 representation of %u = %d, should be 16\n",
			  2863311530u, bitCount (2863311530u));
			printf ("# 1-bits in base 2 representation of %u = %d, should be 1\n",
			  536870912, bitCount (536870912));
			printf ("# 1-bits in base 2 representation of %u = %d, should be 32\n",
			  4294967295u, bitCount (4294967295u));
			return 0;
		  }

		  int bitCount (unsigned int n)
		  {
			/* your code here */
/*		  }
*/
/*
#define CRT_SECURE_NO_WARNINGS
#include <stdio.h>

int bitCount(unsigned int n);

int main()
{
	printf("# 1-bits in base 2 representation of %u = %d, should be 0\n",
		0, bitCount(0));
	printf("# 1-bits in base 2 representation of %u = %d, should be 1\n",
		1, bitCount(1));
	printf("# 1-bits in base 2 representation of %u = %d, should be 16\n",
		2863311530u, bitCount(2863311530u));
	printf("# 1-bits in base 2 representation of %u = %d, should be 1\n",
		536870912, bitCount(536870912));
	printf("# 1-bits in base 2 representation of %u = %d, should be 32\n",
		4294967295u, bitCount(4294967295u));
	return 0;
}

int bitCount(unsigned int n)
{
	unsigned int ones = 0;
	while (n > 0)
	{
		if ((n % 2) == 1)
		{
			ones++;
		}
		n = n / 2;
	}
	return ones;
}
*/

/* Lab 02 Part 0
	Submit your Hello World C source code in text. 
	You can copy your source code in the textbox and submit.
*/
/*
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
int main()
{
	printf("Hello World!");
}
*/


/*	HW2 - write function that returns length of a string. 
		Use given function:
			int length(char *p) {}
		code inside main is also given
*/

/*
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

int length(char* p)
{
	int i = 0;
	while (*p != NULL)
	{
		i++;
		p++;
	}
	return i;
}

int main()
{
	char str[20];
	printf("Input string: ");
	scanf("%[^\n]%*c", str);

	int len = length(str);
	printf("The length of string is %d.", len );
	getchar();
}

*/

#include <stdlib.h>

struct Node {
	char *value;
	struct Node* next;
};

int main()
{
	Node a;
	a.value = (char*)malloc(sizeof(char));
	a.next = (Node*)malloc(sizeof(Node));
	*a.value = 'a';
	printf("Value of a is: %c", *a.value);

	Node b; 
	b.value = (char*)malloc(sizeof(char));
	b.next = (Node*)malloc(sizeof(Node));
	*a.next = b;

	*b.value = 'b';
	printf("\nValue of b is: %c", *b.value);

	printf("\nNode a's next pointer is located at: %x", a.next);
	printf("\nNode a's next pointer points to Node b located at: %x", *a.next);
	printf("\nNode a's next pointer's value is %c", *a.next->value);

	free(a.value);
	free(a.next);
	free(b.value);
	free(b.next);

	return 0;
}