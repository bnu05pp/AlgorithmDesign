// =============================================================================
//  test_6_2.cpp  --  思考题 6-2 各小题的验证与演示程序
//
//  编译：g++ -O2 -std=c++17 test_6_2.cpp -o test_6_2
//  运行：./test_6_2
//
//  输出分五段，分别对应 6-2 的 (a)~(e)；每段都既有「打印演示」又有
//  「断言校验」，方便课堂现场跑给学生看，也方便截图放进 PPT。
// =============================================================================
#include "dary_heap.hpp"

#include <algorithm>
#include <chrono>
#include <cassert>
#include <cmath>
#include <iostream>
#include <queue>
#include <random>
#include <string>
#include <vector>

using namespace clrs;
using std::cout;
using std::string;
using std::vector;

static void banner(const string& s) {
    cout << "\n" << string(72, '=') << "\n" << s << "\n" << string(72, '=') << "\n";
}

// 用 log 公式算堆高，用于和 DaryMaxHeap::height() 对照
static int heightFormula(int n, int d) {
    if (n <= 1) return 0;
    return (int)std::floor(std::log((double)((n - 1) * (d - 1) + 1)) / std::log((double)d));
}

// -----------------------------------------------------------------------------
// (a) 用数组表示 d 叉堆：下标公式演示
// -----------------------------------------------------------------------------
static void demo_a() {
    banner("6-2(a)  用数组表示一个 d 叉堆");

    struct Case { int d; vector<int> keys; };
    vector<Case> cases = {
        {2, {91, 83, 64, 56, 75, 11, 41, 27, 33}},   // 讲义里的二叉最大堆
        {3, {91, 83, 64, 56, 75, 11, 41, 27, 33}},   // 同样的数据用 3 叉堆存
        {4, {91, 83, 64, 56, 75, 11, 41, 27, 33}},
    };

    for (auto& c : cases) {
        DaryMaxHeap h(c.d);
        vector<Item> items;
        for (size_t i = 0; i < c.keys.size(); ++i) items.push_back(Item{c.keys[i], (int)i});
        h.build(items);

        cout << "\n--- d = " << c.d << " ---\n";
        cout << "层序数组 A[1..n] = " << h.toArrayString() << "\n";
        cout << "树形（逐层）：\n" << h.toTreeString();
        cout << "验证：\n";
        cout << "  PARENT(i)      = floor((i-2)/d) + 1\n";
        cout << "  CHILD(i, j)    = d*(i-1) + j + 1   (1 <= j <= d)\n";
        cout << "  d = " << c.d << " 时结点 1 的孩子下标 = ";
        for (int j = 1; j <= c.d; ++j) cout << h.child(1, j) << (j < c.d ? ", " : "");
        cout << "\n";
        if (h.size() >= c.d + 1) {
            int probe = c.d + 1;   // 最后一个孩子
            cout << "  结点 " << probe << " 的父结点 = " << h.parent(probe)
                 << "（应为 1）\n";
            assert(h.parent(probe) == 1);
        }
        cout << "  是最大堆？ " << (h.isMaxHeap() ? "是" : "否") << "\n";
        assert(h.isMaxHeap());
    }
    cout << "\n结论：把 d 叉堆按「从上到下、从左到右」层序编号存入 A[1..n]，\n"
            "      用上面两个既不需要指针也不需要额外空间的公式即可在 O(1) 内\n"
            "      定位父子结点（d 叉堆的实现适配，即 6-2(a)）。\n";
}

