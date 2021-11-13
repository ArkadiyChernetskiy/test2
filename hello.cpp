#include <iostream>

int main(int argc, char** argv)
{
  int mm[1000];
  int* a = new int[1000];
  delete[] a;
  std::cout << "123";
  return 0;
}
