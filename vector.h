#include "utils.h"
#define DEFAULT_CAPACITY 1024 // 默认初始容量设为1024
#define Rank unsigned int // 秩为无符号整型

// 向量模板类
template <typename T>
class Vector
{
protected:
    Rank _size; // 数据规模
    Rank _capacity; // 数据容量
    T *_elem; // 数据储存区

    // 复制数组区间[lo, hi)
    void copyfrom(const T *source, Rank lo, Rank hi)
    {
        _size = hi - lo;
        if (((hi - lo) << 1) > DEFAULT_CAPACITY) _capacity = ((hi - lo) << 1);
        else _capacity = DEFAULT_CAPACITY;
        _elem = new T[_capacity];
        for (Rank i = 0; i < hi - lo; i++) _elem[i] = source[lo + i];
    }

    // 扩容算法
    void expand()
    {
        if (_size < _capacity) return; // 无需扩容
        if (_capacity < DEFAULT_CAPACITY) _capacity = DEFAULT_CAPACITY; // 至少设为默认初始容量
        T *oldelem = _elem;
        _elem = new T[_capacity <<= 1]; // 采用容量加倍策略
        for (Rank i = 0; i < _size; i++) _elem[i] = oldelem[i];
        delete[] oldelem;
    }
    // 收缩算法
    void shrink()
    {
        if (_capacity < (DEFAULT_CAPACITY << 1)) return;
        if ((_size << 2) > _capacity) return;// 以25%为界决定是否收缩
        T *oldElem = _elem;
        _elem = new T[_capacity >>= 1];
        for (Rank i = 0; i < _size; i++ ) _elem[i] = oldElem[i];
        delete[] oldElem;
    }

    // 交换lo和hi处的数据
    // 需要构造临时变量，存在优化空间
    void swap(Rank lo, Rank hi)
    {
        T temp = _elem[lo];
        _elem[lo] = _elem[hi];
        _elem[hi] = temp;
    }

    // 归并算法
    void merge(Rank lo, Rank mid, Rank hi)
    {
        Rank i = 0, j = 0, k = 0;
        T *A = _elem + lo;
        Rank lb = mid - lo;
        T *B = new T[lb];
        for (Rank i = 0;i < lb; i++) B[i] = A[i];
        Rank lc = hi - mid;
        T *C = _elem + mid;

        while ((j < lb) && (k < lc)) A[i++] = (B[j] <= C[k]) ? B[j++] : C[k++]; // 保证稳定性
        while (j < lb) A[i++] = B[j++];
        delete[] B; // new-delete较为耗时，可考虑使用全局变量数组
    }

    // 快排分割算法
    // LGU策略
    Rank partition(Rank lo, Rank hi)
    {
        swap(lo, (lo * 2 + hi) / 3); // 亦可设计为随机选取轴点
        T pivot = _elem[lo];
        Rank mi = lo;
        for (Rank i = lo + 1; i < hi; i++)
        {
            if (_elem[i] < pivot) swap(++mi, i);
        }
        swap(lo, mi);
        return mi;
    }

public:
    // 构造一个容量为c的未初始化向量
    // c默认为DEFAULT_CAPACITY
    Vector(Rank c = DEFAULT_CAPACITY)
    {
        _size = 0;
        _capacity = c;
        _elem = new T[_capacity];
    }
    // 构造一个规模为s的向量，并全部初始化为e
    Vector(Rank s, T e)
    {
        _size = s;
        if ((s << 1) > DEFAULT_CAPACITY) _capacity = (s << 1);
        else _capacity = DEFAULT_CAPACITY;
        _elem = new T[_capacity];
        for (Rank i = 0; i < s; i++) _elem[i] = e;
    }
    // 基于数组区间[lo, hi)构造一个向量
    Vector(const T* source, Rank lo, Rank hi){copyfrom(source, lo, hi);}
    // 基于向量区间[lo, hi)构造一个向量
    Vector(const Vector<T>& source, Rank lo, Rank hi){copyfrom(source._elem, lo, hi);}
    // 复制向量
    Vector(const Vector<T>& source){copyfrom(source._elem, 0, source._size);}

