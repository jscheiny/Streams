int main() {
    Stream<int> s = MakeStream::range(1, 10)
        | map([](int x) { return x*x; })
        | filter([](int y) { return y % 2 == 0; });
}
