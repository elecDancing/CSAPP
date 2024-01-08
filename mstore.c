/*
 * @Descripttion: 
 * @version: 
 * @Author: xp.Zhang
 * @Date: 2023-12-13 18:09:45
 * @LastEditors: xp.Zhang
 * @LastEditTime: 2023-12-13 18:11:23
 */
#include<stdio.h>
long mult2(long, long);

void multstore(long x, long y, long *dest){
    long t = mult2(x, y);
    *dest = t;
}