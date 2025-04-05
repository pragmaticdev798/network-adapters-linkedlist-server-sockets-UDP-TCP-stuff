#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int main()
{
    time_t timer;
    time(&timer);
    printf("yo dawg your Local time is :%s",ctime(&timer));
    
    system("pause");
    return 0;
}