    // 析构函数
    ~Vector(){delete[] _elem;}

    // 报告当前向量的规模
    Rank size(){return _size;}
    // 报告当前向量的容量
    Rank capacity(){return _capacity;}

    // 向量清空
    void clear()
    {
        _size = 0;
        _capacity = DEFAULT_CAPACITY << 1;
        shrink();
    }

    // 重载[]运算符
    // 按mod取秩
    T& operator[] (Rank r)
    {
        r = r % _size;
        if (r < 0) r += _size;
        return _elem[r];
    }
    // 重载[]运算符，只读
    // 按mod取秩
    const T& operator[] (Rank r) const
    {
        r = r % _size;
        if (r >= _size) r -= _size;
        return _elem[r];
    }

    // 重载==运算符
    bool operator== const(const Vector<T>& V)
    {
        if (_size != V._size) return false;
        else
        {
            for (Rank i = 0; i < V._size; i++)
            {
                if (_elem[i] != V._elem[i]) return false;
            }
        }
        return true;
    }
    // 重载!=运算符
    bool operator!= const(const Vector<T>& V)
    {
        if (_size != V._size) return true;
        else
        {
            for (Rank i = 0; i < V._size; i++)
            {
                if (_elem[i] != V._elem[i]) return true;
            }
        }
        return false;
    }

    // 重载<运算符
    // 先大即大，先长即大
    bool operator< const(const Vector<T>& V)
    {
        for (Rank i; (i < _size) && (i < V._size); i++)
        {
            if (_elem[i] < V._elem[i]) return true;
            if (_elem[i] > V._elem[i]) return false;
        }
        if (_size < V._size) return true;
        if (_size > V._size) return false;
        return false;
    }
    // 重载>运算符
    // 先大即大，先长即大
    bool operator> const(const Vector<T>& V)
    {
        for (Rank i; (i < _size) && (i < V._size); i++)
        {
            if (_elem[i] > V._elem[i]) return true;
            if (_elem[i] < V._elem[i]) return false;
        }
        if (_size > V._size) return true;
        if (_size < V._size) return false;
        return false;
    }

    // 重载+运算符
    // 只允许对两相同规模的向量使用
    Vector<T> operator+ const(const Vector<T>& V)
    {
        T* array = new T[_size];
        for (Rank i; i < _size; i++) array[i] = _elem[i] + V._elem[i];
        Vector<T> newV(array, 0, _size);
        return newV;
    }
    // 重载-运算符
    // 只允许对两相同规模的向量使用
    Vector<T> operator- const(const Vector<T>& V)
    {
        T* array = new T[_size];
        for (Rank i; i < _size; i++) array[i] = _elem[i] - V._elem[i];
        Vector<T> newV(array, 0, _size);
        return newV;
    }

    // 重载+=运算符
    // 只允许对两相同规模的向量使用
    void operator+= (const Vector<T>& V)
    {
        for (Rank i; i < _size; i++) _elem[i] += V._elem[i];
    }
    // 重载-=运算符
    // 只允许对两相同规模的向量使用
    void operator-= (const Vector<T>& V)
    {
        for (Rank i; i < _size; i++) _elem[i] -= V._elem[i];
    }

    // 遍历接口
    template <typename VST>
    void traverse(VST& visit){for (Rank i = 0; i < _size; i++) visit(_elem[i]);}

    // 插入操作
    // 在秩r位置插入元素e，已有元素依次后移
    // 默认在向量尾部插入
    void insert(Rank r, const T& e)
    {
        expand();
        for (Rank i = _size; i > r; i--) _elem[i] = _elem[i - 1]; // 从后往前操作，减小空间复杂度
        _elem[r] = e;
        _size++;
    }
    void insert(const T& e){insert(_size, e);}

