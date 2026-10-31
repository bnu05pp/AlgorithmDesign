#include <algorithm>
#include <cstdint>
#include <iostream>
#include <random>
#include <stack>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

using namespace std;

enum class Strategy { standard, alternating, linkedList, randomChoice };

struct Duplicate {
    int value;
    Duplicate* next;
};

struct Node {
    int key;
    Node* left = nullptr;
    Node* right = nullptr;
    Duplicate* same = nullptr;
    bool nextLeft = true;
    explicit Node(int k) : key(k) {}
};

struct Result {
    int items = 0;
    int treeNodes = 0;
    int height = 0;
    long long visitedNodes = 0;
};

class EqualKeyTree {
    Node* root_ = nullptr;
    Strategy strategy_;
    mt19937 rng_;
    Result stats_;

public:
    EqualKeyTree(Strategy strategy, uint32_t seed) : strategy_(strategy), rng_(seed) {}
    EqualKeyTree(const EqualKeyTree&) = delete;
    EqualKeyTree& operator=(const EqualKeyTree&) = delete;

    ~EqualKeyTree() {
        stack<Node*> pending;
        if (root_) pending.push(root_);
        while (!pending.empty()) {
            Node* x = pending.top(); pending.pop();
            if (x->left) pending.push(x->left);
            if (x->right) pending.push(x->right);
            Duplicate* d = x->same;
            while (d) {
                Duplicate* old = d;
                d = d->next;
                delete old;
            }
            delete x;
        }
    }

    void insert(int key, int itemId) {
        ++stats_.items;
        Node** link = &root_;
        while (*link) {
            Node* x = *link;
            ++stats_.visitedNodes;
            if (key < x->key) link = &x->left;
            else if (key > x->key) link = &x->right;
            else if (strategy_ == Strategy::linkedList) {
                x->same = new Duplicate{itemId, x->same};
                return;
            } else if (strategy_ == Strategy::alternating) {
                bool left = x->nextLeft;
                x->nextLeft = !x->nextLeft;
                link = left ? &x->left : &x->right;
            } else if (strategy_ == Strategy::randomChoice) {
                link = (rng_() & 1U) ? &x->left : &x->right;
            } else {
                link = &x->right;
            }
        }
        *link = new Node(key);
        ++stats_.treeNodes;
    }

    Result result() {
        stack<pair<Node*, int>> pending;
        if (root_) pending.push({root_, 1});
        while (!pending.empty()) {
            auto [x, depth] = pending.top(); pending.pop();
            stats_.height = max(stats_.height, depth);
            if (x->left) pending.push({x->left, depth + 1});
            if (x->right) pending.push({x->right, depth + 1});
        }
        return stats_;
    }
};

Result run(Strategy strategy, int n, uint32_t seed) {
    EqualKeyTree tree(strategy, seed);
    for (int i = 0; i < n; ++i) tree.insert(42, i);
    return tree.result();
}

void show(const string& name, const Result& r) {
    cout << name << ": items=" << r.items
         << ", tree_nodes=" << r.treeNodes
         << ", height=" << r.height
         << ", insertion_node_visits=" << r.visitedNodes << '\n';
}

int main(int argc, char* argv[]) {
    int n = 127;
    uint32_t seed = 20261019;
    if (argc > 3) {
        cerr << "Usage: equal_keys_bst [N: 1..2000] [seed]\n";
        return 1;
    }
    try {
        if (argc >= 2) n = stoi(argv[1]);
        if (argc == 3) seed = static_cast<uint32_t>(stoul(argv[2]));
    } catch (...) {
        cerr << "N and seed must be integers.\n";
        return 1;
    }
    if (n < 1 || n > 2000) {
        cerr << "N must be between 1 and 2000.\n";
        return 1;
    }

    Result a = run(Strategy::standard, n, seed);
    Result b = run(Strategy::alternating, n, seed);
    Result c = run(Strategy::linkedList, n, seed);
    Result d = run(Strategy::randomChoice, n, seed);
    if (a.items != n || b.items != n || c.items != n || d.items != n ||
        a.treeNodes != n || b.treeNodes != n || c.treeNodes != 1 || d.treeNodes != n ||
        a.height != n || c.height != 1)
        throw runtime_error("strategy invariant mismatch");

    cout << "CLRS 12-1: inserting " << n << " equal keys, seed=" << seed << "\n";
    show("a standard equality-goes-right", a);
    show("b alternating left/right", b);
    show("c one node plus duplicate list", c);
    show("d random left/right", d);
    cout << "Random strategy: one run is not its expectation.\n";
}
