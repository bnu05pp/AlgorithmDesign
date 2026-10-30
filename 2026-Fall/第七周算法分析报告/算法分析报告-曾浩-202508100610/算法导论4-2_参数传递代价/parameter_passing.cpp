#include <algorithm>
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

struct Stats {
    long long calls = 0;
    long long copiedElements = 0;  // Only copies made to pass parameters.
    long long copybackElements = 0;
    long long scalarMultiplications = 0;
};

enum class Mode { pointer, fullCopy, subrangeCopy };

int binaryPointer(const vector<int>& a, int lo, int hi, int key, Stats& s) {
    ++s.calls;
    if (lo >= hi) return -1;
    int mid = lo + (hi - lo) / 2;
    if (a[mid] == key) return mid;
    return key < a[mid] ? binaryPointer(a, lo, mid, key, s)
                        : binaryPointer(a, mid + 1, hi, key, s);
}

int binaryFull(vector<int> a, int lo, int hi, int key, Stats& s) {
    ++s.calls;
    s.copiedElements += static_cast<long long>(a.size());
    if (lo >= hi) return -1;
    int mid = lo + (hi - lo) / 2;
    if (a[mid] == key) return mid;
    return key < a[mid] ? binaryFull(a, lo, mid, key, s)
                        : binaryFull(a, mid + 1, hi, key, s);
}

int binarySlice(const vector<int>& a, int key, int offset, Stats& s) {
    ++s.calls;
    if (a.empty()) return -1;
    int mid = static_cast<int>(a.size()) / 2;
    if (a[mid] == key) return offset + mid;
    int lo = key < a[mid] ? 0 : mid + 1;
    int hi = key < a[mid] ? mid : static_cast<int>(a.size());
    vector<int> next(a.begin() + lo, a.begin() + hi);
    s.copiedElements += static_cast<long long>(next.size());
    return binarySlice(next, key, offset + lo, s);
}

// CLRS MERGE, translated to zero-based half-open ranges [lo, hi).
void mergeInto(vector<int>& a, int lo, int mid, int hi) {
    vector<int> left(a.begin() + lo, a.begin() + mid);
    vector<int> right(a.begin() + mid, a.begin() + hi);
    size_t i = 0, j = 0;
    int k = lo;
    while (i < left.size() && j < right.size())
        a[k++] = left[i] <= right[j] ? left[i++] : right[j++];
    while (i < left.size()) a[k++] = left[i++];
    while (j < right.size()) a[k++] = right[j++];
}

void mergePointer(vector<int>& a, int lo, int hi, Stats& s) {
    ++s.calls;
    if (hi - lo <= 1) return;
    int mid = lo + (hi - lo) / 2;
    mergePointer(a, lo, mid, s);
    mergePointer(a, mid, hi, s);
    mergeInto(a, lo, mid, hi);
}

vector<int> mergeVectors(const vector<int>& left, const vector<int>& right) {
    vector<int> out;
    out.reserve(left.size() + right.size());
    size_t i = 0, j = 0;
    while (i < left.size() && j < right.size())
        out.push_back(left[i] <= right[j] ? left[i++] : right[j++]);
    while (i < left.size()) out.push_back(left[i++]);
    while (j < right.size()) out.push_back(right[j++]);
    return out;
}

vector<int> mergeFull(vector<int> a, int lo, int hi, Stats& s) {
    ++s.calls;
    s.copiedElements += static_cast<long long>(a.size());
    if (hi - lo <= 1) return {a[lo]};
    int mid = lo + (hi - lo) / 2;
    vector<int> left = mergeFull(a, lo, mid, s);
    vector<int> right = mergeFull(a, mid, hi, s);
    return mergeVectors(left, right);
}

vector<int> mergeSlice(const vector<int>& a, Stats& s) {
    ++s.calls;
    if (a.size() <= 1) return a;
    size_t mid = a.size() / 2;
    vector<int> left(a.begin(), a.begin() + mid);
    vector<int> right(a.begin() + mid, a.end());
    s.copiedElements += static_cast<long long>(a.size());
    return mergeVectors(mergeSlice(left, s), mergeSlice(right, s));
}

struct Matrix {
    int side;
    vector<long long> data;
    explicit Matrix(int n) : side(n), data(static_cast<size_t>(n) * n) {}
    long long& at(int r, int c) { return data[static_cast<size_t>(r) * side + c]; }
    long long at(int r, int c) const { return data[static_cast<size_t>(r) * side + c]; }
};

// A view stores an address and two offsets; partitioning copies no elements.
struct MatrixView {
    Matrix* owner;
    int row = 0, col = 0;
    long long& at(int r, int c) const { return owner->at(row + r, col + c); }
    MatrixView sub(int r, int c) const { return {owner, row + r, col + c}; }
};

Matrix blockCopy(MatrixView source, int n, Stats& s) {
    Matrix out(n);
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            out.at(i,j) = source.at(i,j);
    s.copiedElements += static_cast<long long>(n) * n;
    return out;
}

void copyBack(MatrixView target, MatrixView source, int n, Stats& s) {
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            target.at(i,j) = source.at(i,j);
    s.copybackElements += static_cast<long long>(n) * n;
}

void matrixCore(MatrixView A, MatrixView B, MatrixView C,
                int n, Mode mode, Stats& s);

