//
//  main.cpp
//  tripDES
//
//  Created by Andrew Chichak on 2017-09-30.
//  Copyright © 2017 Andrew Chichak. All rights reserved.
//

#include <iostream>
#include <bitset>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

//"good" functions checked against http://page.math.tu-berlin.de/~kant/teaching/hess/krypto-ws2006/des.htm

std::bitset<56> PermutedChoice1(std::bitset<64> in){
	std::bitset<56> out;
	const int PC1Table[56]={
		57, 49, 41, 33, 25, 17,  9,
		 1, 58, 50, 42, 34, 26, 18,
		10,  2, 59, 51, 43, 35, 27,
		19, 11,  3, 60, 52, 44, 36,
		63, 55, 47, 39, 31, 23, 15,
		 7, 62, 54, 46, 38, 30, 22,
		14,  6, 61, 53, 45, 37, 29,
		21, 13,  5, 28, 20, 12,  4
	};
	for (int i=55,j=0; i>=0; i--,j++) {
		out[i]=in[64-PC1Table[j]];
	}
	return out;
}//good

std::bitset<48> PermutedChoice2(std::bitset<56> in){
	std::bitset<56> out;
	const u_int8_t PC2Table[56]={
		14, 17, 11, 24,  1,  5,  3, 28,
		15,  6, 21, 10, 23, 19, 12,  4,
		26,  8, 16,  7, 27, 20, 13,  2,
		41, 52, 31, 37, 47, 55, 30, 40,
		51, 45, 33, 48, 44, 49, 39, 56,
		34, 53, 46, 42, 50, 36, 29, 32
	};
	for (int i=55,j=0; i>=0; i--,j++) {
		out[i]=in[56-PC2Table[j]];
	}
	std::bitset<48> out2=0;
	for (int i=8; i<56; i++) {
		out2[i-8]=out[i];
	}
	
	return out2;
}//good

template <int N1, int N2 >
std::bitset <N1 + N2> concat( const std::bitset <N1> & b1, const std::bitset <N2> & b2 ) {
	std::string s1 = b1.to_string();
	std::string s2 = b2.to_string();
	return std::bitset <N1 + N2>( s1 + s2 );
}//good

std::vector<std::bitset<48>> KeyGen(std::bitset<64> inKey,bool isEncript){
	std::vector<std::bitset<48>> outSet;
	std::bitset<56> pc1Key = PermutedChoice1(inKey);
	
	std::bitset<28> cBlock;
	std::bitset<28> dBlock;
	
	for (int i=0; i<56; i++) {
		if (i<28){
			dBlock[i]=pc1Key[i];
		}
		else{
			cBlock[i-28]=pc1Key[i];
		}
	}
	
	int rotMount[16]{1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1};
	
	for (int i=0; i<16; i++) {
		for (int j=rotMount[i]; j>0; j--) {
			cBlock=(cBlock>>27|cBlock<<1);
			dBlock=(dBlock>>27|dBlock<<1);
		}
		outSet.push_back(PermutedChoice2( concat<28,28>(cBlock, dBlock)));
	}
	if (!isEncript) {
		std::reverse(outSet.begin(), outSet.end());
	}
	return outSet;
}//good

std::bitset<64> InitialPermutation(std::bitset<64> in){
	std::bitset<64> out;
	const int IPTable[64]={
		58, 50, 42, 34, 26, 18, 10, 2,
		60, 52, 44, 36, 28, 20, 12, 4,
		62, 54, 46, 38, 30, 22, 14, 6,
		64, 56, 48, 40, 32, 24, 16, 8,
		57, 49, 41, 33, 25, 17,  9, 1,
		59, 51, 43, 35, 27, 19, 11, 3,
		61, 53, 45, 37, 29, 21, 13, 5,
		63, 55, 47, 39, 31, 23, 15, 7
	};
	for (int i=0; i<64; i++) {
		out[i]=in[IPTable[i]-1];
	}
	return out;
}//good

