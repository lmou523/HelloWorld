

#include <stdio.h>
#include <Windows.h>



int funadd(int a, int b)
{
	int c = 100;
	int d = 200;
	return a + b + c + d;
}


int main(int argc, char* argv)
{
	
	int c = 10;

	for (int i = 0; i < 100; ++i)
	{
		int a = 4;
		c += a;
	}

	funadd(50, 80);
	printf("----%d----\n----%d----\n",c,funadd(50,90));

	system("pause");
	return 0;
}