#include <stdio.h>
#include <math.h>
#include "fft.h"


//complex functions
Complex CSum(Complex a, Complex b){
	Complex c;

	c.real = a.real+b.real;
	c.imaginary = a.imaginary+b.imaginary;

	return c;
}

Complex CSub(Complex a, Complex b){
	Complex c;

	c.real = a.real-b.real;
	c.imaginary = a.imaginary-b.imaginary;

	return c;
}

Complex CMul(Complex a, Complex b){
	Complex c;

	c.real = a.real*b.real-a.imaginary*b.imaginary;
	c.imaginary = a.imaginary*b.real+a.real*b.imaginary;

	return c;
}


void separateNTimes(Complex* vec, unsigned times, unsigned size){
	unsigned i, halfSize = size/2;
	Complex aux;
	if(times == 0){
		return;
	}

	for(i=0; i<halfSize/2; i++){
		aux = vec[2*i+1];
		vec[2*i+1] = vec[2*i+halfSize];
		vec[2*i+halfSize] = aux;
	}


	separateNTimes(vec,times-1,halfSize);
	separateNTimes(&vec[halfSize],times-1,halfSize);
}


void fftNTimes(Complex* vector, unsigned times, unsigned size){

	unsigned halfSize = size/2, i;
	if(times == 0){
		return;
	}

	fftNTimes(vector,times-1,halfSize);
	fftNTimes(&vector[halfSize], times-1,halfSize);

	for(i=0; i<halfSize; i++){
		Complex Wnk, sum, sub, mul;

		Wnk.real = cos(-2.*PI*(((double)i)/((double)size)));
		Wnk.imaginary = sin(-2.*PI*(((double)i)/((double)size)));

		mul = CMul(vector[halfSize+i],Wnk);
		sum = CSum(vector[i],mul);
		sub = CSub(vector[i],mul);


		vector[i].real = sum.real;
		vector[i].imaginary = sum.imaginary;
		vector[i+halfSize].real = sub.real;
		vector[i+halfSize].imaginary = sub.imaginary;
	}

	return;

}
