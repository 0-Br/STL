#include "utils.h"
#define Rank unsigned int // 秩为无符号整型

// 列表的节点结构体
template <typename T>
struct listnode
{
    T data; // 数据
    listnode<T> *pred; // 指向前驱
    listnode<T> *succ; // 指向后继

    // 默认构造函数
    listnode(){};
    // 节点的初始化构造函数，默认前后均为NULL
    listnode(T e, listnode<T> *p = NULL, listnode<T> *s = NULL) : data(e), pred(p), succ(s){};

    // 插入前驱，返回一个指向插入前驱的指针
    listnode<T>* insert_p(const T& e)
    {
        listnode<T> *oldpred = pred;
        pred = new listnode<T>(e);
        pred -> pred = oldpred;
        pred -> succ = this;
        pred -> pred -> succ = pred;
        return pred;
    }
    // 插入后继，返回一个指向插入后继的指针
    listnode<T>* insert_s(const T& e)
    {
        listnode<T> *oldsucc = succ;
        succ = new listnode<T>(e);
        succ -> succ = oldsucc;
        succ -> pred = this;
        succ -> succ -> pred = succ;
        return succ;
    }
};

template <typename T>
class List
{
protected:
    Rank _size; // 数据规模
    listnode<T> *header, *trailer; // 头尾哨兵

    // 构造时的初始化函数
    void init()
    {
        _size = 0;
        header = new listnode<T>; trailer = new listnode<T>;
        header -> pred = NULL; header -> succ = trailer;
        trailer -> succ = NULL; trailer -> pred = header;
    }

    // 归并算法
    // 返回更新后的首节点
    listnode<T>* merge(listnode<T> *p, Rank n, listnode<T> *q, Rank m)
    {
        listnode<T> *pp = p -> pred;
        while ((0 < m) && (q != p)) // 归并，小者先入
        {
            if ((0 < n) && ((p -> data) <= (q -> data))){p = p -> succ; n--;}
            else{insert(remove((q = q -> succ) -> pred), p); m--;}
        }
        return pp -> succ;
    }

public:
    // 构造一个空列表
    List(){init();}
    // 构造一个列表，规模为s，全部初始化为e
    List(Rank s, T e)
    {
        init();
        for (Rank i = 0; i < s; i++) insert(e);
    }
    // 基于数组区间[lo, hi)构造一个列表
    List(const T *source, Rank lo, Rank hi)
    {
        init();
        for (Rank i = lo; i < hi; i++) insert(source[i]);
    }
    //基于列表区间[p, p + n)构造一个列表
    List(listnode<T> *p_source, Rank n)
    {
        init();
        for (Rank i = 0; i < n; i++)
        {
            insert(p_source -> data);
            p_source = p_source -> succ;
        }
    }
    //复制列表
    List(const List<T>& source)
    {
        init();
        listnode<T> *p_source = source.first();
        for (Rank i = 0; i < source._size; i++)
        {
            insert(p_source -> data);
            p_source = p_source -> succ;
        }
    }

    //析构函数
    ~List()
    {
        clear();
        delete header;
        delete trailer;
    }

    // 报告当前列表的规模
    Rank size(){return _size;}
    // 返回首位指针
    listnode<T>* first(){return header -> succ;}
    // 返回末位指针
    listnode<T>* last(){return trailer -> pred;}
    // 判断列表是否为空
    bool empty(){return !_size;}

    // 获得一个指向秩为r的元素的指针
    // 按mod取秩，效率不高，慎用
    listnode<T>* gp(Rank r)
    {
        r = r % _size;
        if (r < 0) r += _size;
        listnode<T> *p = first();
        for (Rank i = 0; i < r; i++) p = p -> succ;
        return p;
    }

    // 交换p，q指向的元素值
    // 复杂度较插入低
    void exchange(listnode<T> *p, listnode<T> *q)
    {
        T temp = q -> data;
        q -> data = p -> data;
        p -> data = temp;
    }

    // 插入操作
    // 在p处插入元素e，已有元素依次后移，返回指向新插入元素的指针
    // 默认在末尾插入
    // 常数时间较大，建议仅在增加节点时使用
    listnode<T>* insert(const T& e, listnode<T> *p){_size++; return (p -> insert_p(e));}
    listnode<T>* insert(const T& e){return insert(e, trailer);}

    // 单元素删除操作，删除p指向的元素，返回被删除的元素
    T remove(listnode<T> *p)
    {
        _size--;
        T e = p -> data;
        p -> pred -> succ = p -> succ;
        p -> succ -> pred = p -> pred;
        delete p;
        return e;
    }
    // 区间删除操作，删除[p, p + r)间的元素
    void remove(listnode<T> *p, Rank r)
    {
        _size -= r;
        listnode<T> *endp = p;
        for (Rank i = 0; i < r; i++) endp = endp -> succ;
        p -> pred -> succ = endp;
        endp -> pred = p -> pred;
        endp = endp -> pred;
        for (Rank i = 0; i < r; i++)
        {
            listnode<T> *tempp = p;
            p = p -> succ;
            delete tempp;
        }
    }

    // 清空列表
    // 返回列表清空前的规模
    Rank clear()
    {
        Rank oldsize = _size;
        remove(first(), _size);
        return oldsize;
    }

    // 重载[]运算符
    T& operator[] (Rank r){return gp(r) -> data;}
    // 重载[]运算符，只读
    const T& operator[] (Rank r) const{return gp(r) -> data;}