// -----------------------------------------------------------------------------
// (b) 堆高与 n、d 的关系
// -----------------------------------------------------------------------------
static void demo_b() {
    banner("6-2(b)  含 n 个元素的 d 叉堆的高度  h = Theta(log_d n)");

    cout << "理论：高度为 h 的 d 叉堆拥有的结点数介于\n";
    cout << "      (d^h - 1)/(d - 1) + 1  <=  n  <=  (d^(h+1) - 1)/(d - 1)\n";
    cout << "      解出  log_d((d-1)n + 1) - 1  <=  h  <=  log_d((d-1)(n-1) + 1)\n";
    cout << "      即 h = Theta(log_d n)。\n\n";

    cout << "         n        d=2   d=3   d=4   d=8  d=16\n";
    int ns[] = {16, 100, 1000, 10000, 100000, 1000000};
    int ds[] = {2, 3, 4, 8, 16};
    for (int n : ns) {
        std::printf("%10d   ", n);
        for (int d : ds) {
            DaryMaxHeap h(d);
            vector<Item> items;
            items.reserve(n);
            for (int i = 0; i < n; ++i) items.push_back(Item{i, i});
            h.build(items);
            int hm = h.height();
            assert(hm == heightFormula(n, d));   // 实测 == 闭式
            std::printf("%6d", hm);
        }
        std::printf("\n");
    }
    cout << "\n结论：n 相同时 d 越大堆越矮；h 与 log_d n 成正比 —— 这正是\n"
            "      「增大 d 可以让树变浅」的直接依据。\n";
}

// -----------------------------------------------------------------------------
// (c) EXTRACT-MAX
// -----------------------------------------------------------------------------
static void demo_c() {
    banner("6-2(c)  EXTRACT-MAX 的高效实现   —— O(d * log_d n)");

    cout << "算法：\n";
    cout << "  EXTRACT-MAX(A)\n";
    cout << "    1  if A.heap_size < 1  ->  error \"heap underflow\"\n";
    cout << "    2  max = A[1]\n";
    cout << "    3  A[1] = A[A.heap_size]\n";
    cout << "    4  A.heap_size = A.heap_size - 1\n";
    cout << "    5  MAX-HEAPIFY(A, 1)          // 每层比较 d 个孩子\n";
    cout << "    6  return max\n\n";

    int d = 3;
    DaryMaxHeap h(d);
    vector<Item> items;
    vector<int> keys = {91, 83, 64, 56, 75, 11, 41, 27, 33};
    for (size_t i = 0; i < keys.size(); ++i) items.push_back(Item{keys[i], (int)i});
    h.build(items);

    cout << "初始 d = " << d << " 叉堆：" << h.toArrayString() << "\n";
    cout << "反复 EXTRACT-MAX 得到：";
    vector<int> got;
    while (!h.empty()) got.push_back(h.extractMax().key);
    for (size_t i = 0; i < got.size(); ++i) cout << got[i] << (i + 1 < got.size() ? " > " : "");
    cout << "\n";

    vector<int> expect = keys;
    std::sort(expect.begin(), expect.end(), std::greater<int>());
    assert(got == expect);                     // 必须降序输出
    assert(h.empty());
    cout << "校验：输出与降序排序结果一致，堆被清空 —— 通过。\n";
    cout << "复杂度：每层最多 d 次比较，树高 log_d n，故 O(d log_d n)。\n";
}

// -----------------------------------------------------------------------------
// (d) INCREASE-KEY
// -----------------------------------------------------------------------------
static void demo_d() {
    banner("6-2(d)  INCREASE-KEY 的高效实现   —— O(log_d n)");

    cout << "算法（沿父链上浮）：\n";
    cout << "  INCREASE-KEY(A, i, key)\n";
    cout << "    1  if key < A[i].key  ->  error \"new key is smaller than current key\"\n";
    cout << "    2  A[i].key = key\n";
    cout << "    3  while i > 1 and A[PARENT(i)].key < A[i].key\n";
    cout << "    4      exchange A[i] <-> A[PARENT(i)]\n";
    cout << "    5      i = PARENT(i)\n\n";

    int d = 4;
    DaryMaxHeap h(d);
    vector<Item> items;
    vector<int> keys = {91, 83, 64, 56, 75, 11, 41, 27, 33, 60, 5};
    for (size_t i = 0; i < keys.size(); ++i) items.push_back(Item{keys[i], (int)i});
    h.build(items);

    cout << "初始 d = " << d << " 叉堆：" << h.toArrayString() << "\n";

    // 找到关键字 11 所在的下标，把它增大到 95
    int idx = -1;
    for (int i = 1; i <= h.size(); ++i) if (h.raw()[i].key == 11) { idx = i; break; }
    cout << "把下标 " << idx << " 处的关键字 11 增大到 95 ...\n";
    h.increaseKey(idx, 95);
    cout << "新的层序数组：" << h.toArrayString() << "\n";
    assert(h.isMaxHeap());
    assert(h.top().key == 95);                 // 应当一路上浮到根
    cout << "校验：新堆仍是最大堆，且 95 已上浮到根 —— 通过。\n";
    cout << "复杂度：只走一条从 i 到根的父链，长度 <= 堆高 log_d n，\n"
            "        每层 O(1)，故 O(log_d n)。\n";
}

