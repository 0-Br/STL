#pragma once
#include <cstring>
#include "utils.h"
#define DEFAULT_CAPACITY 1024 // 默认初始容量设为1024
#define Rank unsigned int // 秩为无符号整型

// 位图类
class Bitmap
{
private:
    unsigned char* M; // 数据区
    Rank N; // 内存占用
    Rank _sz; // 数据规模

protected:
    // 位图初始化
    // 可以使用校验环法改进效率
    void init(Rank c)
    {
        N = (c + 7) / 8;
        M = new unsigned char[N];
        memset(M, 0, N);
        _sz = 0;
    }

    // 若访问的位置已出界，则需要对位图进行扩容
    // 加倍扩容算法
    void expand(Rank k)
    {
        if (k < 8 * N) return; // 无需扩容
        Rank oldN = N;
        unsigned char *oldM = M;
        Rank oldSz = _sz; // init 会清零规模，扩容后须复原
        init(2 * k);
        memcpy(M, oldM, oldN);
        _sz = oldSz;
        delete[] oldM;
    }

public:
    // 创建一个容量为c的位图，若不指定c则默认为DEFAULT_CAPACITY
    Bitmap(Rank c = DEFAULT_CAPACITY){init(c);}
    // 析构函数
    ~Bitmap(){delete[] M; M = NULL; _sz = 0;}
    // 复制构造与赋值（持有裸内存，须深复制）
    Bitmap(const Bitmap& b){N = b.N; _sz = b._sz; M = new unsigned char[N]; memcpy(M, b.M, N);}
    Bitmap& operator= (const Bitmap& b)
    {
        if (this == &b) return *this;
        delete[] M;
        N = b.N; _sz = b._sz; M = new unsigned char[N]; memcpy(M, b.M, N);
        return *this;
    }

    // 返回位图规模
    Rank size(){return _sz;}

    // 检测k是否存在
    bool test(Rank k){expand(k); return M[k >> 3] & (0x80 >> (k & 0x07));}
    // 存入k
    void set(Rank k){expand(k); if (!test(k)) _sz++; M[k >> 3] |= (0x80 >> (k & 0x07));}
    // 删除k
    void clear(Rank k){expand(k); if (test(k)) _sz--; M[k >> 3] &= ~(0x80 >> (k & 0x07));}
};

// 散列函数
// 采用多项式法
inline Rank encode(char s[])
{
    Rank n = strlen(s);
    Rank re0 = 0;
    for (Rank i = 0; i < n; i++)
    {
        re0 = (re0 << 5) | (re0 >> 27);
        re0 += s[i];
    }
    return re0;
}

// 哈希表模板类
template <typename K, typename V>
class Hashtable
{
private:
    Entry<K, V> **ht; // 桶数组，存放词条指针
    Bitmap *removed; // 懒标记位图
    Rank M; // 桶的总数
    Rank N; // 词条的数目
    Rank L; // 懒标记数目

protected:
    Rank probe4Hit(const K& key) const
    {
        Rank r = encode(key) % M;
        while (ht[r] && (key != ht[r] -> key) || removed -> test(r)) r = (r + 1) % M;
        return r;
    }
    Rank probe4Free(const K& key)
    {
        Rank r = encode(key) % M;
        while (ht[r]) r = (r + 1) % M;
        return r;
    }

    // 重哈希扩容方法
    // 会清楚所有的懒标记
    void rehash()
    {
        Rank oldM = M;
        Entry<K, V>** oldHt = ht;
        M = 4 * N + 1; // 事实上取M为临近的素数更好
        ht = new Entry<K, V>*[M]; N = 0;
        memset(ht, 0, sizeof(Entry<K, V>*) * M);
        release(removed); removed = new Bitmap(M); L = 0;
        for (Rank i = 0; i < oldM; i++)
        {
            // 转移非空桶内的词条：put 会另行创建词条，故转移后须释放原词条
            if (oldHt[i]){put(oldHt[i] -> key, oldHt[i] -> value); release(oldHt[i]);}
        }
        delete[] oldHt;
    }

public:
    // 创建一个桶数为c的哈希表，若不指定c则默认为DEFAULT_CAPACITY
    Hashtable(Rank c = DEFAULT_CAPACITY)
    {
        M = c ? c : DEFAULT_CAPACITY; // 桶数为 0 会令散列时除零
        N = 0; L = 0;
        ht = new Entry<K, V>*[M];
        memset(ht, 0, sizeof(Entry<K, V>*) * M);
        removed = new Bitmap(M);
    }
    // 复制构造（深复制词条与懒标记，遵循与 Vector、List 一致的三法则）
    Hashtable(const Hashtable<K, V>& h)
    {
        M = h.M; N = h.N; L = h.L;
        ht = new Entry<K, V>*[M];
        for (Rank i = 0; i < M; i++) ht[i] = h.ht[i] ? new Entry<K, V>(*h.ht[i]) : NULL;
        removed = new Bitmap(M);
        for (Rank i = 0; i < M; i++) if (h.removed -> test(i)) removed -> set(i);
    }
    // 赋值运算符
    Hashtable<K, V>& operator= (const Hashtable<K, V>& h)
    {
        if (this == &h) return *this;
        for (Rank i = 0; i < M; i++) release(ht[i]);
        delete[] ht; release(removed);
        M = h.M; N = h.N; L = h.L;
        ht = new Entry<K, V>*[M];
        for (Rank i = 0; i < M; i++) ht[i] = h.ht[i] ? new Entry<K, V>(*h.ht[i]) : NULL;
        removed = new Bitmap(M);
        for (Rank i = 0; i < M; i++) if (h.removed -> test(i)) removed -> set(i);
        return *this;
    }
    // 析构函数
    ~Hashtable()
    {
        for (Rank i = 0; i < M; i++) release(ht[i]);
        delete[] ht; ht = NULL;
        release(removed); removed = NULL;
        M = N = L = 0;
    }

    // 读取方法
    // 重载[]运算符
    // 要求输入的关键码必须有效
    const V& operator[] (K key) const
    {
        Rank r = probe4Hit(key);
        return ht[r] -> value;
    }

    // 插入方法
    // 不允许关键码重复
    bool put(K key, V value)
    {
        if (ht[probe4Hit(key)]) return false; // 不插入重复的关键码
        Rank r = probe4Free(key);
        ht[r] = new Entry<K, V>(key, value); N++;
        if (removed -> test(r)){removed -> clear(r); L--;}
        if ((N + L) * 2 > M) rehash(); // 装填因子过小时，执行重哈希
        return true;
    }

    // 删除方法
    bool remove(K key)
    {
        Rank r = probe4Hit(key);
        if (!ht[r]) return false;
        release(ht[r]); ht[r] = NULL; N--;
        removed -> set(r); L++;
        if (L > 3 * N) rehash(); // 懒标记过多时，执行重哈希
        return true;
    }
};