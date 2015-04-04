#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "e_lib.h"

#include "messages.h"

#include "fft.h"

int main(void){


	unsigned char *dataReceived;
	unsigned int *size;
	unsigned char *ack;
	unsigned char *busy;
	unsigned char *dataSent;
	unsigned char *armAck;
	Complex* data;

	//set the pointer to their variables
	size = (int*)(COMMADDRESS_SIZE);
	ack = (char*)(COMMADDRESS_EPIPHANY_ACK);
	dataReceived = (char*)(COMMADDRESS_DATA_TO_EPIPHANY);
	busy = (char*)(COMMADDRESS_BUSY);
	dataSent = (char*)(COMMADDRESS_DATA_TO_ARM);
	armAck = (char*)(COMMADDRESS_ARM_ACK);
	data = (Complex*)(COMMADDRESS_DATA);


	*dataSent = 0;
	//set the core as busy
	*busy = 1;
	//wait for signal that all data has been transfered
	while(!dataReceived[0]);
	//acknowledge receiving the data
	*ack = 1;
	//reset receive data bit
	*dataReceived = 0;

//--------------
//do all that matters
	unsigned quantity = size[0]/sizeof(Complex);
	fft(&data[0],quantity);


//----------------
	//up data sent flag
	*dataSent = 1;
	//down own ack

	//wait for arm to ack
	while(!armAck[0]);

	*ack = 0;

	//reset busy flag
	*busy = 0;

	*armAck = 0;


	return EXIT_SUCCESS;
}