    // 区间删除操作
    // 删除[lo, hi)间的元素，已有元素依次前移
    void remove(Rank lo, Rank hi)
    {
        if (lo == hi) return; // 处理退化情况
        while (hi < _size) // 双指针法
        {
            _elem[lo] = _elem[hi];
            lo++;hi++;
        }
        _size = lo;
        shrink();
    }
    // 单元素删除操作
    // 返回被删除的元素，已有元素依次前移
    T remove(Rank r)
    {
        T e = _elem[r];
        remove(r, r + 1);
        shrink();
        return e;
    }

    // 顺序查找操作
    // 在[lo, hi)中搜索值target，返回其秩，默认为在向量整体中查找
    // 若不存在target，返回__UINT32_MAX__，若存在多个target，返回最大的秩
    Rank find(const T& target, Rank lo, Rank hi)
    {
        while (hi > lo)
        {
            hi--;
            if (_elem[hi] == target) return hi;
        }
        return __UINT32_MAX__;
    }
    Rank find(const T& target){return find(target, 0, _size);}

    // 去除向量中所有重复的元素，返回删除的元素个数
    // 效率很差，如有必要，建议使用排序算法
    Rank deduplicate()
    {
        Rank oldsize = _size;
        for (Rank i = 1; i < _size;)
        {
            if (find(_elem[i], 0, i) == __UINT32_MAX__) i++;
            else remove(i);
        }
        shrink();
        return (oldsize - _size);
    }

    // 向量反序，修改原向量值
    void reverse()
    {
        for(Rank i = 0; i < _size / 2; i++) swap(i, _size - i - 1);
    }

    // 去除向量中所有重复的元素，返回删除的元素个数
    // 只允许对递增向量使用
    Rank uniquify()
    {
        Rank i = 0, j = 0;
        while (++j < _size) // 采用双指针法提高效率
        {
            if (_elem[i] != _elem[j]) _elem[++i] = _elem[j];
        }
        _size = ++i;
        shrink();
        return (j - i);
    }

    // 二分查找算法
    // 在[lo, hi)中搜索值target，返回其秩，默认为在向量整体中查找
    // 总是返回不大于target的最大者的秩
    // 只允许对递增向量使用
    Rank biSearch(T const& target, Rank lo, Rank hi)
    {
        while (lo < hi)
        {
            Rank mid = (hi + lo) >> 1;
            if (target < _elem[mid]) hi = mid;
            else lo = mid + 1;
        }
        return lo - 1;
    }
    Rank biSearch(T const& target){return biSearch(target, 0, _size);}

    // 归并排序算法
    void mergeSort(Rank lo, Rank hi)
    {
        if (hi - lo < 2) return;
        Rank mid = (lo + hi) / 2;
        mergeSort(lo, mid); mergeSort(mid, hi);
        merge(lo, mid, hi);
    }

    // 快速排序算法
    // 可采用栈优化空间复杂度
    void quickSort(Rank lo, Rank hi)
    {
        if (hi - lo < 2) return;
        Rank mi = partition(lo, hi);
        quickSort(lo, mi);
        quickSort(mi + 1, hi);
    }

    // 快速选取算法
    // 在[lo, hi)区间内，将秩k位置转化为轴点
    void quickSelect(Rank lo, Rank hi, Rank k)
    {
        k += lo;
        while (lo < hi)
        {
            Rank i = lo, j = hi; T pivot = _elem[lo]; // 先猜后证
            while (i < j)
            {
                do j--; while ((i < j) && (pivot <= _elem[j])); if (i < j) _elem[i] = _elem[j];
                do i++; while ((i < j) && (_elem[i] <= pivot)); if (i < j) _elem[j] = _elem[i];
            }
            _elem[j] = pivot;
            if (k <= j) hi = j;
            if (i <= k) lo = i;
        }
    }

