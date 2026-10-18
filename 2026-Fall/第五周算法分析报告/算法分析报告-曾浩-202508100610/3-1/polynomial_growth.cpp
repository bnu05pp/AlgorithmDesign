#include <cmath>
#include <iomanip>
#include <iostream>
#include <vector>

using namespace std;

long double normalizedValue(const vector<long double>& coefficients, long double n) {
    // Horner's method in t = 1/n evaluates p(n)/n^d without forming n^d.
    long double value = coefficients.front();
    for (size_t i = 1; i < coefficients.size(); ++i) {
        value = value / n + coefficients[i];
    }
    return value;
}

int main() {
    int degree;
    cout << "Degree d (0..20): ";
    if (!(cin >> degree) || degree < 0 || degree > 20) {
        cerr << "Degree must be between 0 and 20.\n";
        return 1;
    }

    vector<long double> coefficients(degree + 1);
    cout << "Enter a_0, a_1, ..., a_d: ";
    for (auto& coefficient : coefficients) {
        if (!(cin >> coefficient) || !isfinite(coefficient)) {
            cerr << "Invalid coefficient.\n";
            return 1;
        }
    }
    if (coefficients.back() <= 0) {
        cerr << "The leading coefficient a_d must be positive.\n";
        return 1;
    }

    long double k;
    cout << "Exponent k: ";
    if (!(cin >> k) || !isfinite(k)) {
        cerr << "Invalid exponent.\n";
        return 1;
    }

    cout << "\nAsymptotic conclusions (n -> infinity):\n";
    cout << "O(n^k):     " << (k >= degree ? "yes" : "no") << '\n';
    cout << "Omega(n^k): " << (k <= degree ? "yes" : "no") << '\n';
    cout << "Theta(n^k): " << (k == degree ? "yes" : "no") << '\n';
    cout << "o(n^k):     " << (k > degree ? "yes" : "no") << '\n';
    cout << "omega(n^k): " << (k < degree ? "yes" : "no") << '\n';

    cout << "\nSample ratios p(n) / n^k (illustration only):\n";
    cout << scientific << setprecision(6);
    for (long double n : {10.0L, 100.0L, 1000.0L}) {
        long double ratio = normalizedValue(coefficients, n) * pow(n, degree - k);
        cout << "n=" << setw(6) << fixed << setprecision(0) << n << "   ratio=";
        if (isfinite(ratio)) {
            cout << scientific << setprecision(6) << ratio << '\n';
        } else {
            cout << "outside floating-point range\n";
        }
    }
    cout << "The finite samples illustrate the theorem; they are not its proof.\n";
}
