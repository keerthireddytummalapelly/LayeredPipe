#include<stdio.h>
#include<string.h>
#include"encDec.h"
// Add odd parity bit to the binary value
void addParity(int input[7]){
	int parity_bit = 0, count = 0;
	// Count number of ones
	for (int i = 0; i < 7; i++) {
		if(input[i] == 1) {
			count++;
		}
	}
	// Set parity bit if number of ones is even 
	if (count % 2 == 0) {
		parity_bit = 1;
	}
	printf("%d",parity_bit);
	for(int i = 6; i >= 0; i--){
		printf("%d",input[i]);
	}
}

void integerToBinary(int input) {
	int k = 0;
	int binary[7] = {0};
	// Convert integer to binary
	while(input != 0) {
		binary[k++] = input%2;
		input /= 2;
	}
	addParity(binary);
}
void characterToBinary(char input) {
	int ch = input;
	int binary[7] = {0}, i = 0;
	// Convert character to binary
	if (input != EOF && input != '\0') {
		while(ch != 0) {
			binary[i++] = ch%2;
			ch /= 2;
		}
		addParity(binary);
	}
}

void binaryToCharacter(int binary[7]){
	int number = 0, power_of_2 = 1;
	char ch;
	// Convert binary bits to character
	for(int i = 0; i < 7; i++){
		number += binary[i] * power_of_2;
		power_of_2 *= 2;
	}
	
	ch =  number;
	printf("%c", ch);
}
void binaryToInteger(int binary[7]){
	int number = 0, power_of_2 = 1;
	char ch;
	// Convert binary bits to integer
	for(int i = 0; i < 7; i++){
		number += binary[i] * power_of_2;
		power_of_2 *= 2;
	}
	printf("%d",number);
}

// Remove parity bit from binary value
void removeParity(char * input, char * type){
        int parity_bit = input[0], binary[7], count = 0, k = 0;
        for (int i = 7; i >= 1; i--) {
                if(input[i] == '1') {
                        count++;
                        binary[k++] = 1;
                }
                else {
                	binary[k++] = 0;
                }
        }
        // Check parity bit value
        if ((count % 2 == 0 && parity_bit == '1') || (count % 2 == 1 && parity_bit == '0') ) {
		if(strcasecmp(type,"Data") == 0){
			binaryToCharacter(binary);
		}
		else{
			binaryToInteger(binary);
		}
	}	
}
void physicalLayer(char * type, char * data) {
	//If value of type is "encode", encode data
	if (strcasecmp(type,"Encode") == 0) {
		int j = 0; 
		for (int i = 0; i < strlen(data);){
			if ((i+3) < strlen(data) && data[i] == '2' && data[i+1] == '2' && data[i+2] == '2' && data[i+3] == '2'){
				integerToBinary(22);
				integerToBinary(22);
				i += 4;
			}
			else if ((i>=4) && data[i] >='0' && data[i] <= '9' && data[i-1] == '2' && data[i-2] =='2' && data[i-3]=='2' && data[i-4] =='2'){
				if(((i+1) < strlen(data)) && data[i+1] >= '0' && data[i+1] <= '9') {
					integerToBinary(((data[i]-'0') * 10) + (data[i+1]-'0'));
					i += 2;
				}
				else {
					integerToBinary(data[i]-'0');
					i++;
				}
				
			}
			else{
				characterToBinary(data[i]);
				i++;
			}
		}	
	}
	// Else decode data
	else {
		int k = 0, count = 0;
		char binary[8];
		for (int i = 0; i < strlen(data); i += 8) {
	        	k = 0;
	        	count ++;
	        	for (int j = i; j < i+8; j++) {
	                	binary[k++] = data[j];
	        	}
	        	if ((count%35 >=1) && (count%35 <= 3)) {
	        		removeParity(binary, "Integer");
	        	}
	        	else {
	        		removeParity(binary, "Data");
	        	}
		}
	}
}
int main(int argc, char * argv[]) {
	physicalLayer(argv[1],argv[2]);
	return 0;
}

