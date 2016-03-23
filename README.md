# std::thread based thread pool
A simple std::thread based thread pool with a parallel-for loop implementation.


Example of how to perform STREAM benchmark scale operation in parallel:
```C++
void scale(int i, double* a, double* b) {
        a[i]=4*b[i];
}

int main () {

        ThreadPool pool(8);

        int N = 1e9;
        auto a = (double*)calloc(N,sizeof(double));
        auto b = (double*)calloc(N,sizeof(double));
        for (int i=0; i<N; i++) { b[i] = i; }

        {
            Timer timer([&](int elapsed) { cout << elapsed*1e-6 << " ms\n"; });
            pool.ParallelFor(0,N,scale,a,b);
        }


        return 0;
}

```
See example.cc for a more detailed example.
