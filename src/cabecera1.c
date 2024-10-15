#include "cabecera1.h"


int iniciamemorias( ){
	//Inicializacion del motor DC
	IPMOTORDC_mWriteReg(DIR_IPMOTORDC, MOTORDC_RESET, 1);
	usleep(100);
	IPMOTORDC_mWriteReg(DIR_IPMOTORDC, MOTORDC_RESET, 0);
	IPMOTORDC_mWriteReg(DIR_IPMOTORDC, MOTORDC_SENTIDO, 1);
	IPMOTORDC_mWriteReg(DIR_IPMOTORDC, MOTORDC_DUTY, 0);
	IPMOTORDC_mWriteReg(DIR_IPMOTORDC, MOTORDC_FREQCLK, 100000);
	IPMOTORDC_mWriteReg(DIR_IPMOTORDC, MOTORDC_FREQPWM, 100);
	//Inicializacion del sensor HALL
	SENSORHALL_mWriteReg(DIR_SENSORHALL, RESET_HALL, 1);
	usleep(100);
	SENSORHALL_mWriteReg(DIR_SENSORHALL, RESET_HALL, 0);

	//Inicializacion del PID con  Kp 0.42418  Ki 7.1543 = 65536*valor
	Xil_Out32((DIR_PID) + (PID_RESET), 0x1);
	usleep(100);
	Xil_Out32((DIR_PID) + (PID_ENABLE), 0x1);
	Xil_Out32((DIR_PID) + (PID_KP), ceil(65536*0.004));
	Xil_Out32((DIR_PID) + (PID_KI), ceil(1.1543*65536));
	//Inicializacion del contador
	MYIPFILTRO2324_mWriteReg(DIR_ENTRADAPIN, FILTROAV_RESET, 1);
	usleep(100);
	MYIPFILTRO2324_mWriteReg(DIR_ENTRADAPIN, FILTROAV_RESET, 0);
	CONTADORDESCENDENTE_mWriteReg(DIR_CONTADOR, CONTADOR_RESET, 1);
	usleep(100);
	CONTADORDESCENDENTE_mWriteReg(DIR_CONTADOR, CONTADOR_RESET, 0);
	//Inicializacion del motor Steper
	MYIPPASOAPASO_mWriteReg(DIR_PASOAPASO, RESET_STEPPER, 1);
	usleep(100);
	MYIPPASOAPASO_mWriteReg(DIR_PASOAPASO, RESET_STEPPER, 0);
	MYIPPASOAPASO_mWriteReg(DIR_PASOAPASO, FREC_RELOJ_STEPPER, 100000000);
	MYIPPASOAPASO_mWriteReg(DIR_PASOAPASO, ENABLE_STEPPER, 0);
	MYIPPASOAPASO_mWriteReg(DIR_PASOAPASO, DIR_STEPPER, 1);
	MYIPPASOAPASO_mWriteReg(DIR_PASOAPASO, FREC_PASO_STEPPER, 0);
	//Inicializacion del RGB
	MYIPRGB3PINES_mWriteReg(DIR_RGB1, RGB_RESET, 1);
	usleep(100);
	MYIPRGB3PINES_mWriteReg(DIR_RGB1, RGB_RESET, 0);
	MYIPRGB3PINES_mWriteReg(DIR_RGB1, RGB_FRECCLK, 100000000);
	MYIPRGB3PINES_mWriteReg(DIR_RGB1, RGB_FRECPWM, 10000);

	MYIPRGB3PINES_mWriteReg(DIR_RGB2, RGB_RESET, 1);
	usleep(100);
	MYIPRGB3PINES_mWriteReg(DIR_RGB2, RGB_RESET, 0);
	MYIPRGB3PINES_mWriteReg(DIR_RGB2, RGB_FRECCLK, 100000000);
	MYIPRGB3PINES_mWriteReg(DIR_RGB2, RGB_FRECPWM, 10000);
	//Inicializacion del dht11
	MY_IP_TEMP_Y_UMEDAD_mWriteReg(DIR_TEMPERATURA, RESET_TEMPERATURA, 0x1);
	usleep(1000);
	MY_IP_TEMP_Y_UMEDAD_mWriteReg(DIR_TEMPERATURA, RESET_TEMPERATURA, 0x0);
	sleep(1);
	//Inicializacion del sensor de distancia
	MYIPSENSORULTRASOUND_mWriteReg(DIR_DISTANCIA, RESET_HCS, 1);
	usleep(100);
	MYIPSENSORULTRASOUND_mWriteReg(DIR_DISTANCIA, RESET_HCS, 0);
	MYIPSENSORULTRASOUND_mWriteReg(DIR_DISTANCIA, FREC_RELOJ_HCS, 100000000);
	//Inicializacion del servo
	IPSERVOMOTOR_mWriteReg(DIR_SERVO, SERVORESET, 1);
	usleep(10);
	IPSERVOMOTOR_mWriteReg(DIR_SERVO, SERVORESET, 0);
}

