#include <iostream>
#include <vector>
using namespace std;

struct Node {
    int key;
    int size;
    Node* left;
    Node* right;

    Node(int k) {
        key = k;
        size = 1;
        left = nullptr;
        right = nullptr;
    }
};

// 维护子树节点数量
void updateSize(Node* root) {
    if (root == nullptr) return;

    root->size = 1;
    if (root->left) root->size += root->left->size;
    if (root->right) root->size += root->right->size;
}

// 插入节点，构建二叉搜索树
Node* insert(Node* root, int key) {
    if (root == nullptr) return new Node(key);

    if (key < root->key) {
        root->left = insert(root->left, key);
    }
    else if (key > root->key) {
        root->right = insert(root->right, key);
    }

    updateSize(root);
    return root;
}

// 查找目标节点，同时记录父节点
Node* findNode(Node* root, int key, Node*& parent) {
    parent = nullptr;

    while (root != nullptr && root->key != key) {
        parent = root;

        if (key < root->key) {
            root = root->left;
        }
        else {
            root = root->right;
        }
    }

    return root;
}

// 中序遍历，收集原有节点指针
void collectNodes(Node* root, vector<Node*>& nodes) {
    if (root == nullptr) return;

    collectNodes(root->left, nodes);
    nodes.push_back(root);
    collectNodes(root->right, nodes);
}

// 利用中间节点递归重建，复用原有节点，
// 从数组 nodes 的 left 到 right 这一段节点中，
// 构造一棵平衡二叉搜索树，并返回这棵树的根节点。
Node* buildBalanced(vector<Node*>& nodes, int left, int right) {
    if (left > right) return nullptr;

    int mid = left + (right - left) / 2; 
    Node* root = nodes[mid];

    root->left = buildBalanced(nodes, left, mid - 1);
    root->right = buildBalanced(nodes, mid + 1, right);

    updateSize(root);
    return root;
}

// 重建以 x 为根的子树
Node* rebuild(Node* x) {
    vector<Node*> nodes;
    collectNodes(x, nodes);

    return buildBalanced(nodes, 0, (int)nodes.size() - 1);
}

// 前序遍历
void preorder(Node* root) {
    if (root == nullptr) return;

    cout << root->key << " ";
    preorder(root->left);
    preorder(root->right);
}

// 中序遍历
void inorder(Node* root) {
    if (root == nullptr) return;

    inorder(root->left);
    cout << root->key << " ";
    inorder(root->right);
}

// 释放整棵树
void destroy(Node* root) {
    if (root == nullptr) return;

    destroy(root->left);
    destroy(root->right);
    delete root;
}

int main() {
    int n, key;
    Node* root = nullptr;

    cout << "Enter number of nodes: ";
    cin >> n;

    if (n <= 0) {
        cout << "Invalid number of nodes." << endl;
        return 0;
    }

    cout << "Enter the keys: ";
    for (int i = 0; i < n; i++) {
        int value;
        cin >> value;
        root = insert(root, value);
    }//建立二叉树

    cout << "Enter target key: ";
    cin >> key;

    Node* parent = nullptr;
    Node* target = findNode(root, key, parent);

    if (target == nullptr) {
        cout << "Key not found." << endl;
        destroy(root);
        return 0;
    }

    // 只重建目标子树
    Node* newSubtree = rebuild(target);

    // 将重建后的子树接回原树
    if (parent == nullptr) {
        root = newSubtree;
    }
    else if (parent->left == target) {
        parent->left = newSubtree;
    }
    else {
        parent->right = newSubtree;
    }

    // 更新目标子树上方祖先的 size
    Node* current = parent;
    vector<Node*> ancestors;

    while (current != nullptr) {
        ancestors.push_back(current);
        current = nullptr; 
    }

    cout << "Preorder: ";
    preorder(newSubtree);
    cout << endl;

    cout << "Inorder: ";
    inorder(newSubtree);
    cout << endl;

    destroy(root);
    return 0;
}
