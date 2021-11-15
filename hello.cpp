#include <iostream>
#include <cstdlib>

int main (int argc, char** argv)//массив строк= массив массивов символов
{
    int k=argc-1;
    int* len=new int [k];
    //формируем массив из длин
    for (int i=1; i<k+1; i++){
        len[i-1]=0;
        for (int j=0; argv[i][j]!='\0'; j++ ){
            int digit =argv[i][j]-'0';
            len[i-1]*=10;
            len[i-1]+=digit;
        }
        //std::cout<< len[i-1]<<'\n';
    }
    int** a=new int* [k]; //массив из указателей +
    //считываем элементы массивов
    for ( int i=0; i<k; i++){
        a[i]=new int [len[i]];
        for ( int j=0; j< len[i]; j++){
            std::cin>> a[i][j];
        }
    }
    //перебор
    long long answer=0;
    int* ind=new int[k]; //+
    for (int i=0; i<k; i++){
        ind[i]=0;
    }
    while ( ind[k-1]!=len[k-1]){
        int flag=0;
        for (int j=0; j<k; j++) {
            for (int t = j + 1; t < k; t++) {
                if (ind[j] == ind[t]) {
                    flag = 1;
                    break;
                }
            }
        }
        if ( flag==0){ //т.е все числа в наборе индексов оказались различными
            long long p=1;
            for (int j=0; j<k; j++){
                p*=a[j][ind[j]];
            }
            answer+=p;
        }
        ind[0]+=1;
        for (int i=0; i<k-1; i++){
            if ( ind[i]>=len[i]){
                ind[i]=0;
                ind[i+1]+=1;
            }//(k-1)-ый не меняем, его проверяем в начале!
        }
    }
    std::cout<< answer;
    std::cout.flush();
    //осталось правильно прописать все delete
    delete[] ind;
    for (int i=0; i<k; i++){
        delete[] a[i];
    }
    delete[] a;
    delete[] len;
    delete[] argv;

    return 0;
}