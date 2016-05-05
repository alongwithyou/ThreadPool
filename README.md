# std::thread based thread pool
A simple std::thread based thread pool with a parallel-for loop implementation.


Example of how to perform STREAM benchmark scale operation in parallel:
```C++
        ThreadPool pool(8);

        int N = 1e7;
        auto a = (double*)calloc(N,sizeof(double));
        auto b = (double*)calloc(N,sizeof(double));
        for (int i=0; i<N; i++) { b[i] = i; }

        
        pool.ParallelFor (0, N, [&](int i) {
            a[i] = 4*b[i];
        }

        // compared to OpenMP

        #pragma omp parallel for
        for (int i = 0; i < N; i++) {
            a[i] = 4*b[i];
        }
     

```