std::bitset<64> NotInitialPermutation(std::bitset<64> in){
	std::bitset<64> out;
	const int IPTable[64]={
		40,     8,   48,    16,    56,   24,    64,   32,
		39,     7,   47,    15,    55,   23,    63,   31,
		38,     6,   46,    14,    54,   22,    62,   30,
		37,     5,   45,    13,    53,   21,    61,   29,
		36,     4,   44,    12,    52,   20,    60,   28,
		35,     3,   43,    11,    51,   19,    59,   27,
		34,     2,   42,    10,    50,   18,    58,   26,
		33,     1,   41,     9,    49,   17,    57,   25
	};
	for (int i=0; i<64; i++) {
		out[i]=in[IPTable[i]-1];
	}
	return out;
}

std::bitset<48> ExpansionBox(std::bitset<32> in){
	std::bitset<48> out;
	const u_int8_t expTable[48]={
		32,  1,  2,  3,  4,  5,
		 4,  5,  6,  7,  8,  9,
		 8,  9, 10, 11, 12, 13,
		12, 13, 14, 15, 16, 17,
		16, 17, 18, 19, 20, 21,
		20, 21, 22, 23, 24, 25,
		24, 25, 26, 27, 28, 29,
		28, 29, 30, 31, 32,  1,
	};
	for (int i=47; i>=0; i--) {
		out[i]=in[expTable[i]-1];
	}
	return out;
}//good

std::bitset<32> SelectionBox(std::bitset<48> in){
	std::bitset<32> out;
	bool in2[48]={false};
	for (int i=0; i<48; i++) {
		in2[i]=in[47-i];
	}
	int sbox[8][4][16]={{
		{14,  4,  13,  1,   2, 15,  11,  8,   3, 10,   6, 12,   5,  9,   0,  7},
		{ 0, 15,   7,  4,  14,  2,  13,  1,  10,  6,  12, 11,   9,  5,   3,  8},
		{ 4,  1,  14,  8,  13,  6,   2, 11,  15, 12,   9,  7,   3, 10,   5,  0},
		{15, 12,   8,  2,   4,  9,   1,  7,   5, 11,   3, 14,  10,  0,   6, 13}
	},{
		{15,  1,   8, 14,   6, 11,   3,  4,   9,  7,   2, 13,  12,  0,   5, 10},
		{ 3, 13,   4,  7,  15,  2,   8, 14,  12,  0,   1, 10,   6,  9,  11,  5},
		{ 0, 14,   7, 11,  10,  4,  13,  1,   5,  8,  12,  6,   9,  3,   2, 15},
		{13,  8,  10,  1,   3, 15,   4,  2,  11,  6,   7, 12,   0,  5,  14,  9}
	},{
		{10,  0,   9, 14,   6,  3,  15,  5,   1, 13,  12,  7,  11,  4,   2,  8},
		{13,  7,   0,  9,   3,  4,   6, 10,   2,  8,   5, 14,  12, 11,  15,  1},
		{13,  6,   4,  9,   8, 15,   3,  0,  11,  1,   2, 12,   5, 10,  14,  7},
		{ 1, 10,  13,  0,   6,  9,   8,  7,   4, 15,  14,  3,  11,  5,   2, 12}
	},{
		{ 7, 13,  14,  3,   0,  6,   9, 10,   1,  2,   8,  5,  11, 12,   4, 15},
		{13,  8,  11,  5,   6, 15,   0,  3,   4,  7,   2, 12,   1, 10,  14,  9},
		{10,  6,   9,  0,  12, 11,   7, 13,  15,  1,   3, 14,   5,  2,   8,  4},
		{ 3, 15,   0,  6,  10,  1,  13,  8,   9,  4,   5, 11,  12,  7,   2, 14}
	},{
		{ 2, 12,   4,  1,   7, 10,  11,  6,   8,  5,   3, 15,  13,  0,  14,  9},
		{14, 11,   2, 12,   4,  7,  13,  1,   5,  0,  15, 10,   3,  9,   8,  6},
		{ 4,  2,   1, 11,  10, 13,   7,  8,  15,  9,  12,  5,   6,  3,   0, 14},
		{11,  8,  12,  7,   1, 14,   2, 13,   6, 15,   0,  9,  10,  4,   5,  3}
	},{
		{12,  1,  10, 15,   9,  2,   6,  8,   0, 13,   3,  4,  14,  7,   5, 11},
		{10, 15,   4,  2,   7, 12,   9,  5,   6,  1,  13, 14,   0, 11,   3,  8},
		{ 9, 14,  15,  5,   2,  8,  12,  3,   7,  0,   4, 10,   1, 13,  11,  6},
		{ 4,  3,   2, 12,   9,  5,  15, 10,  11, 14,   1,  7,   6,  0,   8, 13}
	},{
		{ 4, 11,   2, 14,  15,  0,   8, 13,   3, 12,   9,  7,   5, 10,   6,  1},
		{13,  0,  11,  7,   4,  9,   1, 10,  14,  3,   5, 12,   2, 15,   8,  6},
		{ 1,  4,  11, 13,  12,  3,   7, 14,  10, 15,   6,  8,   0,  5,   9,  2},
		{ 6, 11,  13,  8,   1,  4,  10,  7,   9,  5,   0, 15,  14,  2,   3, 12}
	},{
		{13,  2,   8,  4,   6, 15,  11,  1,  10,  9,   3, 14,   5,  0,  12,  7},
		{ 1, 15,  13,  8,  10,  3,   7,  4,  12,  5,   6, 11,   0, 14,   9,  2},
		{ 7, 11,   4,  1,   9, 12,  14,  2,   0,  6,  10, 13,  15,  3,   5,  8},
		{ 2,  1,  14,  7,   4, 10,   8, 13,  15, 12,   9,  0,   3,  5,   6, 11}
	}};
	for (int i=0; i<8; i++) {
		out<<=4;
		std::bitset<6> selector;
		for (int j=0; j<6; j++) {
			selector[5-j]=in2[6*i+j];
		}
		std::bitset<2> vert;
		std::bitset<4> horizon;
		vert[1]=selector[5];
		vert[0]=selector[0];
		horizon[3]=selector[4];
		horizon[2]=selector[3];
		horizon[1]=selector[2];
		horizon[0]=selector[1];
		out|=sbox[i][vert.to_ulong()][horizon.to_ulong()];
	}
	return out;
}//good

