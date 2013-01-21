
#include <stdio.h>
#include <omp.h>
#include <boost/lexical_cast.hpp>

int main(int argc, char* argv[])
{	
int i,j;	
double k=0;	
register int a[100000],b[100000],c[100000];	//数据类型可以更改，以测试其它数据效果。

int threads = argc > 1 ? boost::lexical_cast<int>(argv[1]) : 1;

omp_set_num_threads(threads);	 //指定线程数。	
for(i=0;i<100000;i++){a[i]=1;b[i]=1;}	
#pragma omp parallel for shared(a,b,c) private(i,j)	
for(i=0;i<100000;i++)	 //通过指定循环上边界来控制时间。
{	
for(j=0;j<100000;j++)
{	
if(i%2==0) c[j]=a[j]*b[j];	
else c[j]=a[j]+b[j]; 
k++;	
}
}	
printf("%e\n",k);
}

