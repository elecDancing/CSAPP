# CSAPP lab2 BoomLab 

## 简单介绍

打开炸弹目录，共有三个文件

![image-20240109103742733](https://telegraph-image-6yo.pages.dev/file/454c42c32e6c33b4925b1.png)

其中bomb为二进制可执行文件

使用gdb打开该文件

```
gdb bomb
```

![image-20240109104139884](https://telegraph-image-6yo.pages.dev/file/0db86dba99b3dba96d972.png)

尝试打个断点

在第74行打个端点

```
b 74
```

![image-20240109104301302](https://telegraph-image-6yo.pages.dev/file/b10f98bd16be4593bc95c.png)

运行程序试试

![image-20240109104345053](https://telegraph-image-6yo.pages.dev/file/bd5ec051d0a610e9d986d.png)

程序停在了断点处

`info stack`给出对应栈的位置

```
info stack
```

![image-20240109104433556](https://telegraph-image-6yo.pages.dev/file/a3e6da8ec47d52b246a98.png)

`info r`给出对应寄存器里面的值

其中比较重要的是`rsp`指向的是栈指针

![image-20240109104531904](https://telegraph-image-6yo.pages.dev/file/adde91b37394dbea2e0ea.png)

```
disas
```

disas 查看反汇编的结果

![image-20240109104828337](https://telegraph-image-6yo.pages.dev/file/3ffcc5827f1178e63dba0.png)

## 实验正式开始

![image-20240109105807646](https://telegraph-image-6yo.pages.dev/file/2283e449ef8f734805162.png)

反汇编一下初始化炸弹的函数看看

![image-20240109105854447](https://telegraph-image-6yo.pages.dev/file/996635fe7098719029fc8.png)

栈指针向下8

进入了一个函数

然后栈指针加8 结束

欢迎文本

![image-20240109110537942](https://telegraph-image-6yo.pages.dev/file/f872c0dda8573c8c28d96.png)

## 进入phase_1

![image-20240109110606783](https://telegraph-image-6yo.pages.dev/file/4ac4f220fdf31702aca95.png)

`disas phase_1`

 

```
  0x0000000000400ee0 <+0>:     sub    $0x8,%rsp
   0x0000000000400ee4 <+4>:     mov    $0x402400,%esi
   0x0000000000400ee9 <+9>:     callq  0x401338 <strings_not_equal>
   0x0000000000400eee <+14>:    test   %eax,%eax
   0x0000000000400ef0 <+16>:    je     0x400ef7 <phase_1+23>
   0x0000000000400ef2 <+18>:    callq  0x40143a <explode_bomb>
   0x0000000000400ef7 <+23>:    add    $0x8,%rsp
   0x0000000000400efb <+27>:    retq 
```



| sub    $0x8,%rsp                    | 栈指针下移8位                              |
| :---------------------------------- | :----------------------------------------- |
| mov    $0x402400,%esi               | 传入参数（猜测对应的字符串应该储存在这里） |
| callq  0x401338 <strings_not_equal> | 调用函数<strings_not_equal>                |
| test   %eax,%eax                    | 判断                                       |
| je     0x400ef7 <phase_1+23>        | 两者是否相等，相等的话跳到23               |
| callq  0x40143a <explode_bomb>      | 不相等就会走到这里，炸弹爆炸               |
| add    $0x8,%rsp                    | 函数结束                                   |
| retq                                | 返回                                       |

尝试随便跑一下

![image-20240109113216356](https://telegraph-image-6yo.pages.dev/file/8f1e6e62e27341a850e2f.png)

炸弹爆炸

查看`info stack`发现没有栈了，main函数结束了

查看地址0x402400

`p 0x402400` 

![image-20240109143019565](https://telegraph-image-6yo.pages.dev/file/51bef548065cefa43c9d7.png)

为什么没有输出字符串呢，哈哈，其实这是输出的是字符串的指针而已，也就是字符串首个字母的位置

使用

```
x/s 0x402400
```

就可以显示出这个位置储存的字符串

![image-20240109143248578](https://telegraph-image-6yo.pages.dev/file/38a4473deb66a35dab0f5.png)

run一下看看是否正确

使用`q`退出gdb

![image-20240109143603295](C:/Users/xidian/AppData/Roaming/Typora/typora-user-images/image-20240109143603295.png)

还有一个小彩蛋，可以感觉到设计实验的人是真正的热爱，不是应付任务。

## 进入phase_2

```
disad phase_2
```

```assembly
   0x0000000000400efc <+0>:     push   %rbp                      //被调用者保存寄存器
   0x0000000000400efd <+1>:     push   %rbx		                //被调用者保存寄存器
   0x0000000000400efe <+2>:     sub    $0x28,%rsp               //栈深度为0x28字节
   0x0000000000400f02 <+6>:     mov    %rsp,%rsi	            //把当前栈指针挪到rsi
   0x0000000000400f05 <+9>:     callq  0x40145c <read_six_numbers> //调用函数，读取六个数字
   0x0000000000400f0a <+14>:    cmpl   $0x1,(%rsp)              //Compare 1 : *rsp 
   0x0000000000400f0e <+18>:    je     0x400f30 <phase_2+52>    //判断是否相等 相等的话跳到52
   0x0000000000400f10 <+20>:    callq  0x40143a <explode_bomb>  //炸蛋爆炸，说明地址rsp储存的是数字1
   0x0000000000400f15 <+25>:    jmp    0x400f30 <phase_2+52>	//跳转52行
   0x0000000000400f17 <+27>:    mov    -0x4(%rbx),%eax          //rbx - 4 回到rsp所在位置， *（rbx - 4）为1  *eax = 1
   												//这里是从48行跳转过来，rbx -4 指向第二个数字所在位置
   												//*eax = 2；
   0x0000000000400f1a <+30>:    add    %eax,%eax  	            // *eax = 2;
   												//48行后续
   												//*eax = 4;
   0x0000000000400f1c <+32>:    cmp    %eax,(%rbx)	           // 比较*eax ：*rbx 说明*（rsp + 4） = 2
   												//第三个数字为4
   0x0000000000400f1e <+34>:    je     0x400f25 <phase_2+41>    //相同的话跳到41
   0x0000000000400f20 <+36>:    callq  0x40143a <explode_bomb> //不同炸蛋爆炸
   0x0000000000400f25 <+41>:    add    $0x4,%rbx               // rbx + 4 到达第三个数字
  												//rbx + 4 到达第4个数字
   0x0000000000400f29 <+45>:    cmp    %rbp,%rbx  			  // Compare *rbp : *rbx 对第三个和第五个数字进行比较
   												//对第四个和第五个数字进行比较
   0x0000000000400f2c <+48>:    jne    0x400f17 <phase_2+27>   //不相等的话返回27
   0x0000000000400f2e <+50>:    jmp    0x400f3c <phase_2+64>   //跳转64行
   0x0000000000400f30 <+52>:    lea    0x4(%rsp),%rbx    //rsp + 4 储存的是rbx
   0x0000000000400f35 <+57>:    lea    0x18(%rsp),%rbp	//rsp + 24 储存的是rbp
   0x0000000000400f3a <+62>:    jmp    0x400f17 <phase_2+27>   //返回27行
   0x0000000000400f3c <+64>:    add    $0x28,%rsp
   0x0000000000400f40 <+68>:    pop    %rbx
   0x0000000000400f41 <+69>:    pop    %rbp
   0x0000000000400f42 <+70>:    retq   
```

<img src="https://telegraph-image-6yo.pages.dev/file/d67030b7dd06c6ec58d84.png" alt="image-20240109151339056" style="zoom:25%;" />

中间其实就是一个循环

显然这六个数字是这样的

### 这里插播一下gdb知识

**linux下gdb调试 | next, nexti, step, stepi单步调试详解**

> 在gdb中，有next, nexti, step, stepi等指令来单步调试程序，他们功能各不相同，区别在于单步的“跨度”上。

```
next 单步到程序源代码的下一行，不进入函数。
nexti 单步一条机器指令，不进入函数。
step 单步到下一个不同的源代码行（包括进入函数）。
stepi 单步一条机器指令。
```

可以两两分类来记住：

1. next和nexti(即n和ni)是下一条，不进入函数内部，比如说在某一行发生了函数调用，next/nexti就继续到下一行。next是在源码层面的下一行，而nexti就是机器指令层面的，单步到下一个机器指令。
2. step和stepi(即s和si)就是单步步入，进入函数内部，比如说在某一行发生了函数调用，step/stepi就会进入函数体内部，把函数体执行一遍，再返回执行下一条指令。同理，step是在源码层面的操作指令，stepi是在机器指令层面的。

接下来使用gdb来简单的反汇编一下这段代码

在第82行打一个断点`b 82`

![image-20240109160822379](https://telegraph-image-6yo.pages.dev/file/114178656cb1d980ca14c.png)

```assembly
Breakpoint 1 at 0x400e53: file bomb.c, line 82.
```

运行程序，程序停在了第82行

![image-20240109161133706](https://telegraph-image-6yo.pages.dev/file/f065a88b28abef4e89088.png)



使用`stepi`执行

跳转到read_six_numbers函数的时候

使用finish语句结束这个函数

![image-20240109165246861](https://telegraph-image-6yo.pages.dev/file/a967ad2b34350c4de2b94.png)

查看rsp中的值

```
p $rsp
```

![image-20240109163418408](https://telegraph-image-6yo.pages.dev/file/bdf786a3f057b2fdd8003.png)

查看指针中存储的元素

```
p *(int *) (0x7fffffffd920)
```

![image-20240109163836424](https://telegraph-image-6yo.pages.dev/file/385f00105c88498933604.png)

看到rsp中儲存的是1

和我们预想的一样

再看一下其他位置对不对

![image-20240109165613464](https://telegraph-image-6yo.pages.dev/file/87243321e0e61b42d32b7.png)

为什么rsp + 20位置不是32呢？

![](https://telegraph-image-6yo.pages.dev/file/d93effb42a9d76081d8d6.png)

可以查看一下read_six_numbers函数确定一下六个数字的顺序

![image-20240109171546932](https://telegraph-image-6yo.pages.dev/file/1b5bf62f9f429815bc342.png)

运行一下

两个炸弹成功拆除

## 进入phase_3

```assembly
   0x0000000000400f43 <+0>:     sub    $0x18,%rsp
   0x0000000000400f47 <+4>:     lea    0xc(%rsp),%rcx
   0x0000000000400f4c <+9>:     lea    0x8(%rsp),%rdx
   0x0000000000400f51 <+14>:    mov    $0x4025cf,%esi
   0x0000000000400f56 <+19>:    mov    $0x0,%eax
   0x0000000000400f5b <+24>:    callq  0x400bf0 <__isoc99_sscanf@plt>
   0x0000000000400f60 <+29>:    cmp    $0x1,%eax
   0x0000000000400f63 <+32>:    jg     0x400f6a <phase_3+39>
   0x0000000000400f65 <+34>:    callq  0x40143a <explode_bomb>
   0x0000000000400f6a <+39>:    cmpl   $0x7,0x8(%rsp)
   0x0000000000400f6f <+44>:    ja     0x400fad <phase_3+106>
   0x0000000000400f71 <+46>:    mov    0x8(%rsp),%eax
   0x0000000000400f75 <+50>:    jmpq   *0x402470(,%rax,8)
   0x0000000000400f7c <+57>:    mov    $0xcf,%eax
   0x0000000000400f81 <+62>:    jmp    0x400fbe <phase_3+123>
   0x0000000000400f83 <+64>:    mov    $0x2c3,%eax
   0x0000000000400f88 <+69>:    jmp    0x400fbe <phase_3+123>
   0x0000000000400f8a <+71>:    mov    $0x100,%eax
   0x0000000000400f8f <+76>:    jmp    0x400fbe <phase_3+123>
   0x0000000000400f91 <+78>:    mov    $0x185,%eax
   0x0000000000400f96 <+83>:    jmp    0x400fbe <phase_3+123>
   0x0000000000400f98 <+85>:    mov    $0xce,%eax
   0x0000000000400f9d <+90>:    jmp    0x400fbe <phase_3+123>
   0x0000000000400f9f <+92>:    mov    $0x2aa,%eax
   0x0000000000400fa4 <+97>:    jmp    0x400fbe <phase_3+123>
   0x0000000000400fa6 <+99>:    mov    $0x147,%eax
   0x0000000000400fab <+104>:   jmp    0x400fbe <phase_3+123>
   0x0000000000400fad <+106>:   callq  0x40143a <explode_bomb>
   0x0000000000400fb2 <+111>:   mov    $0x0,%eax
   0x0000000000400fb7 <+116>:   jmp    0x400fbe <phase_3+123>
   0x0000000000400fb9 <+118>:   mov    $0x137,%eax
   0x0000000000400fbe <+123>:   cmp    0xc(%rsp),%eax
   0x0000000000400fc2 <+127>:   je     0x400fc9 <phase_3+134>
   0x0000000000400fc4 <+129>:   callq  0x40143a <explode_bomb>
   0x0000000000400fc9 <+134>:   add    $0x18,%rsp
   0x0000000000400fcd <+138>:   retq   
```

首先直接跑一下这个函数

断点打到89行 phase3的地方

![image-20240110103453844](https://telegraph-image-6yo.pages.dev/file/9d4aa75ee12a6a13a4999.png)

进入phase_3之后，第一个函数是_isoc99_sscanf@plt 

可以看到这个函数的参数是两个十进制整数`%d %d`

**猜测phase3就是两个整数，用 12 21 两个数字作为输入测试一下**

```assembly
__isoc99_sscanf (s=0x603820 <input_strings+160> "12 21", format=0x4025cf "%d %d") at isoc99_sscanf.c:26
```

**奇怪的gdb知识又增加了**

使用run开启一个debug的时候，可以对应的用一个stop关闭他。

使用`finish`运行完这个函数

![image-20240110105918128](https://telegraph-image-6yo.pages.dev/file/b3cc4cbef10b70ed0406b.png)

这个意味着这个函数的返回值是2么？

看看程序计数器里面下面五行指令是什么

其实就是上面的phase_3的汇编代码中isoc99_sscanf函数结束之后

**其实这个喊数据就是判断你输入的是不是两个整数，不是的话炸弹直接爆炸**

`display/5i $pc`

```
(gdb) display/5i $pc
1: x/5i $pc
=> 0x400f60 <phase_3+29>:       cmp    $0x1,%eax
   0x400f63 <phase_3+32>:       jg     0x400f6a <phase_3+39>
   0x400f65 <phase_3+34>:       callq  0x40143a <explode_bomb>
   0x400f6a <phase_3+39>:       cmpl   $0x7,0x8(%rsp)
   0x400f6f <phase_3+44>:       ja     0x400fad <phase_3+106>
```

1. `0x400f60 <phase_3+29>: cmp $0x1, %eax`：将寄存器 `%eax` 的值与立即数 `0x1` 进行比较。
2. `0x400f63 <phase_3+32>: jg 0x400f6a <phase_3+39>`：如果前面的比较结果为大于（greater），则跳转到地址 `0x400f6a` 处执行。
3. `0x400f65 <phase_3+34>: callq 0x40143a <explode_bomb>`：调用函数 `explode_bomb`。**(BOOM!)**
4. `0x400f6a <phase_3+39>: cmpl $0x7, 0x8(%rsp)`：将存储在 `%rsp` 寄存器偏移量为 `0x8` 处的内存值与立即数 `0x7` 进行比较。
5. `0x400f6f <phase_3+44>: ja 0x400fad <phase_3+106>`：如果前面的比较结果为大于（above），则跳转到地址 `0x400fad` 处执行。**(BOOM!)**

**NOTE:** eax指向的应该就是isoc99_sscanf函数的返回值吧，打印出来看看

![image-20240110112840600](https://telegraph-image-6yo.pages.dev/file/2660b49fbfdfa72088aee.png)

exactly

那么如果不炸的话 ，我们看看%rsp + 0x8位置到底是什么东西

![image-20240110114310500](https://telegraph-image-6yo.pages.dev/file/0af7b093b4aba77c33285.png)

**正是我们之前用作测试的两个数的第一个数！**

也就是说第一个数要小于7 不然会引发爆炸；

**NOTE:这里是`ja`是无符号的比较**

也就是说如果第一个数是-1的话 计算机认为-1 > 7

第一个数不爆炸的话就会走到`0x400f71`

![image-20240110114649487](https://telegraph-image-6yo.pages.dev/file/1dac7f5ecc565f900f403.png)

这一行把这个值给到了%eax

之后呢执行了一个跳转

```assembly
   0x0000000000400f71 <+46>:    mov    0x8(%rsp),%eax
   0x0000000000400f75 <+50>:    jmpq   *0x402470(,%rax,8)
```

我们直接重新开始gdb吧，直接把断点设置在jmpq的位置

​				**新的gdb知识又增加了**

​				使用`delete`可以删除所有的断点

 ![image-20240110144125460](C:/Users/xidian/AppData/Roaming/Typora/typora-user-images/image-20240110144125460.png)

向下执行一步看看跳到哪里去了

![image-20240110144411725](C:/Users/xidian/AppData/Roaming/Typora/typora-user-images/image-20240110144411725.png)

​		**新的gdb知识又增加了**

​		使用`p 0x2aa`可以将二进制转化为16进制

上面接下来又跳转到了phase_3 +123 执行一下看看

![image-20240110144836853](C:/Users/xidian/AppData/Roaming/Typora/typora-user-images/image-20240110144836853.png)

比较（rsp + 0xc）指向的元素大小和 元素0x2aa  不相等的话BOOM！

看看里面存的啥

![image-20240110145338209](C:/Users/xidian/AppData/Roaming/Typora/typora-user-images/image-20240110145338209.png)

存的正是之前输入的第二个值，由于不等于682，炸弹即将爆炸

将ans.txt中的输入值修改为

```
6 682
```

![image-20240110145705465](C:/Users/xidian/AppData/Roaming/Typora/typora-user-images/image-20240110145705465.png)

成功拆除第三个炸弹

在根据上面的过程

发现从0-6中选一个作为第一个值，后面都会有一个对应的不同的值，只要输入对应值，第三个炸弹就被破解了！

类似这种代码形式

![image-20240110173131646](https://telegraph-image-6yo.pages.dev/file/6065a1dcb8a41fd03ef38.png)

## 进入phase_4

```assembly
   0x000000000040100c <+0>:     sub    $0x18,%rsp
   0x0000000000401010 <+4>:     lea    0xc(%rsp),%rcx
   0x0000000000401015 <+9>:     lea    0x8(%rsp),%rdx
   0x000000000040101a <+14>:    mov    $0x4025cf,%esi
   0x000000000040101f <+19>:    mov    $0x0,%eax
   0x0000000000401024 <+24>:    callq  0x400bf0 <__isoc99_sscanf@plt>
   0x0000000000401029 <+29>:    cmp    $0x2,%eax
   0x000000000040102c <+32>:    jne    0x401035 <phase_4+41>
   0x000000000040102e <+34>:    cmpl   $0xe,0x8(%rsp)
   0x0000000000401033 <+39>:    jbe    0x40103a <phase_4+46>
   0x0000000000401035 <+41>:    callq  0x40143a <explode_bomb>
   0x000000000040103a <+46>:    mov    $0xe,%edx
   0x000000000040103f <+51>:    mov    $0x0,%esi
   0x0000000000401044 <+56>:    mov    0x8(%rsp),%edi
   0x0000000000401048 <+60>:    callq  0x400fce <func4>
   0x000000000040104d <+65>:    test   %eax,%eax
   0x000000000040104f <+67>:    jne    0x401058 <phase_4+76>
   0x0000000000401051 <+69>:    cmpl   $0x0,0xc(%rsp)
   0x0000000000401056 <+74>:    je     0x40105d <phase_4+81>
   0x0000000000401058 <+76>:    callq  0x40143a <explode_bomb>
   0x000000000040105d <+81>:    add    $0x18,%rsp
   0x0000000000401061 <+85>:    retq   
```

发现又是两个整数 使用222 333做测试

![image-20240110170421152](https://telegraph-image-6yo.pages.dev/file/f39938502f17efee4a588.png)

新的gdb知识又增加了

使用指令`layout asm`可以实时的展示目前汇编代码的执行

如图所示下面白色边框的就是layout窗口

![image-20240110170908519](https://telegraph-image-6yo.pages.dev/file/58168513d9547b6e896e5.png)

函数`__isoc99_sscanf`返回值为2

![image-20240110171528874](https://telegraph-image-6yo.pages.dev/file/be72161a4c93474712caa.png)

```assembly
   0x0000000000401024 <+24>:    callq  0x400bf0 <__isoc99_sscanf@plt>
   0x0000000000401029 <+29>:    cmp    $0x2,%eax
   0x000000000040102c <+32>:    jne    0x401035 <phase_4+41> //BOOM!(不相等)
```

可见返回值2不会引发炸弹爆炸

继续往下走

比较 0xe : *（$rsp + 0x8)

`jbe`无符号小于等于

if (*(rsp + 0x8) <= 14) 跳转，炸弹不会爆炸

显然这里需要第1个参数 <= 14;

![image-20240110172700342](https://telegraph-image-6yo.pages.dev/file/47f9a308c80960a6a5abb.png)

重新设置ans.txt 为13 333

再run一次

正常跳转到phase_4 + 46 避免了BOOM！

![](https://telegraph-image-6yo.pages.dev/file/c7f13c79ca4a0ccb4c0c2.png)

**新的gdb知识增加了**

### 使用`p $eax`可以打印出寄存器%eax里面的值

<func4>调用结束之后

![image-20240110181315589](https://telegraph-image-6yo.pages.dev/file/66698fa234a0d58fb96a6.png)

*（$rsp +0xc) 等于0的时候才不会引发爆炸

为什么是这里存的是33呢？换一组数据试试     12，  456

重新调试，断点设置在 callq<fun4> 那里差不多

![image-20240110181704960](https://telegraph-image-6yo.pages.dev/file/31c65879bcc633c2258fc.png)

`jne`非零就跳转到炸弹了

所以%eax的值必须是0才行，什么时候才能是0呢？

![image-20240110183559134](https://telegraph-image-6yo.pages.dev/file/c9b9bf18393a8c689f74e.png)

![image-20240110183656577](https://telegraph-image-6yo.pages.dev/file/b6ef41f72d89d6a07a62c.png)

fun4里面传入了几个值

edx = 14

esi = 0

edi = 传入的第一个参数

```assembly
   0x0000000000400fce <+0>:     sub    $0x8,%rsp
   0x0000000000400fd2 <+4>:     mov    %edx,%eax					eax = 14
   0x0000000000400fd4 <+6>:     sub    %esi,%eax					eax = 14  
   0x0000000000400fd6 <+8>:     mov    %eax,%ecx					ecx = 14
   0x0000000000400fd8 <+10>:    shr    $0x1f,%ecx					ecx = 0
   0x0000000000400fdb <+13>:    add    %ecx,%eax					eax = 2
   0x0000000000400fdd <+15>:    sar    %eax							eax = 1 算数右移一位
   0x0000000000400fdf <+17>:    lea    (%rax,%rsi,1),%ecx			ecx = 1
   0x0000000000400fe2 <+20>:    cmp    %edi,%ecx					比较传入的参数和 1 的大小
   0x0000000000400fe4 <+22>:    jle    0x400ff2 <func4+36>			1 <= 传入的参数的话，跳转36行
   0x0000000000400fe6 <+24>:    lea    -0x1(%rcx),%edx
   0x0000000000400fe9 <+27>:    callq  0x400fce <func4>
   0x0000000000400fee <+32>:    add    %eax,%eax
   0x0000000000400ff0 <+34>:    jmp    0x401007 <func4+57>
   0x0000000000400ff2 <+36>:    mov    $0x0,%eax					eax = 0
   0x0000000000400ff7 <+41>:    cmp    %edi,%ecx					
   0x0000000000400ff9 <+43>:    jge    0x401007 <func4+57>			1 >= 传入的参数的话 到57行
   0x0000000000400ffb <+45>:    lea    0x1(%rcx),%esi
   0x0000000000400ffe <+48>:    callq  0x400fce <func4>
   0x0000000000401003 <+53>:    lea    0x1(%rax,%rax,1),%eax
   0x0000000000401007 <+57>:    add    $0x8,%rsp
   0x000000000040100b <+61>:    retq
```

可见第一个参数等于1是可行的

实验一下发现确实可以

<img src="https://telegraph-image-6yo.pages.dev/file/2a1a3e44824e65570918f.png" alt="image-20240110185500795" style="zoom:50%;" />

炸弹没有爆炸

```assembly
   0x0000000000400fce <+0>:     sub    $0x8,%rsp
   0x0000000000400fd2 <+4>:     mov    %edx,%eax					eax = 14                     eax = 0
   0x0000000000400fd4 <+6>:     sub    %esi,%eax					eax = 14  
   0x0000000000400fd6 <+8>:     mov    %eax,%ecx					ecx = 14
   0x0000000000400fd8 <+10>:    shr    $0x1f,%ecx					ecx = 0
   0x0000000000400fdb <+13>:    add    %ecx,%eax					eax = 2					   eax = 0
   0x0000000000400fdd <+15>:    sar    %eax							eax = 1 算数右移一位
   0x0000000000400fdf <+17>:    lea    (%rax,%rsi,1),%ecx			ecx = 1					    ecx = 0
   0x0000000000400fe2 <+20>:    cmp    %edi,%ecx					比较传入的参数和 1 的大小      比较传入的参数和 0 的大小
   0x0000000000400fe4 <+22>:    jle    0x400ff2 <func4+36>			1 > 传入的参数的话 继续			0 <= 传入的参数的话，跳转36行
   0x0000000000400fe6 <+24>:    lea    -0x1(%rcx),%edx               edx = 0；
   0x0000000000400fe9 <+27>:    callq  0x400fce <func4>				再次调用fun4
   0x0000000000400fee <+32>:    add    %eax,%eax
   0x0000000000400ff0 <+34>:    jmp    0x401007 <func4+57>
   0x0000000000400ff2 <+36>:    mov    $0x0,%eax		                                          0 >= 传入的参数的话 到57行			
   0x0000000000400ff7 <+41>:    cmp    %edi,%ecx					
   0x0000000000400ff9 <+43>:    jge    0x401007 <func4+57>			
   0x0000000000400ffb <+45>:    lea    0x1(%rcx),%esi
   0x0000000000400ffe <+48>:    callq  0x400fce <func4>
   0x0000000000401003 <+53>:    lea    0x1(%rax,%rax,1),%eax 
   0x0000000000401007 <+57>:    add    $0x8,%rsp                      
   0x000000000040100b <+61>:    retq
```

由上面的分析可知，第一个参数要小于14 ，进入func4后进一步判断了这个参数，只能是0 或者 1；

下面以第一个输入为0为例，继续测试

![image-20240110190947547](https://telegraph-image-6yo.pages.dev/file/b669f29a4c8edbcdb17ab.png)

第二个参数为什么456只打印出45？（经过测试发现是虚拟机上面的vscode 修改txt文件的时候不知道为什么会丢失最后一个字符）

使用vim修改就好了

由于和立即数0x0不想等，继续运行会触发爆炸

那么显然第四个就是两个数字[0,0] ，或者是[1,0]都可以拆除炸弹4

![image-20240110192251251](https://telegraph-image-6yo.pages.dev/file/be659829dd70c74694cfe.png)

![image-20240110192301278](https://telegraph-image-6yo.pages.dev/file/946310ed6f4a8099fab4f.png)

## 进入phase_5

### 第一阶段确定字符串长度

![image-20240111145926723](https://telegraph-image-6yo.pages.dev/file/4ccdd34ed3fe4a5be510a.png)

在ans.txt中我写入了一个长度为5的字符串

执行完`string_length`函数之后发现eax中刚好保存的就是 5

看到cmp 就知道了字符串的长度为6

![](https://telegraph-image-6yo.pages.dev/file/ca7cb17a8ca9bce58b599.png)

将eax设置成0然后继续

![image-20240111151545212](https://telegraph-image-6yo.pages.dev/file/c869fe54d666c4640bde0.png)

### 第二阶段找到隐藏字符串

回到42行

![image-20240111153628932](https://telegraph-image-6yo.pages.dev/file/9d61964571c7bf5407727.png)

前面rdi被赋值给了rdx

查看一下rdi里面到底是什么也就知道了rdx里面是啥了

可见里面存储的就是我们输入的字符串的首字母地址

![image-20240111153753887](https://telegraph-image-6yo.pages.dev/file/65ecdfc0504461c4c666e.png)

继续走

吧rbx中的第一个字符给了ecx

![image-20240111155712038](https://telegraph-image-6yo.pages.dev/file/007c364a8e48ecde4d127.png)

n对应的10进制为110 ,对应的二进制为1101110

代码`and $0xf, %edx`是要取n对应的后四位 

1110刚刚好就是14

![image-20240111163601910](https://telegraph-image-6yo.pages.dev/file/38dda2caaac2d492dc528.png)

找到隐藏字符串

![image-20240111163930683](https://telegraph-image-6yo.pages.dev/file/cb1179565c8015ff80793.png)

```
maduiersnfotvbylSo you think you can stop the bomb with ctrl-c, do you?
```

这下知道为什么要取最后四个字节了吧，因为隐藏字符串就16个，后面的那个可以忽略，那个是其他程序调用的字符串，我们之前运行程序的时候使用ctrl-c就会触发

`movzbl 0x4024b0(%rdx), %rdx`

这段代码就是将0x4024b0 + *($rdx) 位置的值给rdx

上面第14个字符是y

![image-20240111165517656](https://telegraph-image-6yo.pages.dev/file/45fef324c41ee59e12b2b.png)

```
mov    %dl,0x10(%rsp,%rax,1)      
```

%dl 是rdx 的最后一个字节 就是`y`     %rax里面储存的是0 rsp里面储存的是输入的字符串的第一个字符

![image-20240111171325065](https://telegraph-image-6yo.pages.dev/file/bb3bafa3d701ca6c042c0.png)

%rsp + 10 位置储存的就是`y`

### 发现了一个美妙的循环

显然这一段就是一个for循环 循环6次

每次取输入字符串中的一个字符的末尾四位 对应隐藏字符串的值 储存到rsp + 10之后的地址中

```assembly
   │0x40108b <phase_5+41>   movzbl (%rbx,%rax,1),%ecx                                                                                                           │
   │0x40108f <phase_5+45>   mov    %cl,(%rsp)                                                                                                                   │
   │0x401092 <phase_5+48>   mov    (%rsp),%rdx                                                                                                                  │
   │0x401096 <phase_5+52>   and    $0xf,%edx                                                                                                                    │
   │0x401099 <phase_5+55>   movzbl 0x4024b0(%rdx),%edx                                                                                                          │
   │0x4010a0 <phase_5+62>   mov    %dl,0x10(%rsp,%rax,1)                                                                                                        │
   │0x4010a4 <phase_5+66>   add    $0x1,%rax                                                                                                                    │
   │0x4010a8 <phase_5+70>   cmp    $0x6,%rax                                                                                                                    │
  >│0x4010ac <phase_5+74>   jne    0x40108b <phase_5+41>   
```

```
string rsp_10;
string s = "nihaoa";
*hideString = 0x4024b0;
for(int i = 0; i < 6; ++i){
	count = s[i] 的后四位;
	取得hideString[count];
	rsp_10 + hideString[count]; 将字符储存到$rsp + 10 的位置
}
```

 

我们运行出去这个循环

看到一个奇妙的地址，我们进去看看是什么吧！

## "flyers"

![image-20240111174459386](https://telegraph-image-6yo.pages.dev/file/9e9038b4a412c3b7491ba.png)

找到"nihaoa" 对应的字符串储存的位置

![image-20240111175251868](https://telegraph-image-6yo.pages.dev/file/2107fbc8843bfada70f27.png)

调用了函数<string not equal>

发现炸弹爆炸

![image-20240111175605594](https://telegraph-image-6yo.pages.dev/file/0f1be18aca0d3801d63ce.png)

**猜测必须让映射后的字符串是flyers**

```
maduiersnfotvbylSo you think you can stop the bomb with ctrl-c, do you?
```

flyers的位置分别是 9 15 14 5 6 7

那我们只要找到字符串后四位是

1001   1111   1110   0101   0110   0111 的字符就好

i           o          n         e         f           g

ionefg是其中一个满足条件的字符串

修改ans.txt重新调试

![image-20240111194142854](https://telegraph-image-6yo.pages.dev/file/00f4d5e305827cbca0736.png)

一直运行发现第五个炸弹被拆除

![image-20240111194509448](https://telegraph-image-6yo.pages.dev/file/f3014d5cbc39f5544a171.png)

## phase_6

phase_6的汇编代码有点长，我们一点点看

```assembly
   0x00000000004010f4 <+0>:     push   %r14
   0x00000000004010f6 <+2>:     push   %r13
   0x00000000004010f8 <+4>:     push   %r12
   0x00000000004010fa <+6>:     push   %rbp
   0x00000000004010fb <+7>:     push   %rbx
   0x00000000004010fc <+8>:     sub    $0x50,%rsp
   0x0000000000401100 <+12>:    mov    %rsp,%r13		# r13 = rsp
   0x0000000000401103 <+15>:    mov    %rsp,%rsi		# rsi = rsp
   0x0000000000401106 <+18>:    callq  0x40145c <read_six_numbers>
   0x000000000040110b <+23>:    mov    %rsp,%r14		# r14 = rsp
   0x000000000040110e <+26>:    mov    $0x0,%r12d		# r12d = 0
   0x0000000000401114 <+32>:    mov    %r13,%rbp		# rbp = rsp
   0x0000000000401117 <+35>:    mov    0x0(%r13),%eax    # eax  = rsp
   0x000000000040111b <+39>:    sub    $0x1,%eax		# 对eax指向的值 - 1
   0x000000000040111e <+42>:    cmp    $0x5,%eax         # 和5比较  （上面两个合起来的意思就是eax指向的值必须小于等于6）
   0x0000000000401121 <+45>:    jbe    0x401128 <phase_6+52> 
   0x0000000000401123 <+47>:    callq  0x40143a <explode_bomb> 		# eax > 6 BOOM！
   0x0000000000401128 <+52>:    add    $0x1,%r12d		# r12d++
   0x000000000040112c <+56>:    cmp    $0x6,%r12d		# 将12d 和0x6进行比较
   0x0000000000401130 <+60>:    je     0x401153 <phase_6+95>
   0x0000000000401132 <+62>:    mov    %r12d,%ebx        # ebx  = r12d
   0x0000000000401135 <+65>:    movslq %ebx,%rax		# rax = r12d
   0x0000000000401138 <+68>:    mov    (%rsp,%rax,4),%eax # eax = rsp + 4 * rax  （显然这里rax是Index）
   0x000000000040113b <+71>:    cmp    %eax,0x0(%rbp)     # 比较当前Index的值 要是和第一个值相等 爆炸
   0x000000000040113e <+74>:    jne    0x401145 <phase_6+81>
   0x0000000000401140 <+76>:    callq  0x40143a <explode_bomb>
   0x0000000000401145 <+81>:    add    $0x1,%ebx          #ebx ++
   0x0000000000401148 <+84>:    cmp    $0x5,%ebx          # 和5比较 <= 5的话继续循环
   0x000000000040114b <+87>:    jle    0x401135 <phase_6+65>
   0x40114d <phase_6+89>   add    $0x4,%r13     #移动到下一个值                                                   
   0x401151 <phase_6+93>   jmp    0x401114 <phase_6+32>    #跳转到32
```

### Part1

看见调用函数<read_six_numbers> 猜测输入是6个数字，修改ans.txt第六行为111 222 333 444 555 666

<img src="https://telegraph-image-6yo.pages.dev/file/dc0283690a0c2114d9362.png" alt="image-20240113134930844" style="zoom:50%;" />

显然rsp中储存的是是第一个输入的数字

第一个数字必须<=6 否则就会爆炸，更改ans.txt 为 1 222 333 444 555

继续允许part1 部分汇编代码，发现后面是一个双层循环结构，要求后面的数字不能前面的数字相等

至此第一部分结束

1. 一共六个数字
2. 所有数字 <= 6
3. 后面数字不可以和前面的数字相同

```
for（int i = 0; i < 6; ++i){
	nums[i] <= 6;
	for(int j = i + 1; j < 6; ++j){
		nums[j] != nums[i];
	}
}
```

更改一下ans.txt

**3 2 1 4 5 6**

### Part2

下面是跳出上面大循环的代码

```assembly
   0x0000000000401151 <+93>:    jmp    0x401114 <phase_6+32>
   0x0000000000401153 <+95>:    lea    0x18(%rsp),%rsi		#使用lea指令 将rsi中储存上rsp +0x18 地址
   0x0000000000401158 <+100>:   mov    %r14,%rax			#使用p /x $r14 输出16进制地址 发现和 p /x $rsp 相同
   0x000000000040115b <+103>:   mov    $0x7,%ecx 			#ecx = 7
   0x0000000000401160 <+108>:   mov    %ecx,%edx			#edx = 7
   0x0000000000401162 <+110>:   sub    (%rax),%edx           # edx = edx - *rax (rax 指向栈底元素，也就是第一个数字)
   0x0000000000401164 <+112>:   mov    %edx,(%rax)			# rax = edx 更新栈底元素
   0x0000000000401166 <+114>:   add    $0x4,%rax			# 指向下一个元素
   0x000000000040116a <+118>:   cmp    %rsi,%rax             #比较当前元素 和 rsp + 0x18的元素
   0x000000000040116d <+121>:   jne    0x401160 <phase_6+108> # 不想等的话继续循环
   0x000000000040116f <+123>:   mov    $0x0,%esi			# esi = 0； 相等的话设置esi = 0
```

rsi 中储存的是0

![image-20240113151133948](https://telegraph-image-6yo.pages.dev/file/4c9abeb73f740fe014c43.png)

上面的汇编代码等价于

```
while（7 - nums[i] !=  *(rsi) ){
	nums[i] = 7 - nums[i]
}
```

**奇怪了，六个数字都小于等于6 如何跳出循环呢？**

![image-20240113151356169](https://telegraph-image-6yo.pages.dev/file/89a030db9352e2e01c730.png)

原来是因为六个数字从rsp开始储存的，然后`$rsp +0x14`刚好存的手机最后一个数字，到达rsi的位置刚刚好rax 和 rsi指向同一个值，导致循环跳出

**NOTE：这一part将每一个数字都更新为nums[i] = 7 - nums[i]**

**更新后的数据为 4 5 6 3 2 1**

### Part3

```assembly
   0x000000000040116d <+121>:   jne    0x401160 <phase_6+108>		
   0x000000000040116f <+123>:   mov    $0x0,%esi					# esi  = 0
   0x0000000000401174 <+128>:   jmp    0x401197 <phase_6+163>
   0x0000000000401176 <+130>:   mov    0x8(%rdx),%rdx				#移动到链表的下一个值
   0x000000000040117a <+134>:   add    $0x1,%eax
   0x000000000040117d <+137>:   cmp    %ecx,%eax
   0x000000000040117f <+139>:   jne    0x401176 <phase_6+130>
   0x0000000000401181 <+141>:   jmp    0x401188 <phase_6+148>
   0x0000000000401183 <+143>:   mov    $0x6032d0,%edx
   0x0000000000401188 <+148>:   mov    %rdx,0x20(%rsp,%rsi,2)
   0x000000000040118d <+153>:   add    $0x4,%rsi
   0x0000000000401191 <+157>:   cmp    $0x18,%rsi
   0x0000000000401195 <+161>:   je     0x4011ab <phase_6+183>
   0x0000000000401197 <+163>:   mov    (%rsp,%rsi,1),%ecx      		# ecx = rsp  + 0 * 1 指向的是当前栈底的元素
   0x000000000040119a <+166>:   cmp    $0x1,%ecx					#比较1 ： ecx
   0x000000000040119d <+169>:   jle    0x401183 <phase_6+143>		# ecx 的值 <= 1 跳转到143
   0x000000000040119f <+171>:   mov    $0x1,%eax					# eax = 1
   0x00000000004011a4 <+176>:   mov    $0x6032d0,%edx				# edx 
   0x00000000004011a9 <+181>:   jmp    0x401176 <phase_6+130>
```

梳理汇编代码的时候发现一个**JOJO的奇妙地址**

打开看看

看到node 猜测是一个链表

第一列是自己的地址，第二列不知道，第三列不知道，第四列是下一个节点的地址（next）

```
sizeof（node) = 16字节
```

猜测一手

```
node{
	int ***;
	int val; //(1,2,3,4,5,6) //4字节
	node* next; // 8 字节
}
```

![image-20240113153530101](https://telegraph-image-6yo.pages.dev/file/d23c51337e6d3cc96c8e6.png)

<img src="https://telegraph-image-6yo.pages.dev/file/90f36c32b4f4983b3d586.png" alt="image-20240113154055196" style="zoom:150%;" />

```
1 : 332
2 : 168
3 : 924
4 : 691
5 : 477
6 : 443
```

那么这里的意思就是给edx 传了一个链表

![image-20240113154433778](https://telegraph-image-6yo.pages.dev/file/69384af5d8ad5df3b6c98.png)

我预感到要有一场酣畅淋漓的decode了，gdb 启动！

<img src="https://telegraph-image-6yo.pages.dev/file/e9dfb93170cbea4312e45.png" alt="image-20240113155441602" style="zoom:33%;" />

![image-20240113155626379](https://telegraph-image-6yo.pages.dev/file/de307479616d1ac66c19f.png)

给edx指向一个链表之后，开始我们的第一次循环

![image-20240113155756643](https://telegraph-image-6yo.pages.dev/file/620e84b4a08647062927a.png)

```assembly
0x0000000000401176 <+130>:   mov    0x8(%rdx),%rdx				#移动到链表的下一个值
   0x000000000040117a <+134>:   add    $0x1,%eax				eax = eax + 1
   0x000000000040117d <+137>:   cmp    %ecx,%eax				
   0x000000000040117f <+139>:   jne    0x401176 <phase_6+130>		eax != ecx 继续移动
   0x0000000000401181 <+141>:   jmp    0x401188 <phase_6+148>		
   0x0000000000401183 <+143>:   mov    $0x6032d0,%edx
   0x0000000000401188 <+148>:   mov    %rdx,0x20(%rsp,%rsi,2)		# rsp + 20 + 2 * rsi 相当与一个新的空间
   0x000000000040118d <+153>:   add    $0x4,%rsi					#rsi  = rsi  + 4
   0x0000000000401191 <+157>:   cmp    $0x18,%rsi					#比较 rsi 和24的大小
   0x0000000000401195 <+161>:   je     0x4011ab <phase_6+183>		#相等的话跳出循环
   0x0000000000401197 <+163>:   mov    (%rsp,%rsi,1),%ecx      		# ecx = rsp  + 0 * 1 指向的是当前栈底的元素
   0x000000000040119a <+166>:   cmp    $0x1,%ecx					#比较1 ： ecx
   0x000000000040119d <+169>:   jle    0x401183 <phase_6+143>		# ecx 的值 <= 1 跳转到143
   0x000000000040119f <+171>:   mov    $0x1,%eax					# eax = 1
   0x00000000004011a4 <+176>:   mov    $0x6032d0,%edx				# edx 
   0x00000000004011a9 <+181>:   jmp    0x401176 <phase_6+130>
```

执行这一条之后 `0x0000000000401176 <+130>:   mov    0x8(%rdx),%rdx	`

rdx指向的是节点里面next的值

开始之前NOTE一下

ecx 代表的是part处理过的六个数组

rdx 代表的是一个含有六个节点的链表

eax 是一个偏移量

rsp + 20 是一个新的空间的首地址

<img src="https://telegraph-image-6yo.pages.dev/file/90d41db1457cdb4a1d870.png" alt="image-20240113163829862" style="zoom:33%;" />

初始的情况大概就是这样

第一次循环ecx = 4 

这一个小循环，让rdx指向了第4个节点

![image-20240113164046668](https://telegraph-image-6yo.pages.dev/file/8337fc5b1327d4e681504.png)

0x603300指向是第四个节点的地址

跳转到148 

<img src="https://telegraph-image-6yo.pages.dev/file/080ce47eb66b663a1a684.png" alt="image-20240113164626567" style="zoom:33%;" />

将对应地址存储下来

然后就是循环，跳出循环的时候就是这样的

<img src="https://telegraph-image-6yo.pages.dev/file/f1c417d2e3726262b3f15.png" alt="image-20240113164858392" style="zoom:33%;" />

验证一下对不对

![image-20240113165447715](https://telegraph-image-6yo.pages.dev/file/3e1983edc3619a2a20fb5.png)

对了！，如果你做到这里的话，太强悍了宝贝！！你就是代码之神！！！

不要放松一鼓作气，就快找到最终的秘密了！！！！

继续看接下来的代码

```assembly
   0x00000000004011ab <+183>:   mov    0x20(%rsp),%rbx		#将rbx指向 rsp + 0x20 node4
   0x00000000004011b0 <+188>:   lea    0x28(%rsp),%rax		# rax 指向 rsp + 0x28
   0x00000000004011b5 <+193>:   lea    0x50(%rsp),%rsi		# rsi 指向 rsp + 0x50 
   #（新空间最后一个node的指针为rsp + 0x48)   
   0x00000000004011ba <+198>:   mov    %rbx,%rcx			# rcx = rbx		node4
   0x00000000004011bd <+201>:   mov    (%rax),%rdx			# rdx  = rax    node5
   0x00000000004011c0 <+204>:   mov    %rdx,0x8(%rcx)		#  rcx + 0x8 = rdx
   0x00000000004011c4 <+208>:   add    $0x8,%rax			#rax = rax + 0x8 上移
   0x00000000004011c8 <+212>:   cmp    %rsi,%rax			#比较rax 和 rsi
   0x00000000004011cb <+215>:   je     0x4011d2 <phase_6+222> #相等的话跳到222 (相等的话说明遍历完成新空间的节点了)
   0x00000000004011cd <+217>:   mov    %rdx,%rcx			#rcx = rdx 上移
   0x00000000004011d0 <+220>:   jmp    0x4011bd <phase_6+201>  #返回201
```

解释一下上面一小段代码 其实就是对新空间的节点进行链接

node4.next = node5

<img src="https://telegraph-image-6yo.pages.dev/file/e7125fc87e239909c170c.png" alt="image-20240113173439098" style="zoom:33%;" />

### Part4

完成上面的代码rdx指向

<img src="https://telegraph-image-6yo.pages.dev/file/5558e213bee18d654e128.png" alt="image-20240113193805509" style="zoom:33%;" />

执行完成上面的代码，我们就完成了连接node4->node5->node6->node3->node2->node1

<img src="https://telegraph-image-6yo.pages.dev/file/90f36c32b4f4983b3d586.png" alt="image-20240113154055196" style="zoom:150%;" />

```assembly
   0x00000000004011d2 <+222>:   movq   $0x0,0x8(%rdx)   	#rdx 指向node1， rdx +0x8 是node1->next 也就是node2
   0x00000000004011da <+230>:   mov    $0x5,%ebp			# ebp = 5
   0x00000000004011df <+235>:   mov    0x8(%rbx),%rax		# rax = rbx + 0x8 取得node5
   0x00000000004011e3 <+239>:   mov    (%rax),%eax			#存在eax = 477 node5->xxx
   0x00000000004011e5 <+241>:   cmp    %eax,(%rbx)			#比较
   0x00000000004011e7 <+243>:   jge    0x4011ee <phase_6+250> #if node4->xxx < node5->xxx BOOM
   0x00000000004011e9 <+245>:   callq  0x40143a <explode_bomb>
   0x00000000004011ee <+250>:   mov    0x8(%rbx),%rbx
   0x00000000004011f2 <+254>:   sub    $0x1,%ebp
   0x00000000004011f5 <+257>:   jne    0x4011df <phase_6+235>
   0x00000000004011f7 <+259>:   add    $0x50,%rsp
   0x00000000004011fb <+263>:   pop    %rbx
   0x00000000004011fc <+264>:   pop    %rbp
   0x00000000004011fd <+265>:   pop    %r12
   0x00000000004011ff <+267>:   pop    %r13
   0x0000000000401201 <+269>:   pop    %r14
   0x0000000000401203 <+271>:   retq   
```

到这里就非常简单了，链表节点中的node[i]->XXX的值必须是递减的否则引发爆炸

那么顺序应该是 node3->node4->node5->node6->node1->node2

对应的更新后的数字应该是 3 4 5 6 1 2

对应的初始数字是 4 3 2 1 6 5

![image-20240113195541246](https://telegraph-image-6yo.pages.dev/file/885a9dd715c36cb9a1c43.png)

完美解决

![image-20240113195551576](https://telegraph-image-6yo.pages.dev/file/c4cb037835975386342c2.png)

至此lab2 boomLab完美结束！！！！

感谢认真的自己
