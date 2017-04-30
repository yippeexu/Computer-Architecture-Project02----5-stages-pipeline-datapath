#include <stdio.h>



void intToBin(unsigned int num, char bin[], int size)
{
	for (int i = 0; i < size; i++) {
		bin[i] = num%2 ? '1' : '0' ;
		num /= 2;				}
}
unsigned int binToInt(char bin[], int head, int tail)
{
	unsigned int sum = 0, power = 1;
	unsigned int shamt = head - tail;
	while (tail <= head) {
		sum += ( bin[tail++] == '1' ? 1 : 0 ) * power;
		power *= 2;
	}
	if ( (sum >> shamt ) == 1 )
		sum = sum | ( -1 << ( shamt + 1 ) );
	return sum;
}
unsigned int binToUnsInt(char bin[], int head, int tail)
{
	unsigned int sum = 0, power = 1;
	while (tail <= head) {
		sum += ( bin[tail++] == '1' ? 1 : 0 ) * power;
		power *= 2;
	}
	return sum;
}
void printBin(char bin[])
{
	for (int i = 31; i >= 0; i--) {
		if ( i==26 || i==21 || i==16 || i==11 || i==6/*i%4==0*/ )
			printf("%c ", bin[i]);
		else
			printf("%c", bin[i]);
	}	printf("\n");
}