// -----------------------------------------------------------------------------
// (e) INSERT
// -----------------------------------------------------------------------------
static void demo_e() {
    banner("6-2(e)  INSERT 的高效实现   —— O(log_d n)");

    cout << "算法：\n";
    cout << "  INSERT(A, key)\n";
    cout << "    1  A.heap_size = A.heap_size + 1\n";
    cout << "    2  A[A.heap_size].key = -infinity\n";
    cout << "    3  INCREASE-KEY(A, A.heap_size, key)\n\n";

    int d = 2;
    DaryMaxHeap h(d);
    vector<Item> items;
    vector<int> keys = {91, 83, 64, 56, 75, 11, 41, 27, 33};
    for (size_t i = 0; i < keys.size(); ++i) items.push_back(Item{keys[i], (int)i});
    h.build(items);
    cout << "初始二叉堆：" << h.toArrayString() << "\n";

    h.insert(85, -1);
    cout << "插入 85 后 ：" << h.toArrayString() << "   （85 上浮到第 2 层）\n";
    assert(h.isMaxHeap());

    h.insert(100, -1);
    cout << "再插入 100 ：" << h.toArrayString() << "   （100 上浮到根）\n";
    assert(h.isMaxHeap());
    assert(h.top().key == 100);
    cout << "校验：两次插入后仍是最大堆，且 100 成为新堆顶 —— 通过。\n";
    cout << "复杂度：尾部添加 O(1) + 一次 INCREASE-KEY O(log_d n)，故 O(log_d n)。\n";
}

// -----------------------------------------------------------------------------
// 随机对拍：用 std::priority_queue 做基准，验证实现正确
// -----------------------------------------------------------------------------
static void stress() {
    banner("随机对拍：与 std::priority_queue 对比 100 组随机操作序列");

    std::mt19937 rng(20261010);
    for (int trial = 0; trial < 100; ++trial) {
        int d = 2 + (int)(rng() % 15);                 // d in [2,16]
        DaryMaxHeap h(d);
        std::priority_queue<int> ref;
        int nextId = 0;
        for (int step = 0; step < 2000; ++step) {
            int op = (int)(rng() % 3);
            if (op == 0 || ref.empty()) {
                int k = (int)(rng() % 100000);
                h.insert(k, nextId++);
                ref.push(k);
            } else if (op == 1) {
                assert(h.extractMax().key == ref.top());
                ref.pop();
            } else if (nextId > 0) {
                int id = (int)(rng() % nextId);
                if (h.indexOf(id) < 0) continue;       // 该对象已被 EXTRACT 掉
                int cur = h.raw()[h.indexOf(id)].key;
                int k = cur + (int)(rng() % 1000);     // 关键字只能增大
                h.increaseKeyById(id, k);
                // 用基准重算：直接在优先队列里重建（仅用于验证）
                ref = std::priority_queue<int>();
                for (int i = 1; i <= h.size(); ++i) ref.push(h.raw()[i].key);
            }
            assert(h.isMaxHeap());
        }
    }
    cout << "全部通过：100 组 × 2000 次随机操作，堆性质与输出均与基准一致。\n";
}

int main() {
    cout << "思考题 6-2「对 d 叉堆的分析」—— C++ 验证程序\n";
    cout << "（《算法导论》第三版 第 6 章 + 思考题 6-2）\n";

    demo_a();
    demo_b();
    demo_c();
    demo_d();
    demo_e();
    stress();

    banner("全部断言通过 —— 6-2(a)~(e) 的实现与复杂度结论均成立");
    return 0;
}
