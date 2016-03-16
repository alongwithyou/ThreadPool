
#include "ThreadPool.hh"

static double* a;
static double* b;

using namespace std;

int main () {

        ThreadPool pool(4);

        int N = 1e7;
        a = (double*)calloc(N,sizeof(double));
        b = (double*)calloc(N,sizeof(double));
        for (int i=0; i<N; i++) { b[i] = i; }

        SERIAL_OPERATION(scale, a[i]=4*b[i]);
        pool.ParallelFor<scale>(0,N);

        cin.get();
        return 0;
}
