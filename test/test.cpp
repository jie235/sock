#include <cstdio>
#include <iostream>
#include <sched.h>
using std::cin;
using std::cout;
using std::endl;

int main() {
    int n;
    char c;
    cout << "round 1 : " << endl;
    n = fgetc(stdin);
    cout << n << endl;

    cout << "round 2 : " << endl;
    // scanf 函数不会丢弃缓冲区中的空白字符。而且，scanf
    // 函数只有遇到非空白字符时才停止读取，所以如果换行符写在后面，会引起输出效果中的格式的不对应
    scanf("%d", &n);
    cout << n << endl;
    // fgetc(stdin);
    cout << "round 3 : " << endl;
    scanf("\n%c", &c);
    cout << c << endl;

    //如果后面不放置空白字符，那么整数就是非空白字符，如果格式正确，读入数据并结束。如果不正确，将读入的字符放回缓冲区，然后抛出异常
    //如果后面放置空白字符，还需要再最后遇到一个非空白字符才会停止
    // scanf("\n%d\n", &n);
    cout << "sizeof test" << endl;
    char message[1024];
    for (int i = 0; i < 30; ++i) {
        message[i] = i;
    }

    cout << sizeof(message) << endl;

    return 0;
}