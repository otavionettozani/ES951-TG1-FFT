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

//an fft algorith that occurs a limited number of times
//post multicore part program execution
void fftNTimes(Complex* vector, unsigned times, unsigned size);


//fully separates the vector in even and uneven indexes
void separate(Complex* vector, unsigned size);

//a full fft algorithm
void fft(Complex* vector, unsigned size);
