#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
using namespace std;
using namespace chrono;

struct Node {
    int val;
    int size;
    Node *l, *r;
    Node(int v) : val(v), size(1), l(nullptr), r(nullptr) {}
};

double alpha;
int rebuildCnt = 0;

void updateSize(Node* p) {
    if (!p) return;
    p->size = 1;
    if (p->l)  p->size += p->l->size;
    if (p->r)  p->size += p->r->size;
}

bool isUnbalanced(Node* p) {
    if (!p) return false;
    int s  = p->size;
    int ls = p->l ? p->l->size : 0;
    int rs = p->r ? p->r->size : 0;
    return (ls > alpha * s) || (rs > alpha * s);
}

bool search(Node* root, int key) {
    Node* cur = root;
    while(cur != nullptr) {
        if (cur->val == key) return true;
        else if (key < cur->val) cur = cur->l;
        else cur = cur->r;
    }
    return false;
}

void collect(Node* root, vector<int>& arr) {
    if (!root) return;
    collect(root->l, arr);
    arr.push_back(root->val);
    collect(root->r, arr);
}

Node* build(vector<int>& arr, int l, int r) {
    if (l > r) return nullptr;
    int mid = (l + r) / 2;
    Node* rt = new Node(arr[mid]);
    rt->l = build(arr, l, mid - 1);
    rt->r = build(arr, mid + 1, r);
    updateSize(rt);
    return rt;
}

Node* rebuild(Node* p) {
    rebuildCnt++;
    vector<int> arr;
    collect(p, arr);
    return build(arr, 0, (int)arr.size() - 1);
}

Node* insert(Node* root, int v) {
    if (!root) return new Node(v);
    if (v < root->val)
        root->l = insert(root->l, v);
    else
        root->r = insert(root->r, v);

    updateSize(root);
    if (isUnbalanced(root))
        return rebuild(root);
    return root;
}

Node* getMin(Node* p) {
    while(p && p->l) p = p->l;
    return p;
}

Node* remove(Node* root, int v) {
    if (!root) return nullptr;

    if (v < root->val) {
        root->l = remove(root->l, v);
    } else if (v > root->val) {
        root->r = remove(root->r, v);
    } else {
        if(!root->l) {
            Node* tmp = root->r;
            delete root;
            return tmp;
        }
        if(!root->r) {
            Node* tmp = root->l;
            delete root;
            return tmp;
        }
        Node* succ = getMin(root->r);
        root->val = succ->val;
        root->r = remove(root->r, succ->val);
    }

    updateSize(root);
    if(isUnbalanced(root))
        return rebuild(root);
    return root;
}

// 中序遍历，输出升序元素
void inorder(Node* rt) {
    if (!rt) return;
    inorder(rt->l);
    cout << rt->val << " ";
    inorder(rt->r);
}

int main() {
    alpha = 0.75;
    Node* tree = nullptr;
    rebuildCnt = 0;
    int op, val;

    while(true) {
        cout <<"\n===== 平衡树菜单 =====\n";
        cout << "1 插入\n";
        cout << "2 删除\n";
        cout << "3 搜索\n";
        cout << "4 查看重建次数\n";
        cout << "5 中序遍历打印树(升序)\n";
        cout << "0 退出程序\n";
        cout << "请输入操作编号：";
        cin >> op;

        if(op == 0) break;
        if(op == 1) {
            cout << "输入要插入的整数：";
            cin >> val;
            tree = insert(tree, val);
            cout << "已插入 " << val << endl;
        } else if(op == 2) {
            cout << "输入要删除的整数：";
            cin >> val;
            tree = remove(tree, val);
            cout << "执行删除 " << val << endl;
        } else if(op == 3) {
            cout << "输入要搜索的整数：";
            cin >> val;
            if(search(tree,val))
                cout << val << " 存在树中\n";
            else
                cout << val << " 不存在\n";
        } else if(op == 4) {
            cout << "重建总次数：" << rebuildCnt << endl;
        } else if(op == 5) {
            cout << "【中序遍历输出】：";
            inorder(tree);
            cout << endl;
        }
    }
    return 0;
}