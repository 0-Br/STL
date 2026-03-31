template <typename T>
struct Cleaner
{
    static void clean(T x){};
};
template <typename T>
struct Cleaner<T*>
{
    static void clean(T* x){if (x) delete x;}
};
template <typename T>
void release(T x){Cleaner<T>::clean(x);}

// 词条模板类
template <typename K, typename V>
struct Entry
{
    K key; // 关键码
    V value; // 值

    // 默认构造
    Entry(K k = K(), V v = V()) : key(k), value(v){};
    // 复制构造
    Entry(const Entry<K, V>& e) : key(e.key), value(e.value){};

    // 重载比较运算符
    bool operator< (const Entry<K, V>& e) const{return (key < e.key);}
    bool operator> (const Entry<K, V>& e) const{return (key > e.key);}
    bool operator<= (const Entry<K, V>& e) const{return (key <= e.key);}
    bool operator>= (const Entry<K, V>& e) const{return (key >= e.key);}
    bool operator== (const Entry<K, V>& e) const{return (key == e.key);}
    bool operator!= (const Entry<K, V>& e) const{return (key != e.key);}
};

// 元组模板类
template <typename T1, typename T2>
struct Tuple
{
    T1 elem_1;
    T2 elem_2;

    // 默认构造
    Entry(T1 e1 = T1(), T2 e2 = T2()) : elem_1(e1), elem_2(e2){};
    // 复制构造
    Entry(const Tuple<T1, T2>& t) : elem_1(t.elem_1), elem_2(t.elem_2){};

    // 重载比较运算符
    bool operator< (const Tuple<T1, T2>& t) const
    {
        if (elem_1 == t.elem_1) return (elem_2 < t.elem_2);
        else return (elem_1 < t.elem_1);
    }
    bool operator> (const Tuple<T1, T2>& t) const
    {
        if (elem_1 == t.elem_1) return (elem_2 > t.elem_2);
        else return (elem_1 > t.elem_1);
    }
    bool operator<= (const Tuple<T1, T2>& t) const
    {
        if (elem_1 == t.elem_1) return (elem_2 <= t.elem_2);
        else return (elem_1 < t.elem_1);
    }
    bool operator>= (const Tuple<T1, T2>& t) const
    {
        if (elem_1 == t.elem_1) return (elem_2 >= t.elem_2);
        else return (elem_1 > t.elem_1);
    }
    bool operator== (const Tuple<T1, T2>& t) const
    {
        if ((elem_1 == t.elem_1) && (elem_2 == t.elem_2)) return true;
        return false;
    }
    bool operator!= (const Tuple<T1, T2>& t) const
    {
        if ((elem_1 == t.elem_1) && (elem_2 == t.elem_2)) return false;
        return true;
    }
};