// CLRS 6-2: d-ary max heap (C++17)
// Input: d n / n elements / query index / increase index and key / insert key
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <vector>
using namespace std;

class DAryMaxHeap {
    int d;
    vector<long long> a;  // 1-based: a[0] is unused

    int parent(int i) const { return (i - 2) / d + 1; }
    int child(int i, int k) const { return d * (i - 1) + k + 1; }

    void siftDown(int i) {
        while (true) {
            int largest = i;
            int first = child(i, 1);
            for (int c = first; c <= min(size(), first + d - 1); ++c) {
                if (a[c] > a[largest]) largest = c;
            }
            if (largest == i) break;
            swap(a[i], a[largest]);
            i = largest;
        }
    }

public:
    DAryMaxHeap(int d_, const vector<long long>& data) : d(d_), a(1, 0) {
        if (d < 2) throw invalid_argument("d must be >= 2");
        a.insert(a.end(), data.begin(), data.end());
        for (int i = size() < 2 ? 0 : parent(size()); i >= 1; --i)
            siftDown(i);
    }

    int size() const { return static_cast<int>(a.size()) - 1; }
    long long maximum() const { return a.at(1); }
    int getParent(int i) const { return i == 1 ? 0 : parent(i); }
    vector<int> getChildren(int i) const {
        vector<int> indices;
        for (int k = 1; k <= d; ++k) {
            int c = child(i, k);
            if (c <= size()) indices.push_back(c);
        }
        return indices;
    }
    int height() const {
        int h = 0;
        for (int i = size(); i > 1; i = parent(i)) ++h;
        return h;
    }
    void increaseKey(int i, long long key) {
        if (i < 1 || i > size() || key < a[i])
            throw invalid_argument("invalid INCREASE-KEY");
        a[i] = key;
        while (i > 1 && a[parent(i)] < a[i]) {
            int p = parent(i);
            swap(a[i], a[p]);
            i = p;
        }
    }
    void insert(long long key) {
        a.push_back(key);
        int i = size();
        while (i > 1 && a[parent(i)] < a[i]) {
            int p = parent(i);
            swap(a[i], a[p]);
            i = p;
        }
    }
    long long extractMax() {
        if (size() == 0) throw out_of_range("empty heap");
        long long result = a[1];
        a[1] = a.back();
        a.pop_back();
        if (size() > 0) siftDown(1);
        return result;
    }
};

int main() {
    int d, n;
    cin >> d >> n;
    vector<long long> values(n);
    for (auto& x : values) cin >> x;
    int queryIndex, increaseIndex;
    long long newKey, insertKey;
    cin >> queryIndex >> increaseIndex >> newKey >> insertKey;

    DAryMaxHeap heap(d, values);
    cout << "PARENT(" << queryIndex << ") = " << heap.getParent(queryIndex) << '\n';
    cout << "CHILDREN(" << queryIndex << ") =";
    for (int c : heap.getChildren(queryIndex)) cout << ' ' << c;
    cout << "\nHEIGHT = " << heap.height() << '\n';
    cout << "INITIAL_MAX = " << heap.maximum() << '\n';
    heap.increaseKey(increaseIndex, newKey);
    cout << "AFTER_INCREASE_KEY_MAX = " << heap.maximum() << '\n';
    heap.insert(insertKey);
    cout << "AFTER_INSERT_SIZE = " << heap.size() << '\n';
    cout << "EXTRACT_MAX = " << heap.extractMax() << '\n';
    cout << "NEXT_MAX = " << heap.maximum() << '\n';
}
