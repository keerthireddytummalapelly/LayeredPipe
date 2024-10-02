#include<stdio.h>
#include<string.h>
#include"encDec.h"
void toUpperCase(char * input) {
	// Convert lowercase letters to uppercase
	for(int i = 0; i < strlen(input) ; i++) {
		if( input[i] >= 'a' && input[i] <='z') {
			input[i] -= 32;
		}
		printf("%c",input[i]);
	}
}
int main(int argc, char * argv[]) {
	toUpperCase(argv[1]);
}
