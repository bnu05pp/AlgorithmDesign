#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;


int HoarePartition(vector<int>& A, int p, int r)
{
    int x = A[p];        // 主元选最左边 A[p]
    int i = p - 1;
    int j = r + 1;
    while (true)
    {
        do {
            i++;
        } while (A[i] < x);

        do {
            j--;
        } while (A[j] > x);

        if (i >= j)
            return j;
        swap(A[i], A[j]);//遍历时间复杂度是o（n) 
    }
}

// Hoare 快排
void QuickSortHoare(vector<int>& A, int p, int r)
{
    if (p < r)
    {
        int q = HoarePartition(A, p, r);
        QuickSortHoare(A, p, q);      // 左区间包含 q
        QuickSortHoare(A, q + 1, r);//递归平均时间复杂度是o（logn） 
    }
}

int main()
{
   int temp[] = {5, 3, 8, 4, 2};
    vector<int> arr(temp, temp + sizeof(temp)/sizeof(int));

    cout << "Before sort: ";
    for (int i = 0; i < arr.size(); i++)
    {
        cout << arr[i] << " ";
    }
    cout << endl;
    QuickSortHoare(arr, 0, arr.size() - 1);
    cout << "After sort: ";
    for (int i = 0; i < arr.size(); i++)
    {
        cout << arr[i] << " ";
    }
    cout << endl;
    return 0;
}
