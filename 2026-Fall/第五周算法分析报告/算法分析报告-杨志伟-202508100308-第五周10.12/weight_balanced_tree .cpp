#include <algorithm>
#include <cstdint>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

using namespace std;

class WeightBalancedTree {
    struct Node {
        int key;
        int size = 1;
        Node* left = nullptr;
        Node* right = nullptr;
        explicit Node(int value) : key(value) {}
    };

    Node* root_ = nullptr;
    int numerator_;
    int denominator_;
    int rebuilds_ = 0;

    static int size(const Node* node) { return node ? node->size : 0; }

    static void refresh(Node* node) {
        node->size = 1 + size(node->left) + size(node->right);
    }

    bool unbalanced(const Node* node) const {
        const int64_t limit = int64_t(numerator_) * node->size;
        return int64_t(denominator_) * size(node->left) > limit ||
               int64_t(denominator_) * size(node->right) > limit;
    }

    // On unwinding, an ancestor overwrites a descendant: the final link is
    // the highest newly unbalanced node on the modified search path.
    bool insertAt(Node*& node, int key, Node**& highest) {
        if (!node) {
            node = new Node(key);
            return true;
        }
        if (key == node->key) return false;

        bool inserted = key < node->key
            ? insertAt(node->left, key, highest)
            : insertAt(node->right, key, highest);
        if (inserted) {
            refresh(node);
            if (unbalanced(node)) highest = &node;
        }
        return inserted;
    }

    int removeMinimum(Node*& node, Node**& highest) {
        if (!node->left) {
            int result = node->key;
            Node* old = node;
            node = node->right;
            delete old;
            return result;
        }
        int result = removeMinimum(node->left, highest);
        refresh(node);
        if (unbalanced(node)) highest = &node;
        return result;
    }

    bool eraseAt(Node*& node, int key, Node**& highest) {
        if (!node) return false;

        if (key == node->key) {
            if (!node->left || !node->right) {
                Node* old = node;
                node = node->left ? node->left : node->right;
                delete old;
                return true;
            }
            node->key = removeMinimum(node->right, highest);
        } else {
            bool erased = key < node->key
                ? eraseAt(node->left, key, highest)
                : eraseAt(node->right, key, highest);
            if (!erased) return false;
        }

        refresh(node);
        if (unbalanced(node)) highest = &node;
        return true;
    }

    static void collectInorder(Node* root, vector<Node*>& nodes) {
        vector<Node*> stack;
        Node* current = root;
        while (current || !stack.empty()) {
            while (current) {
                stack.push_back(current);
                current = current->left;
            }
            current = stack.back();
            stack.pop_back();
            nodes.push_back(current);
            current = current->right;
        }
    }

    static Node* buildMedian(vector<Node*>& nodes, int first, int last) {
        if (first >= last) return nullptr;
        int middle = first + (last - first) / 2;
        Node* root = nodes[middle];
        root->left = buildMedian(nodes, first, middle);
        root->right = buildMedian(nodes, middle + 1, last);
        refresh(root);
        return root;
    }

    static void rebuild(Node*& subtree) {
        vector<Node*> nodes;
        nodes.reserve(size(subtree));
        collectInorder(subtree, nodes);
        subtree = buildMedian(nodes, 0, static_cast<int>(nodes.size()));
    }

    static void destroy(Node* node) {
        if (!node) return;
        destroy(node->left);
        destroy(node->right);
        delete node;
    }

    static int height(const Node* node) {
        return node ? 1 + max(height(node->left), height(node->right)) : 0;
    }

    static void printTree(const Node* node, int depth) {
        if (!node) return;
        printTree(node->right, depth + 1);
        cout << string(depth * 4, ' ') << node->key << " (size=" << node->size << ")\n";
        printTree(node->left, depth + 1);
    }

    static void printInorder(const Node* node) {
        if (!node) return;
        printInorder(node->left);
        cout << node->key << ' ';
        printInorder(node->right);
    }

    bool verifyAt(const Node* node, optional<int> low, optional<int> high,
                  int& computed) const {
        if (!node) {
            computed = 0;
            return true;
        }
        if ((low && node->key <= *low) || (high && node->key >= *high)) return false;
        int leftSize = 0, rightSize = 0;
        if (!verifyAt(node->left, low, node->key, leftSize) ||
            !verifyAt(node->right, node->key, high, rightSize)) return false;
        computed = 1 + leftSize + rightSize;
        return node->size == computed && !unbalanced(node);
    }

public:
    WeightBalancedTree(int numerator = 2, int denominator = 3)
        : numerator_(numerator), denominator_(denominator) {}

    WeightBalancedTree(const WeightBalancedTree&) = delete;
    WeightBalancedTree& operator=(const WeightBalancedTree&) = delete;
    ~WeightBalancedTree() { destroy(root_); }

    bool insert(int key) {
        Node** highest = nullptr;
        bool changed = insertAt(root_, key, highest);
        if (highest) {
            rebuild(*highest);
            ++rebuilds_;
        }
        return changed;
    }

    bool erase(int key) {
        Node** highest = nullptr;
        bool changed = eraseAt(root_, key, highest);
        if (highest) {
            rebuild(*highest);
            ++rebuilds_;
        }
        return changed;
    }

    bool contains(int key) const {
        const Node* current = root_;
        while (current) {
            if (key == current->key) return true;
            current = key < current->key ? current->left : current->right;
        }
        return false;
    }

    bool verify() const {
        int computed = 0;
        return verifyAt(root_, nullopt, nullopt, computed);
    }

    void showTree() const { printTree(root_, 0); }
    void showInorder() const { printInorder(root_); cout << '\n'; }
    void showStats() const {
        cout << "size=" << size(root_) << " height=" << height(root_)
             << " rebuilds=" << rebuilds_
             << " valid=" << (verify() ? "yes" : "NO") << '\n';
    }
};

int main(int argc, char* argv[]) {
    int numerator = 2, denominator = 3;
    if (argc == 3) {
        try {
            numerator = stoi(argv[1]);
            denominator = stoi(argv[2]);
        } catch (...) {
            cerr << "Usage: tree_demo [alpha_numerator alpha_denominator]\n";
            return 1;
        }
    } else if (argc != 1) {
        cerr << "Usage: tree_demo [alpha_numerator alpha_denominator]\n";
        return 1;
    }
    if (denominator <= 0 || numerator <= 0 ||
        int64_t(2) * numerator <= denominator || numerator >= denominator) {
        cerr << "Require 1/2 < alpha < 1 (example: 2 3).\n";
        return 1;
    }

    WeightBalancedTree tree(numerator, denominator);
    cout << "Amortized weight-balanced BST, alpha=" << numerator << '/'
         << denominator << "\nCommands: i KEY, d KEY, s KEY, p, t, v, q\n";
    string command;
    while (cin >> command && command != "q") {
        if (command == "i" || command == "d" || command == "s") {
            int key;
            if (!(cin >> key)) break;
            if (command == "i") cout << (tree.insert(key) ? "inserted" : "duplicate") << '\n';
            if (command == "d") cout << (tree.erase(key) ? "deleted" : "not found") << '\n';
            if (command == "s") cout << (tree.contains(key) ? "found" : "not found") << '\n';
        } else if (command == "p") {
            tree.showInorder();
        } else if (command == "t") {
            tree.showTree();
        } else if (command == "v") {
            tree.showStats();
        } else {
            cout << "Unknown command\n";
        }
    }
}
