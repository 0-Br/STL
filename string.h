#include <cstring>
#include "utils.h"

// 检查字符串是否匹配成功
bool is_match(char *P, char *T, int i){return (strlen(T) >= i + strlen(P));}

// 字符串匹配算法，返回最终的匹配位置
// 蛮力算法
int match_BF(char *P, char *T)
{
    int n = strlen(T), i; // 文本串长度和当前比对位置
    int m = strlen(P), j; // 模式串长度和当前比对位置
    for (i = 0; i < n - m + 1; i++) // 提前终止
    {
        for (j = 0; j < m; j++) if (T[i + j] != P[j]) break;
        if (j >= m) break;
    }
    return i;
}

// 对模式串P构建next表
int* build_Next(char *P)
{
    int m = strlen(P), j = 0;
    int *next = new int[m];
    int t = next[0] = -1; // next表首项必然为-1
    while (j < m - 1)
    {
        if ((t < 0) || (P[t] == P[j]))
        {
            if (P[++t] != P[++j]) next[j] = t; // 避免重复犯错
            else next[j] = next[t];
        }
        else t = next[t];
    }
    return next;
}

// 字符串匹配算法，返回最终的匹配位置
// KMP算法
int match_KMP(char *P, char *T)
{
    int *next = build_Next(P);
    int n = strlen(T), i = 0; // 文本串长度和当前比对位置
    int m = strlen(P), j = 0; // 模式串长度和当前比对位置
    while ((j < m) && (i < n))
    {
        if ((j < 0) || T[i] == P[j]){i++; j++;}
        else j = next[j]; // 文本串始终不回退
    }
    delete[] next;
    return i - j;
}

// 对模式串构建Bad Charactor Shift表
// 画家算法
int* build_BC(char *P)
{
    int *bc = new int[256]; // 字符集更大的时候需要考虑使用Bitmap
    for (int j = 0; j < 256; j++) bc[j] = -1;
    for (int m = strlen(P), j = 0; j < m; j++) bc[(int)P[j]] = j;
    return bc;
}

// 字符串匹配算法，返回最终的匹配位置
// BM算法
// 可使用GS表进一步优化
int match_BM(char *P, char *T)
{
    int *bc = build_BC(P);
    int n = strlen(T), i = 0;
    int m = strlen(P);
    while (n >= i + m)
    {
        int j = m - 1;
        while (P[j] == T[i + j]) if (--j < 0) break;
        if (j < 0) break;
        else i += (1 > (j - bc[(int)T[i + j]])) ? 1 : (j - bc[(int)T[i + j]]);
    }
    delete[] bc;
    return i;
}