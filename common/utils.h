// utils.h
#ifndef UTILS_H
#define UTILS_H

// Function declarations 
unsigned char hex2byte(char *hex);
unsigned char * hex2bytes(char *input, int len);
char * hex2base64(char * input);
unsigned char * base642bytes(char * input);
unsigned char * bytes2hex(unsigned char * input, int len);
int max(int a, int b);
int findIndex(char * str, char element);
unsigned char * singlebyteXOR(unsigned char * bytes, unsigned char byte, int len);
void freqcalc(unsigned char * bytes, float * freqprobs, int len);
float cipherscore(unsigned char * encoded, int len);
unsigned char findSingleXORKey(unsigned char * encoded, int len);
unsigned char * RKXOR(unsigned char * message, unsigned char * key, int meslen, int keylen);
int hammingdistance(unsigned char * str1, unsigned char * str2, int len);
unsigned char gf_multiply(unsigned char a, unsigned char b);
unsigned char affine_transform(unsigned char x);
void shift(unsigned char * word);
void shiftby(unsigned char * word, int n);

#endif // UTILS_H