    // 遍历接口
    template <typename VST>
    void traverse(VST& visit)
    {
        for (listnode<T> *p = first(); p != trailer; p = p -> succ) visit(p -> data);
    }

    // 在区间[p, p + n)中查找元素target
    // 返回指向该元素的指针，默认在列表全体中查找，此情况效率较高（常数）
    // 若有多个目标，返回最后一个，若查找失败，返回NULL
    listnode<T>* find(const T& target, listnode<T> *p, Rank n)
    {
        listnode<T> *re0 = NULL;
        for (Rank i = 0; i < n; i++)
        {
            if (p -> data == target) re0 = p;
            p = p -> succ;
        }
        return re0;
    }
    listnode<T>* find(const T& target)
    {
        listnode<T> *p = last();
        for (Rank i = 0; i < _size; i++)
        {
            if (p -> data == target) return p;
            p = p -> pred;
        }
        return NULL;
    }

    // 选取区间[p, p + n)中最大的元素
    // 返回指向该处的指针，等大靠后优先
    listnode<T>* selectMax(listnode<T> *p, Rank n)
    {
        listnode<T> *pMax = p;
        for (Rank i = 1; i < n; i++)
        {
            p = p -> succ;
            if ((p -> data) >= (pMax -> data)) pMax = p;
        }
        return pMax;
    }

    // 列表去重
    // 返回删除的元素个数
    Rank deduplicate()
    {
        Rank oldsize = _size;
        listnode<T> *p = last();
        for (Rank r = 0; p != header; p = p -> pred)
        {
            if (listnode<T> *q = find(p -> data, p -> succ, r)) remove(q);
            else r++;
        }
        return oldsize - _size;
    }

    // 列表去重
    // 返回删除的元素个数
    // 采用检查临近元素的方法，可优化
    // 只允许对递增列表使用
    Rank uniquify()
    {
        if (_size < 2) return 0; // 排除平凡情况
        Rank oldsize = _size;
        listnode<T> *p = first();
        listnode<T> *q;
        while (trailer != (q = p -> succ))
        {
            if ((p -> data) != (q -> data)) p = q;
            else remove(q);
        }
        return oldsize - _size;
    }

    // 选择排序算法
    // 对[p, p + r)间的元素排序，修改原值
    void selectionSort(listnode<T> *p, Rank n)
    {
        listnode<T> *tail = p;
        for (Rank i = 1; i < n; i++) tail = tail -> succ;
        while (1 < n)
        {
            listnode<T> *pMax = selectMax(p, n);
            exchange(pMax, tail);
            tail = tail -> pred; n--;
        }
    }

    // 归并排序算法
    // 对[p, p + r)间的元素排序，修改原值
    void mergeSort(listnode<T>*& p, Rank n)
    {
        if (n < 2) return;
        listnode<T> *q = p; Rank m = n >> 1;
        for (Rank i = 0; i < m; i++) q = q -> succ;
        mergeSort(p, m); mergeSort(q, n - m);
        p = merge(p, m, q, n - m);
    }
};

// 栈模板类
// 最大栈
template <typename T>
class Stack : protected List<T>
{
protected:
    // 维护两个列表，分别记录到队尾的最大值的位置和长度信息
    List<T> pair_p;
    List<Rank> pair_r;

public:
    // 构建一个空栈
    Stack(){};

    // 访问栈顶，为只读接口
    const T& top() const{return (List<T>::first() -> data);}
    // 获取栈中的最大元素，只读
    const T& getMax() const{return (pair_p[0]);}
    // 入栈
    void push(const T& e)
    {
        if ((pair_p.size() == 0) || (e > pair_p[0]))
        {
            pair_p.insert(e);
            pair_r.insert(1);
        }
        else pair_r[0]++;
        List<T>::insert(e);
    }
    // 出栈
    T pop()
    {
        pair_r[0]--;
        if (pair_r.first() -> data == 0)
        {
            pair_p.remove(pair_p.first());
            pair_r.remove(pair_r.first());
        }
        return List<T>::remove(List<T>::first());
    }
};

// 基于列表和堆实现的队列
template <typename T>
class Queue : protected List<T>
{
private:
    // 维护两个列表，分别记录到队尾的最大值的位置和长度信息
    List<T> pair_p;
    List<Rank> pair_r;

public:
    // 构建一个空队列
    Queue(){};

    // 访问队首，为只读接口
    const T& front() const{return (List<T>::first() -> data);}
    // 获取队列中的最大元素，只读
    const T& getMax() const{return pair_p[0];}
    // 入队
    void enqueue(const T& e)
    {
        listnode<T> *pp = pair_p.insert(e);
        listnode<Rank> *pr = pair_r.insert(1);
        listnode<Rank> *endr = pr;
        pp = pp -> pred;
        pr = pr -> pred;
        while (((pp -> data) < e) && ((pp -> pred) != NULL))
        {
            pp = pp -> pred; pr = pr -> pred;
            pair_p.remove(pp -> succ);
            (endr -> data) += pair_r.remove(pr -> succ);
        }
        List<T>::insert(e);
    }
    // 出队
    T dequeue()
    {
        (pair_r.first() -> data)--;
        if (pair_r.first() -> data == 0)
        {
            pair_p.remove(pair_p.first());
            pair_r.remove(pair_r.first());
        }
        return List<T>::remove(List<T>::first());
    }
};