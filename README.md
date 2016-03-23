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


        // cold start for timing purposes
        pool.ParallelFor(0,N,scale,a,b);

        int ntrials = 10;
        double tperformance = 0.0;
        for (int i=0; i<ntrials; i++)
        {
            Timer timer([&](int elapsed) {
                cout << "Trial " << i << ": "<< elapsed*1e-6 << " ms\n";
                tperformance+=elapsed;
            });
            pool.ParallelFor(0,N,scale,a,b);
        }
        cout << "Average: " << tperformance*1e-6 / ntrials << " ms\n\n";



        return 0;
}

```
