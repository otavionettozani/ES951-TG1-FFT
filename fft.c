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
	if(times == 0 || size == 1){
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
	if(times == 0 || size == 1){
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



void separate(Complex* vector, unsigned size){
	unsigned i, halfSize = size/2, quarterSize = halfSize/2;
	Complex aux;
	if(size == 1){
		return;
	}

	for(i=0; i<quarterSize; i++){
		aux = vector[2*i+1];
		vector[2*i+1] = vector[2*i+halfSize];
		vector[2*i+halfSize] = aux;
	}


	separate(vector,halfSize);
	separate(&vector[halfSize],halfSize);

}


void fft(Complex* vector, unsigned size){

	unsigned halfSize = size/2, i;


	if(size == 1){
		return;
	}

	//separate(vector, size);

	fft(vector,halfSize);
	fft(&vector[halfSize],halfSize);

	for(i=0; i<halfSize; i++){
		Complex Wnk, sum, sub, mul;

		float theta = ((float)i)/((float)size);
		Wnk.real = cos(-2.*PI*theta);
		Wnk.imaginary = sin(-2.*PI*theta);


		mul.real = vector[halfSize+i].real*Wnk.real-vector[halfSize+i].imaginary*Wnk.imaginary;
		mul.imaginary = vector[halfSize+i].real*Wnk.imaginary-vector[halfSize+i].imaginary*Wnk.real;
		//mul = CMul(vector[halfSize+i],Wnk);

		sum.real = vector[i].real+mul.real;
		sum.imaginary=vector[i].imaginary+mul.imaginary;
		//sum = CSum(vector[i],mul);

		sub.real = vector[i].real-mul.real;
		sub.imaginary=vector[i].imaginary-mul.imaginary;
		//sub = CSub(vector[i],mul);


		vector[i].real = sum.real;
		vector[i].imaginary = sum.imaginary;
		vector[i+halfSize].real = sub.real;
		vector[i+halfSize].imaginary = sub.imaginary;
	}

	return;

}
