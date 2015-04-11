#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <e-hal.h>
#include "messages.h"
#include "fft.h"

#define RAM_SIZE (0x8000)
#define VECTOR_SIZE (262144)

void clearFlags(){


	e_epiphany_t dev;
	int clear[2] = {0,0};
	unsigned int i,j;

	for(i=0;i<4;i++){
		for(j=0;j<4; j++){
			e_open(&dev,i,j,1,1);
			e_reset_group(&dev);
			e_write(&dev,0,0,COMMADDRESS_BUSY,clear,2*sizeof(int));
			e_close(&dev);
		}
	}
	usleep(10000);

}

void dumpMemory(void* dev, unsigned row, unsigned col){

	printf("adsasdadads");
	unsigned read_buffer[RAM_SIZE/4], k;
	unsigned read_data;
	unsigned addr;
	char filename[9] = "logs2.txt";
	FILE* file = fopen(filename,"w");

	fprintf(file,"(ROW,COL)   ADDRESS   DATA\n");
	fprintf(file,"-----------------------------\n");

	for(k=0;k<RAM_SIZE/4;k++){
		addr=4*k;
		e_read(&dev, row, col, addr, &read_data, sizeof(int));
		read_buffer[k]=read_data;
	}
	for(k=0;k<RAM_SIZE/4;k++){
		fprintf(file,"(%2d,%2d)     0x%08x   0x%08x\n",row,col,k*4,read_buffer[k]);
	}


}

void dataSent(void *dev, unsigned row, unsigned col, unsigned size){

	unsigned sizePointer[1];
	unsigned char bitOn[1];
	unsigned char bitOff[1];
	unsigned char ack[1];

	sizePointer[0] = size;
	bitOn[0] = 1;
	bitOff[0] = 0;

	//send to the core the size of the message sent
	e_write(dev, row, col, COMMADDRESS_SIZE, sizePointer, sizeof(unsigned));
	//set the data_to_epiphany bit to 1, so that the core knows that the data has been transfered
	e_write(dev, row, col,COMMADDRESS_DATA_TO_EPIPHANY,bitOn,sizeof(char));

	//wait for the core to acknowledge the receiving of the data
	while (!ack[0]){
		e_read(dev,row,col,COMMADDRESS_EPIPHANY_ACK,ack,sizeof(char));
	}
	return;
}



int main(){


	e_platform_t platform;
	e_epiphany_t dev;
	e_mem_t memory;

	e_return_stat_t result;

	unsigned read_buffer[RAM_SIZE/4];
	unsigned read_data;
	unsigned addr;
	int i,j,k;


	char filename[9] = "logs.txt";
	FILE* file = fopen(filename,"w");

	e_init(NULL);
	e_get_platform_info(&platform);

	clearFlags();

//-------------------------------FFT----------------------

	//data for fft
	Complex vector[VECTOR_SIZE];

	//max elements to fit in the memory of the epiphany core
	unsigned maxElements = (0x8000 - COMMADDRESS_DATA)/sizeof(Complex);

	for(i=1;maxElements!=1;maxElements>>=1){
		i<<=1;
	}
	maxElements = i;


	//initialize data
	for (i=0; i<VECTOR_SIZE; i++){
		vector[i].real = i;
		vector[i].imaginary = 0;
	}

	//number of sections that the input vector will be divided
	unsigned sections = VECTOR_SIZE/maxElements < 16 ? (VECTOR_SIZE <16 ? VECTOR_SIZE : 16): VECTOR_SIZE/maxElements;
	unsigned sectionSize = VECTOR_SIZE/sections;

	char* processingSections;

	processingSections = (char*) malloc(sections*sizeof(char));
	for( i=0;i<sections;i++){
		processingSections[i]=0;
	}

	// steps taken before the processing of the vector;
	unsigned preprocessingSteps = 0;

	for(i = sections; i != 1; i>>=1){
		preprocessingSteps ++;
	}

	//a partir daqui a fft em si acontece ----------------------

	//PRE SEPARATE ----- Separa-se o vetor de modo a cada parte da fft que sera feita possa caber nos cores;
	separateNTimes(vector, preprocessingSteps,VECTOR_SIZE);


	//full fft in separated sections
	int row = 0, col = 0;
	char busyRead[1];
	char dataToArm[1];
	char armAck[1] = {1};
	char coresOrganization[4][4] = {{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}};
	int bufferSize[1]={sectionSize};
	i = 0;
	j=0;
	while(1){

	//open the cores
		e_open(&dev, row,col,1,1);

		e_read(&dev,0,0,COMMADDRESS_BUSY,&busyRead,sizeof(char));


	//if the core is free, load the program and write to the memory the data
	printf("core %d, %d = %d\n",row, col, busyRead[0]);
		if(!busyRead[0]){

			e_load("epiphanyProgram.srec",&dev,0,0,E_TRUE);
			e_write(&dev,0,0,COMMADDRESS_DATA,&vector[i*sectionSize],sectionSize*sizeof(Complex));
			coresOrganization[row][col] = i;
			i++;
			dataSent(&dev,0,0,sectionSize*sizeof(Complex));

		}else{
	// if the cores are not free, check if they have data to pass on
			int position = coresOrganization[row][col];
			e_read(&dev,0,0,COMMADDRESS_DATA_TO_ARM,&dataToArm,sizeof(char));

			if(dataToArm[0]){
				e_read(&dev,0,0,COMMADDRESS_DATA,&vector[position*sectionSize],sectionSize*sizeof(Complex));
				e_write(&dev,0,0,COMMADDRESS_ARM_ACK,&armAck[0],sizeof(char));
				processingSections[position]=1;

				j++;
				e_reset_group(&dev);

				int counter;
				char hasEnded = 1;
				for(counter=0; counter<sections;counter++){
					if(processingSections[counter]==0){
						hasEnded = 0;
						break;
					}
				}

				if(hasEnded){
					break;
				}

			}

		}

		//go to next core

		e_close(&dev);
		row ++;
		col = (row = row%4) ? col : col+1;

		if(col == 4){
			col = row = 0;

		}
		col = col%4;
	}



	//final multicore junctions in the rest of the vector


	//final singlecore junctions in the rest of the vector



	//print the result
	fprintf(file,"results:\n");
	for(i=0; i<VECTOR_SIZE;i++){
		fprintf(file,"(%g, %g)\n",vector[i].real,vector[i].imaginary);
	}


	//return 0;

//-------------------------DUMP MEMORY -----------------------------
	//read all memory
	e_open(&dev, 0, 0, platform.rows, platform.cols);
	fprintf(file,"(ROW,COL)   ADDRESS   DATA\n");
	fprintf(file,"-----------------------------\n");
	for (i=0; i<(platform.rows); i++) {
		for (j=0; j<(platform.cols); j++) {
			for(k=0;k<RAM_SIZE/4;k++){
				addr=4*k;
				e_read(&dev, i, j, addr, &read_data, sizeof(int));
				read_buffer[k]=read_data;
			}
			for(k=0;k<RAM_SIZE/4;k++){
				fprintf(file,"(%2d,%2d)     0x%08x   0x%08x\n",i,j,k*4,read_buffer[k]);
			}
		}
	}


	fclose(file);
	e_close(&dev);
	e_finalize();

	return EXIT_SUCCESS;

}