float temperaturaNTC(){

	int temperatura_entera, temperatura_decimal;
	    float temperatura_verdad = 0.0, bit3, bit2 , bit1, bit0;
 	temperatura_entera  = MY_IP_DS18B20_AZM_mReadReg(DIR_TERMOMETRO,TEMP_INT);
 	temperatura_decimal = MY_IP_DS18B20_AZM_mReadReg(DIR_TERMOMETRO,TEMP_DEC);
 	if(temperatura_decimal & 0x8){
 		bit0=0.5;
 	}
 	else{
 		bit0=0;
 	}
 	if(temperatura_decimal & 0x4){
 	    		bit1=0.25;
 	    	}
 	else{
 		bit1=0;
 	}
 	if(temperatura_decimal & 0x2){
 	    		bit2=0.125;
 	    	}
 	else{
 		bit2=0;
 	}
 	if(temperatura_decimal & 0x1){
 	    		bit3=0.0625;
 	    	}
 	else{
 		bit3=0;
 	}
 	temperatura_verdad=temperatura_entera+bit0+bit1+bit2+bit3;
				 return temperatura_verdad;
}
int RGB1(int R, int G, int B){
	   MYIPRGB3PINES_mWriteReg(DIR_RGB1, RGB_R, R);
	   MYIPRGB3PINES_mWriteReg(DIR_RGB1, RGB_G, G);
	   MYIPRGB3PINES_mWriteReg(DIR_RGB1, RGB_B, B);
	   return  MYIPRGB3PINES_mReadReg(DIR_RGB1, RGB_ERROR);
}
int RGB2(int R, int G, int B){
	   MYIPRGB3PINES_mWriteReg(DIR_RGB2, RGB_R, R);
	   MYIPRGB3PINES_mWriteReg(DIR_RGB2, RGB_G, G);
	   MYIPRGB3PINES_mWriteReg(DIR_RGB2, RGB_B, B);
	   return  MYIPRGB3PINES_mReadReg(DIR_RGB2, RGB_ERROR);
}
int distancia(){
	return MYIPSENSORULTRASOUND_mReadReg(DIR_DISTANCIA,DIST_HCS);
}
int motorDCfun(int Sentido, int Duty){
	int DutyPI = 0;
	Xil_Out32((DIR_PID) + (PID_Consigna), ceil(Duty*65536));
	Xil_Out32((DIR_PID) + (PID_SENSOR),  SENSORHALL_mReadReg(DIR_SENSORHALL, SALIDA_HALL));
	Xil_Out32(DIR_PID+PID_STROBE, 0x00000001);
	usleep(10);
	Xil_Out32(DIR_PID+PID_STROBE, 0x00000000);
	DutyPI= Xil_In32((DIR_PID) + (PID_OUTPUT));
	IPMOTORDC_mWriteReg(DIR_IPMOTORDC, MOTORDC_SENTIDO, Sentido);
	IPMOTORDC_mWriteReg(DIR_IPMOTORDC, MOTORDC_DUTY, DutyPI);
 	return SENSORHALL_mReadReg(DIR_SENSORHALL, SENTIDO_HALL);
}
void motorPasoAPaso(int Direccion, int paso){
	MYIPPASOAPASO_mWriteReg(DIR_PASOAPASO, ENABLE_STEPPER, 1);
	MYIPPASOAPASO_mWriteReg(DIR_PASOAPASO, DIR_STEPPER, Direccion);
	MYIPPASOAPASO_mWriteReg(DIR_PASOAPASO, FREC_PASO_STEPPER, paso);
	if(paso == 0){
		MYIPPASOAPASO_mWriteReg(DIR_PASOAPASO, ENABLE_STEPPER, 0);
	}
}

