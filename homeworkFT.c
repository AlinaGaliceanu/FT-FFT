#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

int P, N;
double *v, *res_re, *res_im;

void* threadFunction(void *var)
{
	int thread_id = *(int*)var;	
	double re, im;

	for(int i = N/P * thread_id ; i < thread_id * N/P + N/P; i++) {
		re = 0;
		im = 0;

		for (int j = 0; j < N; ++j) {
			double u = 2 * M_PI * i * j / N;
			re += v[j] * cos(u);
			im += -v[j] * sin(u);
		}

		res_re[i] = re;
		res_im[i] = im;
	}

	return 0;
}

int main(int argc, char * argv[])
{
	P = atoi(argv[3]);
	int i;

	pthread_t tid[P];
	int thread_id[P];

	for(i = 0;i < P; i++)
		thread_id[i] = i;

	
	FILE *in, *out;
	N = 0;

	in = fopen(argv[1], "r");
	int ret = fscanf(in, "%d", &N);
	
	if (ret < 0) {
		return 0;
	}

	out = fopen(argv[2], "w");
	fprintf(out, "%d\n", N);

	v = malloc(N * sizeof(double)); 
	res_re = malloc(N * sizeof(double)); 
	res_im = malloc(N * sizeof(double)); 

	for (int k = 0; k < N; ++k) {
		ret = fscanf(in, "%lf", &v[k]);
		if (ret < 0) {
			return 0;
		}
	}

	for(i = 0; i < P; i++) {
		pthread_create(&(tid[i]), NULL, threadFunction, &(thread_id[i]));
	}

	for(i = 0; i < P; i++) {
		pthread_join(tid[i], NULL);
	}

	for (i = 0; i < N; ++i) {
		fprintf(out, "%lf %lf\n", res_re[i], res_im[i]);
	}

	fclose(in);
	fclose(out);
	free(v);
	free(res_re);
	free(res_im);

	return 0;
}
