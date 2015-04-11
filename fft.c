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
		aux.real = vector[2*i+1].real;
		aux.imaginary = vector[2*i+1].imaginary;
		vector[2*i+1].real = vector[2*i+halfSize].real;
		vector[2*i+1].imaginary = vector[2*i+halfSize].imaginary;
		vector[2*i+halfSize].real = aux.real;
		vector[2*i+halfSize].imaginary = aux.imaginary;
	}


	separate(vector,halfSize);
	separate(&vector[halfSize],halfSize);

}


void e_fft(Complex* vector, unsigned size){

	unsigned halfSize = size/2, i;

	if(size == 1){
		return;
	}

	separate(vector, size);

	e_fft(vector,halfSize);
	e_fft(&vector[halfSize],halfSize);

	for(i=0; i<halfSize; i++){
		Complex Wnk, sum, sub, mul;

		//taylor for sin and cos
		float divisor;
		switch(size){
			case 2:
				divisor = -6.2831/2.;
			break;
			case 4:
				divisor = -6.2831/4.;
			break;
			case 8:
				divisor = -6.2831/8.;
			break;
			case 16:
				divisor = -6.2831/16.;
			break;
			case 32:
				divisor = -6.2831/32.;
			break;
			case 64:
				divisor = -6.2831/64.;
			break;
			case 128:
				divisor = -6.2831/128.;
			break;
			case 256:
				divisor = -6.2831/256.;
			break;
			case 512:
				divisor = -6.2831/512.;
			break;
			case 1024:
				divisor = -6.2831/1024.;
			break;
			case 2048:
				divisor = -6.2831/2048.;
			break;
		}

		float fi = i;
		float theta = fi*divisor;
		float theta2 = theta*theta;
		float theta3 = theta2*theta;
		float theta4 = theta3*theta;
		float theta5 = theta4*theta;
		float theta6 = theta5*theta;
		float theta8 = theta6*theta2;

		float den2 = -0.5;
		float den3 = -0.166666;
		float den4 = 0.041666;
		float den5 = 0.008333;
		float den6 = -0.001388;
		float den8 = 0.000024;

		theta2 = theta2*den2;
		theta3 = theta3*den3;
		theta4 = theta4*den4;
		theta5 = theta5*den5;
		theta6 = theta6*den6;
		theta8 = theta8*den8;

		float s = theta+theta3+theta5;
		float c = 1+theta2+theta4+theta6+theta8;

		//Wnk.real = cos(-2.*PI*theta);
		//Wnk.imaginary = sin(-2.*PI*theta);

		Wnk.real = c;
		Wnk.imaginary = s;
		//taylor ended here


		mul.real = vector[halfSize+i].real*Wnk.real-vector[halfSize+i].imaginary*Wnk.imaginary;
		mul.imaginary = vector[halfSize+i].real*Wnk.imaginary+vector[halfSize+i].imaginary*Wnk.real;


		sum.real = vector[i].real+mul.real;
		sum.imaginary=vector[i].imaginary+mul.imaginary;


		sub.real = vector[i].real-mul.real;
		sub.imaginary=vector[i].imaginary-mul.imaginary;



		vector[i].real = sum.real;
		vector[i].imaginary = sum.imaginary;
		vector[i+halfSize].real = sub.real;
		vector[i+halfSize].imaginary = sub.imaginary;
	}

	return;

}
