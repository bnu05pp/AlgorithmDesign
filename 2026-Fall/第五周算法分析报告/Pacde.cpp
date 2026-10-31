#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

class DaryMaxHeap {
public:
    //成员变量
    int d; vector<int> A; int heap_size;
    //构造函数
    DaryMaxHeap(int d_, vector<int> arr) {
        d = d_; A.push_back(0);
        for (int x : arr) A.push_back(x);
        heap_size = arr.size();
        buildHeap();//使用建堆函数让A数组拥有d叉最大堆性质
    }
    int parent(int i) { return (i+d-2)/d; }
    int child(int i, int k) { return d*(i-1)+1+k; }
    // 下沉：遍历d个子节点找最大，交换后递归
    void dMaxHeapify(int i) {
        int largest = i;
        for (int k=1; k<=d; k++) {
            int c = child(i,k);
            if (c <= heap_size && A[c]>A[largest]) largest=c;
        }
        if (largest != i) { swap(A[i],A[largest]); dMaxHeapify(largest); }
    }
    // 建堆：从最后一个非叶节点自底向上
    void buildHeap() {
        for (int i=parent(heap_size); i>=1; i--)
            dMaxHeapify(i);
    }
    void printHeap() {
        for (int i=1; i<=heap_size; i++) cout<<A[i]<<" ";
        cout<<endl;
    }


    int extractMax() {
        if (heap_size < 1) exit(-1); // 空堆
        int max_val = A[1]; // 保存根=最大值
        A[1] = A[heap_size]; // 堆尾补根
        heap_size--; // 缩堆
        dMaxHeapify(1); // 从根下沉修复
        return max_val;
    }


    // increaseKey: 把位置 i 的关键字增大为 new_key
    void increaseKey(int i, int new_key) {
        if (new_key < A[i]) return; // 不能减小
        A
    [i] = new_key; // 更新值
        siftUp(i); // 上浮调整
    }

    // siftUp: 上浮，每层只和 1 个父节点比较
    void siftUp(int i) {
        while (i > 1 && A[i] > A[parent(i)]) {
            swap(A[i], A[parent(i)]); // 和父交换
            i = parent(i); // 上移一层
        }
    }

    // insert: 在堆尾插入新元素，再上浮
    void insert(int key){
        heap_size++; // 堆大小+1
        if (heap_size >= (int)A.size())
            A.push_back(key); // 扩容
        else
            A[heap_size] = key; // 堆尾放新元素
        siftUp(heap_size); // 上浮调整（同d题）
    }
    // siftUp 复用 d 题：每层只比 1 个父节点 = Θ(1)
};

int main() {
    vector<int> raw = {1,4,2,7,5};
    DaryMaxHeap heap(3, raw);
    heap.printHeap();

    cout << heap.extractMax() << endl;
    heap.printHeap();

    heap.increaseKey(4, 10); // 位置4增大到10
    heap.printHeap();
    
    heap.insert(10); // 插入 10
    heap.insert(3);  // 插入 3
    heap.printHeap();
}

