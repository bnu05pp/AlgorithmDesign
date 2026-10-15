// =============================================================================
//  bench_6_2.cpp  --  思考题 6-2 的复杂度实测程序
//
//  同一份源码编译两次：
//    (1) 墙钟时间版：
//        g++ -O2 -std=c++17 -static bench_6_2.cpp -o bench_time.exe
//    (2) 比较次数版（更能说明「为什么是 O(d log_d n)」）：
//        g++ -O2 -std=c++17 -static -DDARY_HEAP_COUNT bench_6_2.cpp -o bench_count.exe
//
//  输出 CSV：
//    data/timing.csv   列： d,n,op,ns_per_op
//    data/counts.csv   列： d,n,op,cmp_per_op
//    以及 data/heights.csv  列： d,n,height
//
//  其中 op ∈ { build, insert, extract, increase }
// =============================================================================
#include "dary_heap.hpp"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>

using namespace clrs;
using std::string;
using std::vector;

enum Op { OP_BUILD = 0, OP_INSERT = 1, OP_EXTRACT = 2, OP_INCREASE = 3 };
static const char* opName(int op) {
    switch (op) {
        case OP_BUILD:    return "build";
        case OP_INSERT:   return "insert";
        case OP_EXTRACT:  return "extract";
        default:          return "increase";
    }
}

static vector<Item> randomItems(int n, std::mt19937& rng) {
    std::uniform_int_distribution<int> dist(0, (1 << 30) - 1);
    vector<Item> v((size_t)n);
    for (int i = 0; i < n; ++i) v[(size_t)i] = Item{dist(rng), i};
    return v;
}

// 执行一轮指定的操作，返回本轮耗费（时间：秒；计数版：比较次数）
static long double execOnce(int op, int d, int n, unsigned seed) {
    std::mt19937 rng(seed);
    vector<Item> items = randomItems(n, rng);

#ifdef DARY_HEAP_COUNT
    unsigned long long c0 = g_cmp;
#else
    auto t0 = std::chrono::steady_clock::now();
#endif

    switch (op) {
        case OP_BUILD: {
            DaryMaxHeap h(d);
            h.build(items);
            break;
        }
        case OP_INSERT: {
            DaryMaxHeap h(d);
            for (const auto& it : items) h.insert(it.key, it.id);
            break;
        }
        case OP_EXTRACT: {
            DaryMaxHeap h(d);
            h.build(items);
            while (!h.empty()) h.extractMax();
            break;
        }
        default: {                                   // OP_INCREASE
            DaryMaxHeap h(d);
            h.build(items);
            std::uniform_int_distribution<int> pi(1, n);
            std::uniform_int_distribution<int> dv(1, 1000);
            for (int k = 0; k < n; ++k) {
                int i = pi(rng);
                h.increaseKey(i, h.raw()[(size_t)i].key + dv(rng));
            }
            break;
        }
    }

#ifdef DARY_HEAP_COUNT
    return (long double)(g_cmp - c0);
#else
    auto t1 = std::chrono::steady_clock::now();
    return std::chrono::duration<long double>(t1 - t0).count();
#endif
}

int main(int argc, char** argv) {
    const int ds[]  = {2, 3, 4, 8, 16};
    const int ns[]  = {10000, 30000, 100000, 300000, 1000000};
    const int REPS  = 3;                              // 每格重复 3 次取最优

    bool writeHeight = (argc > 1 && string(argv[1]) == "--heights");

#ifdef DARY_HEAP_COUNT
    std::cout << "== 比较次数版（counts.csv）==\n";
    const char* outPath = "data/counts.csv";
#else
    std::cout << "== 墙钟时间版（timing.csv）==\n";
    const char* outPath = "data/timing.csv";
#endif

    std::ofstream csv(outPath);
#ifdef DARY_HEAP_COUNT
    csv << "d,n,op,cmp_per_op\n";
#else
    csv << "d,n,op,ns_per_op\n";
#endif

    // 整齐的表头
    std::printf("\n%-6s %-9s %-9s %14s\n", "d", "n", "op",
#ifdef DARY_HEAP_COUNT
                "cmp/op");
#else
                "ns/op");
#endif

    for (int d : ds) {
        for (int n : ns) {
            for (int op = 0; op < 4; ++op) {
                long double best = 1e300L;
                for (int r = 0; r < REPS; ++r) {
                    long double c = execOnce(op, d, n, 12345u + 97u * (unsigned)r);
                    best = std::min(best, c);
                }
                long double per = best / (long double)n;   // 摊到每个元素
#ifdef DARY_HEAP_COUNT
                csv << d << "," << n << "," << opName(op) << "," << (unsigned long long)(per + 0.5) << "\n";
                std::printf("%-6d %-9d %-9s %14.2f\n", d, n, opName(op), (double)per);
#else
                csv << d << "," << n << "," << opName(op) << "," << (long double)(per * 1e9L) << "\n";
                std::printf("%-6d %-9d %-9s %14.1f\n", d, n, opName(op), (double)(per * 1e9L));
#endif
                std::cout.flush();
            }
        }
    }
    csv.close();

    // 顺带记录「实测堆高」→ heights.csv
    {
        std::ofstream hf("data/heights.csv");
        hf << "d,n,height\n";
        std::mt19937 hrng(1);
        for (int d : ds)
            for (int n : ns) {
                DaryMaxHeap h(d);
                vector<Item> items = randomItems(n, hrng);
                h.build(items);
                hf << d << "," << n << "," << h.height() << "\n";
            }
    }

    std::cout << "\n已写出 " << outPath << " 和 data/heights.csv\n";
    (void)writeHeight;
    return 0;
}
