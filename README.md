## Gakio

一个非常简单的表达式求值解释器。使用算符优先文法，支持对整数和小数进行简单的加减乘除运算。实际使用如下图所示：

![使用示例](https://github.com/SerhoLiu/gakio/raw/master/doc/eg.png)

### 一些细节

> 1. 变量名：变量名必须以字母或者下划线开头，其他情况下是格式错误，变量名长度不大于128个字符
> 2. 数字：采用 C 中的双精度浮点型，对于整数运算，在结果小于 2**52 时不会出现误差，大于时将会产生误差
     支持负数，如果负数不是在首位，需要加上括号，否则会出现算术错误。

### 使用

#### For Linux or Unix
> 1. git clone git://github.com/SerhoLiu/gakio.git
> 2. cd gakio
> 3. make
> 4. ./gakio

#### For Windows

### 算符优先关系表

![算符优先关系表](https://github.com/SerhoLiu/gakio/raw/master/doc/token.png)

### 待改进

#### version 1.1.0
> 1. 扩展精度计算
> 2. 内置常用的计算函数

#### version 1.0.0
> 1. 变量名长度限制，目前的20个字符太短 (ok)
> 2. 将值的 Tagged Union 表示法改为 Tagged Pointer (值统一采用 double)
> 3. 重构词法分析器，现在的实现太过于繁琐 (除非增加语言功能，否则不会修改)
> 4. 变量表改成可动态扩展的 (ok)

### Changelog

#### version 1.1.0
> 1. 将数值统一成 double 类型
> 2. 变量表实现自动扩展
> 3. 增加 vpool 对字符串数值对象进行管理

### VERSION

__1.1.0__