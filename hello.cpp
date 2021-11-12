#include <iostream>

int main(int argc, char** argv)
{
  int* mm = new int [1000000];
  mm[100] = 0;
  mm[100]+= 1;
  delete[] mm;

  std::cout << atoi (argv[1]);
  for (int i = 1; i < argc; i++)
  {
    
    for (int j = 0; j < atoi (argv[i]); j++)
    {
      int a = 0;
      std::cin >> a;
      
    }
  }
  return 0;
}
