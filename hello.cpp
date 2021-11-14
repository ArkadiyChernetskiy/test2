#include <iostream>

int main(int argc, char** argv)
{
  int mm[10];
  int* a = new int[1000];
  delete[] a;
  std::cout << "123";
  return 0;
}