std::bitset<32> PBox(std::bitset<32> in){
	std::bitset<32> out=0;
	std::bitset<32> in2=0;
	for (int i=0; i<32; i++) {
		in2[i]=in[31-i];
	}
	const int IPTable[32]={
		16,   7,  20,  21,
		29,  12,  28,  17,
		 1,  15,  23,  26,
		 5,  18,  31,  10,
		 2,   8,  24,  14,
		32,  27,   3,   9,
		19,  13,  30,   6,
		22,  11,   4,  25
	};
	for (int i=0; i<32; i++) {
		out[i]=in2[IPTable[i]-1];
	}
	std::bitset<32> out2=0;
	for (int i=0; i<32; i++) {
		out2[i]=out[31-i];
	}
	return out2;
}//good

std::bitset<64> DES(u_int64_t inBlock, std::vector<std::bitset<48> > &keys) {
	std::bitset<64> bitBlock=inBlock;
	std::bitset<64> permBlock=InitialPermutation(bitBlock);
	std::bitset<32> leftBlock;
	std::bitset<32> rightBlock;
	for (int i=0; i<64; i++) {
		if (i<32){
			rightBlock[i]=permBlock[i];
		}
		else{
			leftBlock[i-32]=permBlock[i];
		}
	}
	for (int i=0; i<16; i++) {
		std::bitset<48> expBlock=ExpansionBox(rightBlock);
		std::bitset<48> xorBlock=expBlock^keys[i];
		std::bitset<32> sBlock=SelectionBox(xorBlock);
		std::bitset<32> pBlock=PBox(sBlock);
		std::bitset<32> temp = rightBlock;
		rightBlock = pBlock^leftBlock;
		leftBlock = temp;
	}
	std::bitset<64> full = NotInitialPermutation(concat<32,32>(rightBlock, leftBlock));
	return full;
}

