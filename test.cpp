/*
 * @Descripttion: 
 * @version: 
 * @Author: xp.Zhang
 * @Date: 2023-12-05 01:15:06
 * @LastEditors: xp.Zhang
 * @LastEditTime: 2024-01-17 18:42:41
 */
// #include<stdio.h>
// int main(){
    // unsigned int x = 2147483648;
    // int y = (int)x; //-2147483648
    // printf("x = %d \n",y);

    // x = 1;
    // y = -2147483647;
    // printf("%s \n",(y - x < -2147483647) ? " True " : " Flase ");
    // printf("%u \n",y - x);
    // printf("%u \n",-2147483647);
    // printf("%x", 0x80 << 24);
    // printf("%d", !((0xffffffff & 0xAAAAAAAA) ^ 0xAAAAAAAA));
    // int a = 0x7f << 8; //0x7f00
    // int b = 0xff | a; //0x7fff
    // int c = (0xff << 8) | 0xff;
    // int d = b << 16 | c;
    // printf("%x", d);
    // int x, y, z;
    // scanf("%d %d %d",&x,&y,&z);
    // int a=!!(x); //a=0 if x<0 else a =1
    // int b=~a+1;
    // printf("%x %x \n", b, ~b);
    // int c=~(y&~b)+1;
    // printf("%x\n", c);
    // int d=~(z&b)+1;
    // printf("%d\n", d);
    // printf("%d\n", !(-5));
    // 先将x转为bool， 然后求其nagate，因为-(1)的补码 全1， -(0）的补码为全0
// 我们转换出这两个特别的补码 就可以&了
// int conditional(int x, int y, int z)  // 8个ops
// {
     // printf("%d", 160 % 16);

// }
#include <iostream>
#include <vector>

void func() {
    std::vector<int> v;
    for (int i = 0; i < 10000000; ++i) {
        v.push_back(i);
    }
}

int main() {
    func();
    return 0;
}