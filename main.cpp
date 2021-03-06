#include <iostream>
#include <cstdint>
#include <ctime>

void printText();
void printNow();

//CONST
const unsigned char SBOX[256] = {
        0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
        0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
        0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
        0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
        0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
        0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
        0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
        0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
        0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
        0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
        0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
        0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
        0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
        0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
        0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
        0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16,
};

const unsigned char RCON[10] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36};


//VARIABLE
uint8_t text[16] = {0x01, 0x23, 0x45, 0x67,
                    0x89, 0xab, 0xcd, 0xef,
                    0xfe, 0xdc, 0xba, 0x98,
                    0x76, 0x54, 0x32, 0x10};
uint64_t *text64 = (uint64_t *)text;
uint8_t userKey[16] = {0x0f, 0x15, 0x71, 0xc9,
                       0x47, 0xd9, 0xe8, 0x59,
                       0x0c, 0xb7, 0xad, 0xd6,
                       0xaf, 0x7f, 0x67, 0x98};
uint64_t *userKey64 = (uint64_t *)userKey;

uint8_t state[16];

uint8_t  keyExtended[176];
uint64_t *keyExtended64 = (uint64_t *)keyExtended;
uint32_t *keyExtended32 = (uint32_t *)keyExtended;

uint32_t T[4][256];

uint8_t *ptNow = state;
uint32_t *ptNow32 = (uint32_t *)ptNow;
uint64_t *ptNow64 = (uint64_t *)ptNow;
uint8_t *ptPre = text;


//FUNCTION


inline uint8_t multiple02(unsigned char a) {
    if ((a & 0x80) == 0x80) return ((a << 1) ^ 0x1b);
    else return (a << 1);
}

inline uint8_t multiple03(unsigned char a) {
    return multiple02(a) ^ a;
}

inline uint8_t multiGF28(int a, uint8_t b) {
    if (a == 2) return multiple02(b);
    else if (a == 3) return multiple03(b);
    else return b;
}

void InitTable() {
    int matrix[4][4] = {
            {2, 3, 1, 1},
            {1, 2, 3, 1},
            {1, 1, 2, 3},
            {3, 1, 1, 2}
    };
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 256; ++j) {
            uint8_t *pt = (uint8_t *) (&T[i][j]);
            for (int k = 0; k < 4; ++k)
                pt[k] = multiGF28(matrix[k][i], SBOX[j]);
        }
}

inline uint32_t g(uint32_t word, int round) {
    uint8_t byte[4];
    byte[0] = (uint8_t) word;
    byte[1] = (uint8_t) (word >> 8);
    byte[2] = (uint8_t) (word >> 16);
    byte[3] = (uint8_t) (word >> 24);
    word = (uint32_t) (SBOX[byte[1]] ^ RCON[round - 1]) |
           (uint32_t) SBOX[byte[2]] << 8 |
           (uint32_t) SBOX[byte[3]] << 16 |
           (uint32_t) SBOX[byte[0]] << 24;
    return word;
}

void keyExpansion() {
    keyExtended64[0] = userKey64[0];
    keyExtended64[1] = userKey64[1];
    for (int i = 4; i < 44; i += 4) {
        keyExtended32[i+0] = keyExtended32[i-4] ^ g(keyExtended32[i-1], i/4);
        keyExtended32[i+1] = keyExtended32[i-3] ^ keyExtended32[i+0];
        keyExtended32[i+2] = keyExtended32[i-2] ^ keyExtended32[i+1];
        keyExtended32[i+3] = keyExtended32[i-1] ^ keyExtended32[i+2];
    }
}


inline void lastTransform() {
    text[0] =  SBOX[state[0]] ^keyExtended[160];
    text[1] =  SBOX[state[5]] ^keyExtended[161];
    text[2] =  SBOX[state[10]]^keyExtended[162];
    text[3] =  SBOX[state[15]]^keyExtended[163];
    text[4] =  SBOX[state[4]] ^keyExtended[164];
    text[5] =  SBOX[state[9]] ^keyExtended[165];
    text[6] =  SBOX[state[14]]^keyExtended[166];
    text[7] =  SBOX[state[3]] ^keyExtended[167];
    text[8] =  SBOX[state[8]] ^keyExtended[168];
    text[9] =  SBOX[state[13]]^keyExtended[169];
    text[10] = SBOX[state[2]] ^keyExtended[170];
    text[11] = SBOX[state[7]] ^keyExtended[171];
    text[12] = SBOX[state[12]]^keyExtended[172];
    text[13] = SBOX[state[1]] ^keyExtended[173];
    text[14] = SBOX[state[6]] ^keyExtended[174];
    text[15] = SBOX[state[11]]^keyExtended[175];
}


void Encrypt() {
    uint8_t *foo;
    text64[0] ^= keyExtended64[0];
    text64[1] ^= keyExtended64[1];
    for (int round = 1; round < 10; ++round) {
        ptNow32[0] = T[0][ptPre[0]] ^ T[1][ptPre[5]] ^ T[2][ptPre[10]] ^T[3][ptPre[15]]^ keyExtended32[round*4+0];
        ptNow32[1] = T[0][ptPre[4]] ^ T[1][ptPre[9]] ^ T[2][ptPre[14]] ^T[3][ptPre[3]] ^ keyExtended32[round*4+1];
        ptNow32[2] = T[0][ptPre[8]] ^ T[1][ptPre[13]] ^T[2][ptPre[2]] ^ T[3][ptPre[7]] ^ keyExtended32[round*4+2];
        ptNow32[3] = T[0][ptPre[12]] ^T[1][ptPre[1]] ^ T[2][ptPre[6]] ^ T[3][ptPre[11]]^ keyExtended32[round*4+3];
        foo =(uint8_t *) ptNow32;
        ptNow32 = (uint32_t *)ptPre;
        ptPre = foo;
    }
    lastTransform();
}

//Test
void printKeyExtend() {
    for (int i = 0; i < 11; ++i) {
        printf("\n");
        for (int j = 0; j < 16; ++j) {
            printf("%#x ", keyExtended[i*16+j]);
        }
    }
}

void printText() {
    for (int j = 0; j < 16; ++j) {
        printf("%#x ", text[j]);
    }
    printf("\n");
}

void printNow() {
    for (int j = 0; j < 16; ++j) {
        printf("%#x ", ptNow[j]);
    }
    printf("\n");
}

int main() {
    using namespace std;
    InitTable();
    keyExpansion();
    Encrypt();
    printText();
    //0xff 0xb 0x84 0x4a 0x8 0x53 0xbf 0x7c 0x69 0x34 0xab 0x43 0x64 0x14 0x8f 0xb9

    long time = 0x2000000;
    clock_t start, end;
    start = clock();
    for (int i = 0; i < time; ++i) {
        Encrypt();
    }
    end = clock();
    double total = (double) (end - start) / CLOCKS_PER_SEC;
    cout << "time for run " << time << " is :" << total << endl;
    cout << "bit for per second is: " << (time / total) * 128 << "bit" << endl;
    cout << ((time / total) * 128) / (8 * 1000000) << "MB" << endl;

    return 0;
}
