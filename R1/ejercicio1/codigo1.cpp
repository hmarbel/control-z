#include <cstdio>
#include <cstdint>
#include <vector>

class Input {
    static constexpr unsigned S = 1 << 20;
    char buf[S];
    unsigned pos = 0, len = 0;
    int get() {
        if (pos == len) {
            len = static_cast<unsigned>(fread(buf, 1, S, stdin));
            pos = 0;
            if (!len) return -1;
        }
        return buf[pos++];
    }
public:
    unsigned read() {
        int c;
        do { c = get(); } while (c >= 0 && c <= ' ');
        unsigned x = 0;
        while (c >= '0' && c <= '9') {
            x = x * 10 + c - '0';
            c = get();
        }
        return x;
    }
};
static Input in;

int main() {
    const unsigned n = in.read(), l = in.read(), r = in.read(), m = in.read();
    std::vector<unsigned> valid(n + 1);
    std::vector<int> balance(n + 1);
    int low = 0, high = 0;
    for (unsigned j = 1; j <= n; ++j) {
        unsigned a = in.read();
        valid[j] = valid[j - 1] + (a != 0);
        balance[j] = balance[j - 1] + (a == 0 ? 0 : (a >= m ? 1 : -1));
        if (balance[j] < low) low = balance[j];
        if (balance[j] > high) high = balance[j];
    }
    std::vector<unsigned> freq(high - low + 1);
    std::vector<unsigned long long> indices(high - low + 1);
    unsigned add = 0, remove = 0;
    unsigned long long count = 0, sum = 0, answer = 0, lengths = 0;
    int threshold = -low;
    for (unsigned j = 1; j <= n; ++j) {
        int next = balance[j] - low;
        // The query boundary moves by at most one balance bucket.
        if (next > threshold) {
            count += freq[next];
            sum += indices[next];
        } else if (next < threshold) {
            count -= freq[threshold];
            sum -= indices[threshold];
        }
        threshold = next;
        if (valid[j] >= l) {
            unsigned limit = valid[j] - l;
            while (add < j && valid[add] <= limit) {
                int b = balance[add] - low;
                ++freq[b];
                indices[b] += add;
                if (b <= threshold) { ++count; sum += add; }
                ++add;
            }
        }
        if (valid[j] > r) {
            unsigned limit = valid[j] - r;
            while (remove < add && valid[remove] < limit) {
                int b = balance[remove] - low;
                --freq[b];
                indices[b] -= remove;
                if (b <= threshold) { --count; sum -= remove; }
                ++remove;
            }
        }
        answer += count;
        lengths += count * j - sum;
    }
    printf("%llu %llu\n", answer, lengths);
}