    // 希尔排序算法
    // 使用PS序列
    void shellSort(Rank lo, Rank hi)
    {
        for (Rank d = 0x7FFFFFFF; d > 0; d >>= 1)
        {
            for (Rank j = lo + d; j < hi; j++)
            {
                T x = _elem[j]; Rank i = j;
                while ((lo + d <= i) && (x < _elem[i - d]))
                {
                    _elem[i] = _elem[i - d];
                    i -= d;
                }
                _elem[i] = x;
            }
        }
    }
};

// 堆模板类
template<typename T>
class Heap : protected Vector<T>
{
protected:
    // 返回父节点的秩
    Rank r_parent(Rank r){return (r - 1) >> 1;}
    // 返回左孩子节点的秩
    Rank r_lc(Rank r){return (r << 1) + 1;}
    // 返回右孩子节点的秩
    Rank r_rc(Rank r){return (r + 1) << 1;}

    // 返回父子间的最大者的秩
    // 相等时父节点优先，其次左孩子节点优先
    // 父节点值为参数输入，而非真实值
    Rank proper(Rank r, const T& parent)
    {
        Rank lc = r_lc(r); Rank rc = r_rc(r);
        if ((lc >= Vector<T>::_size) && (rc >= Vector<T>::_size)) return r;
        else if ((lc < Vector<T>::_size) && (rc >= Vector<T>::_size)) return (Vector<T>::_elem[lc] > parent) ? lc : r;
        else
        {
            if ((Vector<T>::_elem[rc] > parent) && (Vector<T>::_elem[rc] > Vector<T>::_elem[lc])) return rc;
            else if (Vector<T>::_elem[lc] > parent) return lc;
            else return r;
        }
    }

    // 将区间[lo, hi)堆化
    // Floyd算法（自下而上的下滤）
    void heapify(Rank lo, Rank hi)
    {
        for (Rank i = (hi - lo) / 2 - 1; i != lo - 1; i--) percolateDown(i);
    }

    // 上滤操作
    // 对秩r的元素执行上滤，返回结束位置
    // 改进：在上滤完成后统一进行交换
    Rank percolateUp(Rank r)
    {
        T temp = Vector<T>::_elem[r];
        while (0 < r)
        {
            Rank lo = r_parent(r);
            if (Vector<T>::_elem[lo] > temp) break;
            Vector<T>::_elem[r] = Vector<T>::_elem[lo]; r = lo;
        }
        Vector<T>::_elem[r] = temp;
        return r;
    }

    // 下滤操作
    // 对秩r的元素执行下滤，返回结束位置
    // 改进：在下滤完成后统一进行交换
    Rank percolateDown(Rank r)
    {
        T temp = Vector<T>::_elem[r];
        Rank hi = proper(r, temp);
        while (r != hi)
        {
            Vector<T>::_elem[r] = Vector<T>::_elem[hi]; r = hi;
            hi = proper(r, temp);
        }
        Vector<T>::_elem[r] = temp;
        return r;
    }

public:
    // 默认构造函数
    Heap(){};
    // 基于数组区间[lo, hi)构造一个堆
    Heap(const T *source, Rank lo, Rank hi)
    {
        Vector<T>::copyfrom(source, lo, hi);
        heapify(0, Vector<T>::_size);
    }

    // 报告当前堆的规模
    Rank size(){return Vector<T>::size();}

    // 访问最大元素，只读
    T& getMax(){return Vector<T>::_elem[0];}

    // 插入操作，在堆中插入元素e
    void insert(const T& e)
    {
        Vector<T>::insert(e);
        percolateUp(Vector<T>::_size - 1);
    }

    // 删除最大元素并返回
    T delMax()
    {
        Vector<T>::swap(0, --Vector<T>::_size);
        percolateDown(0);
        return Vector<T>::_elem[Vector<T>::_size];
    }
};