float velocidad(){
	float rpm_secundario;
	int velocidad_us;
	velocidad_us = SENSORHALL_mReadReg(DIR_SENSORHALL, SALIDA_HALL);
	rpm_secundario=60000000.0/(velocidad_us*120.0);
	return rpm_secundario;
}
float temperaturaDHT11(){
	u16 Temperatura;
	int temperatura_intero;
	int temperatura_decimal;
	float TemperaturaF = 0.0, bit7, bit6 , bit5, bit4, bit3, bit2 , bit1;
	MY_IP_TEMP_Y_UMEDAD_mWriteReg(DIR_TEMPERATURA, QUE_VER_TEMPERATURA, 1);
	Temperatura = MY_IP_TEMP_Y_UMEDAD_mReadReg(DIR_TEMPERATURA, LED_TEMPERATURA);

	    		temperatura_intero = (Temperatura&0xFF00)>>8; // Mascara con 1111111100000000 y deslocado 8 bits a la derecha
	    		temperatura_decimal = (Temperatura&0x00FF);	  // Mascara con 0000000011111111
	    		if(temperatura_decimal & 0x1){
	    		 		bit7=0.5;
	    		 	}
	    		 	else{
	    		 		bit7=0;
	    		 	}
	    		 	if(temperatura_decimal & 0x2){
	    		 	    		bit6=0.25;
	    		 	    	}
	    		 	else{
	    		 		bit6=0;
	    		 	}
	    		 	if(temperatura_decimal & 0x4){
	    		 	    		bit5=0.125;
	    		 	    	}
	    		 	else{
	    		 		bit5=0;
	    		 	}
	    		 	if(temperatura_decimal & 0x8){
	    		 	    		bit4=0.0625;
	    		 	}
	    		 	else{
	    		 		bit4=0;
	    		 	}
	    		 	if(temperatura_decimal & 0x10){
	    		 	 		bit3=0.0312;
	    		 	 	}
	    		 	 	else{
	    		 	 		bit3=0;
	    		 	 	}
	    		 	 	if(temperatura_decimal & 0x20){
	    		 	 	    		bit2=0.0156;
	    		 	 	    	}
	    		 	 	else{
	    		 	 		bit2=0;
	    		 	 	}
	    		 	 	if(temperatura_decimal & 0x40){
	    		 	 	    		bit2=0.0078;
	    		 	 	    	}
	    		 	 	else{
	    		 	 		bit2=0;
	    		 	 	}
	    		 	 	if(temperatura_decimal & 0x80){
	    		 	 	    		bit1=0.0039;
	    		 	 	    	}
	    		 	 	else{
	    		 	 		bit1=0;
	    		 	 	}

	    		TemperaturaF = temperatura_intero+bit7+bit6+bit5+bit4+bit3+bit2+bit1;
 return TemperaturaF;

}
float humedadDHT11(){
	u16 humedad;
	u8 humedad_intero;
	u8 humedad_decimal;
	float humedadF = 0, bit7, bit6 , bit5, bit4, bit3, bit2 , bit1;
	MY_IP_TEMP_Y_UMEDAD_mWriteReg(DIR_TEMPERATURA, QUE_VER_TEMPERATURA, 0);
	 humedad = MY_IP_TEMP_Y_UMEDAD_mReadReg(DIR_TEMPERATURA, LED_TEMPERATURA);
	     		humedad_intero = (humedad&0xFF00)>>8; // Mascara con 1111111100000000 y deslocado 8 bits a la derecha
	     		humedad_decimal = (humedad&0x000F);	  // Mascara con 0000000011111111
	    		if(humedad_decimal & 0x1){
	    		 		bit7=0.5;
	    		 	}
	    		 	else{
	    		 		bit7=0;
	    		 	}
	    		 	if(humedad_decimal & 0x2){
	    		 	    		bit6=0.25;
	    		 	    	}
	    		 	else{
	    		 		bit6=0;
	    		 	}
	    		 	if(humedad_decimal & 0x4){
	    		 	    		bit5=0.125;
	    		 	    	}
	    		 	else{
	    		 		bit5=0;
	    		 	}
	    		 	if(humedad_decimal & 0x8){
	    		 	    		bit4=0.0625;
	    		 	    	}
	    		 	else{
	    		 		bit4=0;
	    		 	}
	    		 	if(humedad_decimal & 0x10){
	    		 	 		bit3=0.0312;
	    		 	 	}
	    		 	 	else{
	    		 	 		bit3=0;
	    		 	 	}
	    		 	 	if(humedad_decimal & 0x20){
	    		 	 	    		bit2=0.0156;
	    		 	 	    	}
	    		 	 	else{
	    		 	 		bit2=0;
	    		 	 	}
	    		 	 	if(humedad_decimal & 0x40){
	    		 	 	    		bit2=0.0078;
	    		 	 	    	}
	    		 	 	else{
	    		 	 		bit2=0;
	    		 	 	}
	    		 	 	if(humedad_decimal & 0x80){
	    		 	 	    		bit1=0.0039;
	    		 	 	    	}
	    		 	 	else{
	    		 	 		bit1=0;
	    		 	 	}
	 humedadF = humedad_intero+bit7+bit6+bit5+bit4+bit3+bit2+bit1;
 return humedadF;
}
void servogrados(float grados){
	//20000 20 ms
	float cuenta = 50;
	int POS= 0;
	cuenta= cuenta+(20000/180)*grados;
	POS= ceil(cuenta);
	IPSERVOMOTOR_mWriteReg(DIR_SERVO, ServoPWM, POS);

}
long escribeBRAM(int i, int ESTADO, float valor, u64 cuenta){
	Xil_Out32(DIR_BRAM+i,ESTADO);
	i=i+4;
	Xil_Out32(DIR_BRAM+i,valor);
	i=i+4;
	Xil_Out32(DIR_BRAM+i,cuenta);
	i=i+4;
	return i;
}
int leeBRAM(int i){
	int e = 0;
	while(e<i){
		printf("Error %d:\n Vale %4.f y paso durante %d \n",Xil_In32(DIR_BRAM+e), (float)Xil_In32(DIR_BRAM+e+4),Xil_In32(DIR_BRAM+e+8));
		e=e+12;
	}
	return e-i;
}
int uartbotones(u32 DatosRecividos){
	int valor = 0;
	if (DatosRecividos == 49)//El primer pulsador esta a 1?
	{
		valor = 1;
	}
	if (DatosRecividos == 50)//El segundo pulsador esta a 1?
	{
		valor = 2; //Para acceder a un uncio led ejemplo 0x1 0x2
	}
	if (DatosRecividos == 52)//El tercer pulsador esta a 1?
	{
		valor = 3; //Encender todas
	}
	if (DatosRecividos == 56)//El cuarto pulsador esta a 1?
	{
		valor = 4; //Apagar todas
	}
	return valor;
}
int contador(){
	int valor;
	valor = CONTADORDESCENDENTE_mReadReg(DIR_CONTADOR, CUENTA);
	if (valor > 150){
		valor = 0;
		CONTADORDESCENDENTE_mWriteReg(DIR_CONTADOR, CONTADOR_RESET, 1);
		usleep(100);
		CONTADORDESCENDENTE_mWriteReg(DIR_CONTADOR, CONTADOR_RESET, 0);
	}
	valor = 200 - valor*10;
	return valor;
}