/*
int main() {
	std::bitset<64> plaintext=0x123456789ABCDEF;
	std::bitset<64> Key=0x133457799BBCDFF1;
	std::vector<std::bitset<48>> keys = KeyGen(Key, true);
	std::bitset<64> permBlock=InitialPermutation(plaintext);
	std::bitset<32> leftBlock;
	std::bitset<32> rightBlock;
	for (int i=0; i<64; i++) {
		if (i<32){
			rightBlock[i]=permBlock[i];
		}
		else{
			leftBlock[i-32]=permBlock[i];
		}
	}
	for (int i=0; i<16; i++) {
		std::bitset<48> expBlock=ExpansionBox(rightBlock);
		std::bitset<48> xorBlock=expBlock^keys[i];
		std::bitset<32> sBlock=SelectionBox(xorBlock);
		std::bitset<32> pBlock=PBox(sBlock);
		std::bitset<32> temp = rightBlock;
		rightBlock = pBlock^leftBlock;
		leftBlock = temp;
	}
	std::bitset<64> full = NotInitialPermutation(concat<32,32>(rightBlock, leftBlock));
	std::bitset<64> ref=0b1000010111101000000100110101010000001111000010101011010000000101;

	return 0;
}
*/

int main() {
	std::string inFile;
	std::cout << "Input file name to use: ";
	std::cin >> inFile;
	std::ifstream fi;
	fi.open(inFile, std::ios::in | std::ios::binary);
	std::string outFile;
	std::cout << "Output file name to use: ";
	std::cin >> outFile;
	std::ofstream fo;
	fo.open(outFile, std::ios::out | std::ios::binary);
	
	if (fi.is_open()&&fo.is_open()) {
		
		u_int64_t inKey;
		std::cout << "Enter key 1: ";
		std::cin >> std::hex >> inKey;
		std::bitset<64> bitKey1(inKey);
		std::cout << "Enter key 2: ";
		std::cin >> std::hex >> inKey;
		std::bitset<64> bitKey2(inKey);
		std::cout << "Enter key 3: ";
		std::cin >> std::hex >> inKey;
		std::bitset<64> bitKey3(inKey);
		
		char routine;
		bool encrypt=false;
		std::cout<<"Encription?:(y/n) ";
		std::cin>>routine;
		if (routine=='y') {
			encrypt=true;
		}
		std::vector<std::bitset<48>> keys1 = KeyGen(bitKey1, encrypt);
		std::vector<std::bitset<48>> keys2 = KeyGen(bitKey2, !encrypt);
		std::vector<std::bitset<48>> keys3 = KeyGen(bitKey3, encrypt);
		
		while (fi.peek()!=EOF){
			u_int64_t inBlock = 0;
			u_int8_t inBlock2;
			int numunused=0;
			for (int i=0;i<8;i++){
				fi.read(reinterpret_cast<char *>(&inBlock2), sizeof(inBlock2));
				if(fi.peek()==EOF && numunused==0){
					numunused=i-1;
				}
				inBlock <<= 8;
				inBlock |= inBlock2;
			}
			std::bitset<64>temp=inBlock;
			for (int i=0; i<numunused*8; i++) {
				temp[i]=0b0;
			}
			inBlock=temp.to_ullong();
			std::bitset<64> firstRound = DES(inBlock, keys1);
			std::bitset<64> secondRound = DES(firstRound.to_ullong(), keys2);
			std::bitset<64> thirdRound = DES(secondRound.to_ullong(), keys3);
			
			uint64_t numberTheThird=thirdRound.to_ullong();
			unsigned char *outBlock2=(unsigned char*)&numberTheThird;
			
			char outBlock3[8];
			for (int i=0; i<8; i++) {
				outBlock3[7-i]=outBlock2[i];
			}
			for (int i=0;i<8;i++){
				fo<<outBlock3[i];

			}
		}
	}
	else
		std::cout<<"Failure to open File"<<std::endl;
	
	return 0;
}
