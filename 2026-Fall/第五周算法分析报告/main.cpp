#include <iostream>
#include <vector>
#include <string>

using namespace std;

const int R = 256;
// MSD字符串排序
void MSD_sort(
    vector<string>& a,
    int left,
    int right,
    int d)
{
    // 当前区间只有一个元素
    if(right <= left)
        return;
    // 统计字符桶
    vector<int> count(R + 2,0);
//统计第d位字符,-1表示字符串结束
    for(int i=left;i<=right;i++)
    {
        int c;
        if(d >= a[i].length())
            c = -1;
        else
            c = (unsigned char)a[i][d];
        count[c+2]++;
    }
    // 转换为索引
    for(int r=0;r<R+1;r++)
    {
        count[r+1]+=count[r];
    }
    // 保存每个桶开始位置
    vector<int> start=count;
    vector<string> aux(right-left+1);
//根据第d位字符分桶
    for(int i=left;i<=right;i++)
    {
        int c;
        if(d>=a[i].length())
            c=-1;
        else
            c=(unsigned char)a[i][d];
        aux[count[c+1]++] = a[i];
    }

    // 放回原数组
    for(int i=left;i<=right;i++)
    {
        a[i]=aux[i-left];
    }
//递归处理非空字符桶,c=-1结束符不用继续
    for(int r=0;r<R;r++)
    {
        int l=left+start[r+1];
        int h=left+start[r+2]-1;
        if(l<h)
        {
            MSD_sort(
                a,
                l,
                h,
                d+1
            );
        }
    }
}

int main()
{
    int n;
    cin>>n;
    vector<string> arr(n);
    for(int i=0;i<n;i++)
    {
        cin>>arr[i];
    }
    MSD_sort(
        arr,
        0,
        n-1,
        0
    );
    for(auto &s:arr)
    {
        cout<<s<<" ";
    }
    return 0;
}
