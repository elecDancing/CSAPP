# CSAPP lab1 dataLab

## 使用&和~表示异或

![](https://telegraph-image-6yo.pages.dev/file/839f2030ff435afb48b00.png)

## 最小的二进制补码

0x80000000 int32共有八个字节，32个bit，每个字节可以同一个十六进制数表示，转化为二进制为1 + 后面31个0 ，1是符号位，这个十六进制数表示的是int32最小的数字的补码。

## 补码和~补码的和为-1

## 不是0的全是1

这道题目中x ？ y : z 这个表达式很容易让人误解成 x>0 return y

实际上是x!=0 也会return y

也就是x = -5 的时候也会return y

<img src="https://telegraph-image-6yo.pages.dev/file/ec2489ed9ae9f886da82e.png" alt="image-20231212164400787" style="zoom:67%;" />

## 符号相同的两个数相减肯定不会溢出

但是符号不同的两个数相减可能会溢出

<img src="https://telegraph-image-6yo.pages.dev/file/02151acae919ed000ab14.png" alt="image-20231212171641064" style="zoom:67%;" />

## 返回最少的表示比特

```text
return the minimum number of bits required to represent x in
 *             two's complement
 *  Examples: howManyBits(12) = 5
 *            howManyBits(298) = 10
 *            howManyBits(-5) = 4
 *            howManyBits(0)  = 1
 *            howManyBits(-1) = 1
```

本题就是要找到从右向左，最左边的1在第几位然后加上一位符号位即可，如果是负数的话我们对其取反然后是同样的操作

例如 -5  = 0xfffffffb

对其取反就是0x00000004

4 = 0100 

那么最左边的1在第三位 再加上一个符号位就是四位

## 浮点数转换为整数

![image-20231213101945450](https://telegraph-image-6yo.pages.dev/file/f7c23fb65a78f5b41ee7e.png)

M是frac部分，E确定了小数点的位置，所以当E < 23 的时候，尾数需要舍去 23 - E 位

具体操作如下， 如果是E<0 那么就是 exp = 0，说明是纯小数，那么表示的是0.****的小数，直接转换为0；

如果 exp = 255 是特殊值 直接返回0x80000000u 如果超范围了(E>=31)也会直接返回0x80000000u

如果是规格化的数，我们正常处理

按照上面的公式

1. 先给尾数补充上省略的1

2. 判断E < 23 则尾数需要舍去23 - E 位
3. 根据符号位返回

## 浮点数的表示范围

![image-20231213105016363](https://telegraph-image-6yo.pages.dev/file/695fdbef03b6587ce1637.png)

