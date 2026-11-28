// CLRS 8-3: linear-time sorting of variable-length data (C++17)
// Integers: positive decimal strings without leading zeros.
// Strings: lowercase letters a-z. Using string for integers avoids 64-bit overflow.
#include <algorithm>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
using namespace std;

// 8-3(a): group by digit length, then stable LSD radix sort within each group.
vector<string> sortVariableIntegers(const vector<string>& nums) {
    size_t maxLen = 0;
    for (const string& s : nums) maxLen = max(maxLen, s.size());

    vector<vector<string>> groups(maxLen + 1);
    for (const string& s : nums) groups[s.size()].push_back(s);

    vector<string> result;
    result.reserve(nums.size());
    for (size_t len = 1; len <= maxLen; ++len) {
        auto& group = groups[len];
        for (int pos = static_cast<int>(len) - 1; pos >= 0; --pos) {
            vector<vector<string>> buckets(10);
            for (string& s : group)
                buckets[s[pos] - '0'].push_back(move(s));
            group.clear();
            for (auto& bucket : buckets)
                for (string& s : bucket) group.push_back(move(s));
        }
        for (string& s : group) result.push_back(move(s));
    }
    return result;
}

// 8-3(b): MSD radix sort. Bucket 0 is the end-of-string sentinel.
void msdSort(vector<string> group, size_t pos, vector<string>& output) {
    if (group.size() <= 1) {
        for (string& s : group) output.push_back(move(s));
        return;
    }

    vector<vector<string>> buckets(27);  // 0=end, 1..26='a'..'z'
    for (string& s : group) {
        int id = (pos == s.size()) ? 0 : (s[pos] - 'a' + 1);
        buckets[id].push_back(move(s));
    }
    for (string& s : buckets[0]) output.push_back(move(s));
    for (int id = 1; id <= 26; ++id)
        if (!buckets[id].empty()) msdSort(move(buckets[id]), pos + 1, output);
}

vector<string> sortVariableStrings(vector<string> words) {
    vector<string> output;
    output.reserve(words.size());
    msdSort(move(words), 0, output);
    return output;
}

void print(const vector<string>& items) {
    for (size_t i = 0; i < items.size(); ++i)
        cout << (i == 0 ? "" : " ") << items[i];
    cout << '\n';
}

int main() {
    int m;
    cin >> m;
    vector<string> nums(m);
    for (string& s : nums) cin >> s;

    int k;
    cin >> k;
    vector<string> words(k);
    for (string& s : words) cin >> s;

    cout << "8-3(a) SORTED_INTEGERS: ";
    print(sortVariableIntegers(nums));
    cout << "8-3(b) SORTED_STRINGS: ";
    print(sortVariableStrings(move(words)));
}
