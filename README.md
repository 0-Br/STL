# STL — 手写 C++ 标准模板库

清华大学邓俊辉老师「数据结构与算法」课程中的个人项目——手写 C++ 容器与算法库，涵盖向量、列表、栈、队列、多种搜索树、散列表及字符串匹配等核心数据结构与算法。全部组件为 header-only 实现，无外部依赖。

## 组件一览

### `utils.h` — 基础工具

- `release` / `Cleaner`：泛型资源回收模板，自动区分指针与非指针类型
- `Entry<K, V>`：键值对模板，重载全部比较运算符
- `Tuple<T1, T2>`：二元组模板，按字典序比较

### `vector.h` — 向量与堆

**`Vector<T>`** — 动态数组，容量加倍 / 25% 收缩策略

- 构造：默认、填充、数组区间、向量区间、拷贝
- 访问：`operator[]`（按 mod 取秩）、`traverse` 遍历
- 查找：顺序查找 `find`、二分查找 `biSearch`（返回不大于目标的最大秩）
- 修改：`insert`、`remove`（单元素 / 区间）、`reverse`
- 去重：`deduplicate`（无序）、`uniquify`（有序，双指针）
- 排序：归并排序 `mergeSort`、快速排序 `quickSort`（LGU 分割）、希尔排序 `shellSort`
- 选取：`quickSelect`（快速选取第 k 小元素）
- 运算符：`==` `!=` `<` `>` `+` `-` `+=` `-=`

**`Heap<T>`** — 最大堆（继承 `Vector`）

- Floyd 建堆、上滤 / 下滤（延迟交换优化）
- `insert`、`delMax`、`getMax`

### `list.h` — 列表、栈与队列

**`List<T>`** — 双向链表，带头尾哨兵

- 构造：空、填充、数组区间、节点区间、拷贝
- 访问：`operator[]`（按 mod 取秩）、`first` / `last`、`traverse`
- 查找：`find`（返回最后一个匹配节点）、`selectMax`
- 修改：`insert`（前插）、`remove`（单元素 / 区间）、`clear`
- 去重：`deduplicate`（无序）、`uniquify`（有序）
- 排序：选择排序 `selectionSort`、归并排序 `mergeSort`

**`Stack<T>`** — 栈（继承 `List`），O(1) `getMax`

- `push` / `pop` / `top` / `getMax`
- 通过辅助列表对维护最大值，实现常数时间取最大

**`Queue<T>`** — 队列（继承 `List`），O(1) 均摊 `getMax`

- `enqueue` / `dequeue` / `front` / `getMax`
- 通过单调辅助列表对维护最大值

### `tree.h` — 二叉树家族

**`binode<T>`** — 二叉树节点，维护 parent / lc / rc / height / size / color

- 中序前驱 `pred`、中序后继 `succ`、中序遍历 `trav_in`

**`Bitree<T>`** — 二叉树

- 插入根 / 左孩子 / 右孩子、递归删除子树、中序遍历

**`BST<T>`** — 二叉搜索树（继承 `Bitree`）

- `search`（二分查找）、`insert`、`remove`
- 循秩访问 `rfind`（按中序秩定位节点）
- 3+4 重构 `connect_34`、统一旋转 `rotateAt`

**`AVLtree<T>`** — AVL 树（继承 `BST`）

- 插入后单次旋转恢复平衡，删除后逐层旋转恢复平衡

**`Splaytree<T>`** — 伸展树（继承 `BST`）

- 双层伸展 `splay`，每次访问将目标伸展至根
- 基于伸展的 `search` / `insert` / `remove`

**`RBtree<T>`** — 红黑树（继承 `BST`）

- 双红修正 `solveDR`、双黑修正 `solveDB`
- 外部节点黑高度为 0，红黑高度统一维护

### `dictionary.h` — 位图与散列表

**`Bitmap`** — 位图

- `test` / `set` / `clear`，自动加倍扩容

**`Hashtable<K, V>`** — 散列表

- 多项式散列函数 `encode`
- 线性探测（开放定址）
- 懒删除（`Bitmap` 标记）
- 装填因子过高或懒标记过多时自动重哈希

### `string.h` — 字符串匹配

- `match_BF`：蛮力匹配
- `match_KMP`：KMP 算法（含改进 next 表，避免重复失配）
- `match_BM`：Boyer-Moore 算法（坏字符表 BC）

## 编译与使用

```bash
# 编译测试
g++ -std=c++11 -o test __test__.cpp

# 在自己的代码中引用
#include "vector.h"   // Vector, Heap
#include "list.h"     // List, Stack, Queue
#include "tree.h"     // Bitree, BST, AVLtree, Splaytree, RBtree
#include "dictionary.h" // Bitmap, Hashtable
#include "string.h"   // match_BF, match_KMP, match_BM
```

所有组件均为 header-only，无需额外链接。
