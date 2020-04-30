#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <complex.h>

int P, N, step;
double *v;
typedef double complex cplx;
cplx *buf, *out;
pthread_barrier_t my_barrier;

void _fft(cplx buf[], cplx out[], int n, int step)
{
	if (step < n) {
		_fft(out, buf, n, step * 2);
		_fft(out + step, buf + step, n, step * 2);
 
		for (int i = 0; i < n; i += 2 * step) {
			cplx t = cexp(-I * M_PI * i / n) * out[i + step];
			buf[i / 2]     = out[i] + t;
			buf[(i + n)/2] = out[i] - t;
		}
	}
}

void fft(cplx buf[], int n)
{
	for (int i = 0; i < n; i++) out[i] = buf[i];
	_fft(buf, out, n, 1);
}

void *threadFunction1(void *var) {

	fft(buf, N);
	return 0;
}

void *threadFunction2(void *var) {
	int thread_id = *(int*) var;

	for (int i = 0; i < N; i++)
	 out[i] = buf[i];
 	if (thread_id == 0) {
 		_fft(out, buf, N, 2);
 	} else { 
 		_fft(out + 1, buf + 1, N, 2);
 	}

 	return 0;
}

void *threadFunction4(void *var) {
	int thread_id = *(int *) var;
	
	if (thread_id == 0) {
		_fft(out, buf, N, 4);
	} else {
		if (thread_id == 1) {
			_fft(out + 2, buf + 2, N, 4);
		} else { 
			if (thread_id == 2) {
				_fft(out + 1, buf + 1, N, 4);
			} else {
				if (thread_id == 3) {
					_fft(out + 3, buf + 3, N, 4);
				}
			}
		}
	}
		pthread_barrier_wait(&my_barrier);
	
	return 0;
	
}

int main(int argc, char * argv[])
{
	P = atoi(argv[3]);
	int i;
	pthread_barrier_init(&my_barrier, NULL, 4);

	pthread_t tid[P];
	int thread_id[P];
	for(i = 0;i < P; i++)
		thread_id[i] = i;

	
	FILE *in, *output;
	N = 0;

	in = fopen(argv[1], "r");
	int ret = fscanf(in, "%d", &N);

	if(ret < 0) {
		return 0;
	}
	output = fopen(argv[2], "w");
	fprintf(output, "%d\n", N);

	v = malloc(N * sizeof(double)); 
	buf = malloc(N * sizeof(cplx));
	out = malloc(N * sizeof(cplx));

	for (int k = 0; k < N; ++k) {
		ret = fscanf(in, "%lf", &v[k]);

		if(ret < 0) {
			return 0;
		}
		
		buf[k] = creal(v[k]);
	}

	if (P ==  1) {
		pthread_create(&(tid[0]), NULL, threadFunction1, &(thread_id[0]));
	} else {
		if (P == 2) {
			for(i = 0; i < P; i++) {
				pthread_create(&(tid[i]), NULL, threadFunction2, &(thread_id[i]));
			}
		}	
	} 

	if (P == 4) {
		for (int i = 0; i < N; i++)
	out[i] = buf[i];
 	
		for(i = 0; i < P; i++) {
			pthread_create(&(tid[i]), NULL, threadFunction4, &(thread_id[i]));
		}
	}

	for(i = 0; i < P; i++) {
		pthread_join(tid[i], NULL);
	}
	
	if( P == 2) {
		int step = 1;
		for (int i = 0; i < N; i += 2 * step) {
			cplx t = cexp(-I * M_PI * i / N) * out[i + step];
			buf[i / 2]     = out[i] + t;
			buf[(i + N)/2] = out[i] - t;
		}
	}

	if( P == 4) {
		step = 1;
		for (int i = 0; i < N; i += 4 * step) {
			cplx t = cexp(-I * M_PI * i / N) * out[i + 2 * step];
			buf[i / 2]     = out[i] + t;
			buf[(i + N)/2] = out[i] - t;
		}		
		

		step = 1;
		for (int i = 0; i < N; i += 4 * step) {
			cplx t = cexp(-I * M_PI * i / N) * out[i +  2 * step + 1];
			buf[i / 2 +1]     = out[i + 1] + t;
			buf[(i + N)/ 2 + 1] = out[i + 1] - t;
		}		
		
		step = 1;
		for (int i = 0; i < N; i += 2 * step) {
			cplx t = cexp(-I * M_PI * i / N) * buf[i + step];
			out[i / 2]     = buf[i] + t;
			out[(i + N)/2] = buf[i] - t;
		}
		buf = out;
	}
	
	pthread_barrier_destroy(&my_barrier);

	for (i = 0; i < N; ++i) {
		fprintf(output, "%lf %lf\n", creal(buf[i]), cimag(buf[i]));
	}

	fclose(in);
	fclose(output);
	free(v);
	free(buf);
	// free(out);

	return 0;
}
