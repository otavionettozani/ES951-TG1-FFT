#define PI (3.141592653589793)

typedef struct{
	double real;
	double imaginary;
} Complex;

//complex functions
Complex CSum(Complex a, Complex b);

Complex CSub(Complex a, Complex b);

Complex CMul(Complex a, Complex b);

//use the separate algorith only a limited number of times
//prepares the vector to the multi-core program to run fft
void separateNTimes(Complex* vec, unsigned times, unsigned size);

void fftNTimes(Complex* vector, unsigned times, unsigned size);
