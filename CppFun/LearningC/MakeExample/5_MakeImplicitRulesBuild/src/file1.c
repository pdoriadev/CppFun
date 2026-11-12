#include "../includes/helloworld.h"
#include "../includes/file1.h"
#include <stdio.h>

int add(int a, int b){
  return a + b;
}

int main() {
  char c = 'c';
  printf("hello world \n");
  printf("added %d\n", add(1, 1));
  say_hello();
  return 0;
}