// A/B are read-only inputs. C is an input/output parameter (C += A * B).
// Copying C requires copying its active result back after the call.
void matrixCall(MatrixView A, MatrixView B, MatrixView C,
                int n, Mode mode, Stats& s) {
    if (mode == Mode::pointer) {
        matrixCore(A, B, C, n, mode, s);
    } else if (mode == Mode::fullCopy) {
        Matrix a = *A.owner, b = *B.owner, c = *C.owner;
        s.copiedElements += static_cast<long long>(a.data.size() + b.data.size() + c.data.size());
        MatrixView ca{&a, A.row, A.col}, cb{&b, B.row, B.col}, cc{&c, C.row, C.col};
        matrixCore(ca, cb, cc, n, mode, s);
        copyBack(C, cc, n, s);
    } else {
        Matrix a = blockCopy(A, n, s);
        Matrix b = blockCopy(B, n, s);
        Matrix c = blockCopy(C, n, s);
        MatrixView ca{&a}, cb{&b}, cc{&c};
        matrixCore(ca, cb, cc, n, mode, s);
        copyBack(C, cc, n, s);
    }
}

// CLRS MATRIX-MULTIPLY-RECURSIVE, textbook p.51, lines 1-15.
void matrixCore(MatrixView A, MatrixView B, MatrixView C,
                int n, Mode mode, Stats& s) {
    ++s.calls;
    if (n == 1) {
        C.at(0,0) += A.at(0,0) * B.at(0,0);
        ++s.scalarMultiplications;
        return;
    }
    int h = n / 2;
    auto A11 = A, A12 = A.sub(0,h), A21 = A.sub(h,0), A22 = A.sub(h,h);
    auto B11 = B, B12 = B.sub(0,h), B21 = B.sub(h,0), B22 = B.sub(h,h);
    auto C11 = C, C12 = C.sub(0,h), C21 = C.sub(h,0), C22 = C.sub(h,h);
    matrixCall(A11, B11, C11, h, mode, s);
    matrixCall(A11, B12, C12, h, mode, s);
    matrixCall(A21, B11, C21, h, mode, s);
    matrixCall(A21, B12, C22, h, mode, s);
    matrixCall(A12, B21, C11, h, mode, s);
    matrixCall(A12, B22, C12, h, mode, s);
    matrixCall(A22, B21, C21, h, mode, s);
    matrixCall(A22, B22, C22, h, mode, s);
}


Matrix schoolbook(const Matrix& a, const Matrix& b) {
    Matrix c(a.side);
    for (int i = 0; i < a.side; ++i)
        for (int k = 0; k < a.side; ++k)
            for (int j = 0; j < a.side; ++j)
                c.at(i,j) += a.at(i,k) * b.at(k,j);
    return c;
}

void show(const string& name, const Stats& s) {
    cout << name << ": calls=" << s.calls
         << ", parameter_elements_copied=" << s.copiedElements;
    if (s.scalarMultiplications)
        cout << ", scalar_multiplications=" << s.scalarMultiplications
             << ", result_elements_copied_back=" << s.copybackElements;
    cout << '\n';
}

int main(int argc, char* argv[]) {
    int n = 8;
    if (argc > 2) {
        cerr << "Usage: parameter_passing [power_of_two_N: 2..16]\n";
        return 1;
    }
    if (argc == 2) {
        try { n = stoi(argv[1]); }
        catch (...) { cerr << "N must be an integer.\n"; return 1; }
    }
    if (n < 2 || n > 16 || (n & (n - 1))) {
        cerr << "N must be 2, 4, 8 or 16.\n";
        return 1;
    }

    vector<int> sorted(n);
    iota(sorted.begin(), sorted.end(), 0);
    Stats bp, bf, bs;
    int key = -1;  // Follow the longest left path for power-of-two input sizes.
    int r1 = binaryPointer(sorted, 0, n, key, bp);
    int r2 = binaryFull(sorted, 0, n, key, bf);
    int r3 = binarySlice(sorted, key, 0, bs);
    if (r1 != -1 || r2 != -1 || r3 != -1) throw runtime_error("binary search mismatch");

    vector<int> reversed = sorted;
    reverse(reversed.begin(), reversed.end());
    Stats mp, mf, ms;
    vector<int> inPlace = reversed;
    mergePointer(inPlace, 0, n, mp);
    vector<int> fullSorted = mergeFull(reversed, 0, n, mf);
    vector<int> sliceSorted = mergeSlice(reversed, ms);
    if (inPlace != sorted || fullSorted != sorted || sliceSorted != sorted)
        throw runtime_error("merge sort mismatch");

    Matrix a(n), b(n);
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j) {
            a.at(i,j) = (i + j) % 5 - 2;
            b.at(i,j) = (2 * i + j) % 7 - 3;
        }
    Stats cp, cf, cs;
    Matrix initial(n);
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j) initial.at(i,j) = i - j + 1;
    Matrix expected = schoolbook(a,b);
    for (size_t i = 0; i < expected.data.size(); ++i) expected.data[i] += initial.data[i];
    Matrix p = initial, f = initial, s = initial;
    matrixCall({&a}, {&b}, {&p}, n, Mode::pointer, cp);
    matrixCall({&a}, {&b}, {&f}, n, Mode::fullCopy, cf);
    matrixCall({&a}, {&b}, {&s}, n, Mode::subrangeCopy, cs);
    if (p.data != expected.data || f.data != expected.data || s.data != expected.data)
        throw runtime_error("matrix multiplication mismatch");

    cout << "CLRS 4-2 parameter passing, N=" << n << "\n";
    cout << "All three versions of each algorithm produced identical correct results.\n";
    cout << "Parameter copies count A, B and C; result copy-back is reported separately.\n";
    cout << "Matrix test verifies C_final = C_initial + A*B with nonzero C_initial.\n\n";
    cout << "Binary search (absent key):\n";
    show("  pointer",bp); show("  full copy",bf); show("  subrange copy",bs);
    cout << "Merge sort (reverse input):\n";
    show("  pointer",mp); show("  full copy",mf); show("  subrange copy",ms);
    cout << "Recursive matrix multiplication:\n";
    show("  pointer",cp); show("  full copy",cf); show("  submatrix copy",cs);
    return 0;
}
