
#include "ThreadPool.hh"

static double* a;
static double* b;

using namespace std;


void raw_scale(int i, double* a, double* b) {
        a[i]=4*b[i];
}

int main () {

        ThreadPool pool(4);

        int N = 1e7;
        a = (double*)calloc(N,sizeof(double));
        b = (double*)calloc(N,sizeof(double));
        for (int i=0; i<N; i++) { b[i] = i; }

        SERIAL_OPERATION(static_scale, a[i]=4*b[i]);
        pool.ParallelFor<static_scale>(0,N);

        pool.ParallelFor(0,N,raw_scale,a,b);


        cin.get();
        return 0;
}
