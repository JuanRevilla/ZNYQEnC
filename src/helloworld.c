//Añadir uso de BRAM LECTURA, uso del timer, uso del servo
// y lectura de la uart y comentarlo adecuadamente
//Librerias necesarias contenidas todas en cabecera1.h con las funciones que no necesitan GPIO
#include <stdio.h>
#include "platform.h"
#include "cabecera1.h"
//Declaracion de las funciones que necesitan GPIO
int interruptorfuncion(XGpio interruptor, XGpio cal);
int FinalCarrera1(XGpio FC);
int FinalCarrera2(XGpio FC);
float temperaturaSPI(XGpio cs);
float corrienteSPI(XGpio cs);
float termoparSPI(XGpio cs);
int botones(XGpio pulsadoryled,int valor);
void luces(XGpio pulsadoryled, int valor);
//Variables globales de configuracion
XSpi SpiInstance;
XSpi_Config *ConfigPtr;
XUartLite UartLite;
u8 MCP3008_buffer[3];
//Añadir lo referente al timer cuando suba que si no me da pereza
int main()
{
	//Declaracion de variables de tipo GPIO que se usaran dentro del main
	XGpio cs, cal, FC, interruptor, pulsadoryled;
	XTmrCtr Timer0;

	u8 DatosRecividos;
	u32 cuenta;
	//Llamamos al init platform para poder usar las memorias
    init_platform();
    //Iniciamos los GPIO
	int Status = XGpio_Initialize(&pulsadoryled, DIR_BOTONESLED);
	if (Status != XST_SUCCESS)
		return XST_FAILURE;
	Status = XGpio_Initialize(&interruptor, DIR_SWICH);
	if (Status != XST_SUCCESS)
		return XST_FAILURE;
	Status = XGpio_Initialize(&cal, DIR_CAL);
	if (Status != XST_SUCCESS)
		return XST_FAILURE;
	Status = XGpio_Initialize(&cs, DIR_CS);
	if (Status != XST_SUCCESS)
		return XST_FAILURE;
	Status = XGpio_Initialize(&FC, DIR_FC);
	if (Status != XST_SUCCESS)
		return XST_FAILURE;
	Status = XUartLite_Initialize(&UartLite, DIR_UART);
	if(Status != XST_SUCCESS){
		return XST_FAILURE;
	 }
	XGpio_SetDataDirection(&pulsadoryled, 1, ~buttonMask);
	XGpio_SetDataDirection(&pulsadoryled, 2, ~LEDMask);
	XGpio_SetDataDirection(&interruptor, 1, ~buttonMask);
	XGpio_SetDataDirection(&cs, 1, ~LEDMask);
	XGpio_SetDataDirection(&cal, 1, ~LEDMask);
	XGpio_SetDataDirection(&FC, 1, ~buttonMask);
	XGpio_SetDataDirection(&FC, 2, ~buttonMask);
	//Iniciamos el SPI
	ConfigPtr = XSpi_LookupConfig(DIR_SPI);
	if (ConfigPtr == NULL) {
		return XST_FAILURE;
	}
	Status = XSpi_CfgInitialize(&SpiInstance, ConfigPtr, ConfigPtr->BaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	Status = XSpi_SetOptions(&SpiInstance,XSP_MASTER_OPTION|XSP_MANUAL_SSELECT_OPTION);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	XSpi_Start(&SpiInstance);
	XSpi_SetSlaveSelect(&SpiInstance, 0x0);
	XSpi_IntrGlobalDisable(&SpiInstance);
	//Iniciamos el timer
	Status = XTmrCtr_Initialize(&Timer0, DIR_TIMER);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}
		 XTmrCtr_SetOptions(&Timer0, 0, 0x0);
		 XTmrCtr_SetResetValue(&Timer0, 0, 0);
	//Iniciamos los perifericos diseñados en vivado
	iniciamemorias();
	//Declaracion de variables necesarias en el bucle
	int 	botones1= 0,botones1A = 0, FC1=0, FC2=0,
			sentido = 1, sentidoR = 0, duty=0, distancia1=0,
			distanciaM=0, interruptor1=0, paso = 200, luz1, luz2;
	int timeron=0, timeron1=0, timeron2=0, timeron3=0,
		contadorTermopar= 0, contadorNTC1=0, contadorPlaca=0, contadorConsumo=0;
	int i=0;
	short EstadoMensaje0 = 0,EstadoMensaje1 = 0,EstadoMensaje2= 0,
			EstadoMensaje3 = 0,  EstadoMensaje4 = 0, EstadoMensajeCaliente = 0;
	float temperaturaNTC1, temperaturaPlaca, consumo, temperaturaTermopar,
	temperaturaNTCA, temperaturaPlacaA, consumoA, termoparA,
			velocidad1, humedad, temperatura, grados;
	u8 DatosRecividosA = 0;
	char mensaje[200];
	//Iniciamos el OLED
    oledInit(IIC_SLAVE_ADDR, OLED_128x64, 0, 1);
    oledFill(0); // fill with black
    sleep(0.5);
    oledWriteString(1,1,"Mensaje",FONT_BIG);
    oledWriteString(1,4,"Oled 4",FONT_BIG);
    //Bucle
    while(1)
    {
    	   //Espacio para leer de perifericos
    		XUartLite_Recv(&UartLite, &DatosRecividos, 1);
    		if(DatosRecividos != DatosRecividosA){
    			DatosRecividosA = DatosRecividos;

    		}
    		else{
    			DatosRecividos = 0;
    		}
    	    FC1 				= FinalCarrera1(FC);
    	    FC2 				= FinalCarrera2(FC);
    	    distancia1			= distancia();
    	    velocidad1 			= velocidad();
    	    temperaturaNTC1 	= temperaturaNTC(); // lee mal
			temperaturaTermopar = termoparSPI(cs);
			temperaturaPlaca	= temperaturaSPI(cs);
			consumo				= corrienteSPI(cs);
			humedad 			= humedadDHT11();
			temperatura 		= temperaturaDHT11();
			//xil_printf("Humedad = %x Temperatura = %x\n", humedad, temperatura);

			//escribir en la bram

			if(temperaturaTermopar>=500){
						contadorTermopar++;
						usleep(100);
						termoparA=temperaturaTermopar*1000;
						timeron = 1;
			}
			else if(timeron == 1 && temperaturaTermopar<500){
						i=escribeBRAM(i, 1, termoparA, (contadorTermopar/10));
						contadorTermopar = 0;
						timeron = 0;
			}
			if(temperaturaNTC1>40){
						contadorNTC1 = contadorNTC1++;
						usleep(100);
						temperaturaNTCA=temperaturaNTC1;
						timeron1 = 1;
					}
			else if(timeron1 == 1 && temperaturaNTC1<=40){
					i=escribeBRAM(i, 2, temperaturaNTCA, (contadorNTC1/10));
					contadorNTC1 = 0;
					timeron1 = 0;
				}
			if(temperaturaPlaca>40){ //ESTO ESTA MAL
						contadorPlaca++;
						usleep(100);
						temperaturaPlacaA=temperaturaPlaca*1000;
						timeron2 = 1;
				}

			else if(timeron2 == 1 && temperaturaPlaca<=40)
			{
					i = escribeBRAM(i, 3, temperaturaPlacaA, (contadorPlaca/10));
					contadorPlaca= 0;
					timeron2 = 0;
			}

			if(consumo > 1){
						contadorConsumo++;
						usleep(100);
						consumoA=consumo*1000;
						timeron3 = 1;
			}
			else if(timeron3 == 1 && (consumo <= 1))
			{
									i=escribeBRAM(i, 4, consumoA, contadorConsumo);
									contadorConsumo = 0;
									timeron3 = 0;
			}

			if(DatosRecividos < 49){
    	    botones1=botones(pulsadoryled,botones1);
			}
			else{
				botones1 = uartbotones(DatosRecividos);
			}
    	    // Si cambia el boton que se pulsa
    	    if(botones1 != botones1A){
    	    	luces(pulsadoryled, botones1);
    	    	botones1A = botones1;
    	    }
    	    // estados del programa
    	    switch(botones1){
    	    	//Inicial no se ha pulsado un boton
				case(0):
					luz1=RGB1(10,50,10);
					luz2=RGB2(10,50,10);
					EstadoMensaje1 = 0;
					EstadoMensaje2 = 0;
					EstadoMensaje3 = 0;
					EstadoMensaje4 = 0;
					grados = 10;
					paso = 0;
					if(EstadoMensaje0 == 0){
						oledFill(0); // fill with black
						EstadoMensaje0 = 1;
					}

					sprintf(mensaje,"%.2f g/m",humedad);
					oledWriteString(1,1,mensaje,FONT_BIG);
					sprintf(mensaje,"%.2f C",temperatura);
					oledWriteString(1,4,mensaje,FONT_BIG);

				break;
				case(1):
					//Considerar añadir un int para no escribir todo el rato
					EstadoMensaje0 = 0;
					EstadoMensaje2 = 0;
					EstadoMensaje3 = 0;
					EstadoMensaje4 = 0;
					grados = 20;
					luz1=RGB1(20,20,0);
					luz2=RGB2(20,20,0);
					paso = contador(); //CAMBIAR ESTA LINEA AL CONTADOR
					if(EstadoMensaje1 == 0){
						oledFill(0);
						oledWriteString(1,1,"Marcha",FONT_BIG);
						EstadoMensaje1 = 1;
					}
					if(FC1)
					{
						xil_printf("FC1\n");
						sentido = 0;
						XGpio_DiscreteWrite(&cal, 1, 0);
					}
					if(FC2)
					{
						xil_printf("FC2\n");
						sentido = 1;
					}
					if(sentido)
					{
						XGpio_DiscreteWrite(&cal, 1, 0);
						if(distancia1 < 100) duty = distancia1;
							//Añadir el display de distanciaç
							oledWriteString(1,1,"Distancia",FONT_BIG);
							sprintf(mensaje,"%d  cm",distancia1);
							oledWriteString(1,4,mensaje,FONT_BIG);
					}
					else
					{
					duty = 0;
					if(temperaturaTermopar < 35)
					{
						// Añadir el sprintf
						interruptor1 = interruptorfuncion(interruptor,cal);
						oledWriteString(1,1,"Temperatura",FONT_BIG);
						sprintf(mensaje,"%.3f ºC   ",temperaturaPlaca);
						oledWriteString(1,4,mensaje,FONT_BIG);

						//Aqui peta lo comento
					}
					else
					{
						XGpio_DiscreteWrite(&cal, 1, 0);
						oledWriteString(1,1,"Cuidado",FONT_BIG);
						oledWriteString(1,4,"Quema",FONT_BIG);
					}
					//Añadir BRAM medimos la corriente y en paro y marcha la escribimos
					}
				break;
				case(2):
					EstadoMensaje0 = 0;
					EstadoMensaje1 = 0;
					EstadoMensaje3 = 0;
					EstadoMensaje4 = 0;
					sentidoR = sentido;
					luz1=RGB1(10,10,50);
					luz2=RGB2(10,10,50);
					if(EstadoMensaje2 == 0)
					{
						oledFill(0); // fill with black
						oledWriteString(1,1,"Paro",FONT_BIG);
						EstadoMensaje2 = 1;
					}
					if(FC1==1)
					{
						sentidoR = 2;
						XGpio_DiscreteWrite(&cal, 1, 0);
					}
					if(FC2==1)
					{
						XGpio_DiscreteWrite(&cal, 1, 0);
						sentidoR = 3;
					}
					switch (sentidoR)
					{
						case 0:
							paso = contador(); //CAMBIAR ESTA LINEA AL CONTADOR
							duty = 0;
							grados = 10;
							if(temperaturaTermopar < 35){
								interruptor1 = interruptorfuncion(interruptor,cal);
								oledWriteString(1,1,"Temperatura",FONT_BIG);
								sprintf(mensaje,"%.3f ºC   ",temperaturaPlaca);
								oledWriteString(1,4,mensaje,FONT_BIG);
							}
							else
							{
								XGpio_DiscreteWrite(&cal, 1, 0);
								oledFill(0);
								oledWriteString(1,1,"Cuidado",FONT_BIG);
								oledWriteString(1,4,"Quema",FONT_BIG);
							}
						break;
						case 1:
							grados = 20;
							paso = contador(); //CAMBIAR ESTA LINEA AL CONTADOR
							XGpio_DiscreteWrite(&cal, 1, 0);
							if(distancia1 < 100) duty = distancia1;
							//Añadir el display de distancia
							sprintf(mensaje,"%d  cm",distancia1);
							oledWriteString(1,4,mensaje,FONT_BIG);
						break;
						case 2:
							paso = 0;
							duty = 0;
							grados = 30;
							xil_printf("FC1 alcanzado parando proceso \n");
						break;
						case 3:
							paso = 0;
							duty = 0;
							grados = 40;
							xil_printf("FC2 alcanzado parando proceso \n");
						break;
					}
				break;
				case(3):
					duty = 0;
					EstadoMensaje0 = 0;
					EstadoMensaje2 = 0;
					EstadoMensaje1 = 0;
					EstadoMensaje4 = 0;
					grados = 40;
					XGpio_DiscreteWrite(&cal, 1, 0);
					luz1=RGB1(0,10,10);
					luz2=RGB2(0,10,10);
					if(EstadoMensaje3 == 0){
						oledFill(0); // fill with black
						oledWriteString(1,1,"Reset",FONT_BIG);
						EstadoMensaje3 = 1;
					}
					if(distancia1 >= 23){
						sentido = 0;
						paso = 200;
						distanciaM = distancia1-23;
						oledWriteString(1,1,"Reiniciando ",FONT_BIG);
						itoa(distanciaM,mensaje, 10);
						oledWriteString(1,4,mensaje,FONT_BIG);
					}
					else if (distancia1 <= 20)
					{
						sentido = 1;
						paso = 200;
						distanciaM = 20-distancia1;
						oledWriteString(1,1,"Reiniciando ",FONT_BIG);
						itoa(distanciaM,mensaje, 10);
						oledWriteString(1,4,mensaje,FONT_BIG);
					}
					else
					{
						oledWriteString(1,1,"Reiniciado",FONT_BIG);
						paso = 0;
					}
				break;
				case(4):
					EstadoMensaje0 = 0;
					EstadoMensaje2 = 0;
					EstadoMensaje3 = 0;
					EstadoMensaje1 = 0;
					grados=50;
					luz1=RGB1(10,0,0);
					luz2=RGB2(20,0,0);
					XGpio_DiscreteWrite(&cal, 1, 0);
					if(timeron){
						i=escribeBRAM(i, 1, termoparA, (contadorTermopar/10));
						timeron2 = 0;
					}
					if(timeron1){
						i=escribeBRAM(i, 2, temperaturaNTCA, (contadorNTC1/10));
						timeron1 = 0;
					}
					if(timeron2){
						i = escribeBRAM(i, 3, temperaturaPlacaA, (contadorPlaca/10));
						timeron2 = 0;
					}
					if(timeron3){
						i=escribeBRAM(i, 4, consumoA, contadorConsumo);
						timeron3 = 0;
					}

					if(EstadoMensaje4 == 0){
						oledFill(0); // fill with black
						oledWriteString(1,1,"Alarma",FONT_BIG);
						EstadoMensaje4 = 1;
						i=leeBRAM(i);
					}
					duty = 0;
					paso = 0;

					if(luz1 || luz2){
						xil_printf("Fallo en el DT de las luces\n",temperaturaPlaca);
					}
					if(temperaturaTermopar > 35){
						printf("Temperatura de temopar alta %.2f\n",temperaturaTermopar);
					}
					if(temperaturaNTC1 > 30){
						printf("Temperatura de la cinta alta %.2f\n",temperaturaNTC1);
					}
					if(consumo > 1.5)
					{
						if(!EstadoMensajeCaliente){
							oledFill(0);
							oledWriteString(1,1,"CORTO",FONT_BIG);
							oledWriteString(1,4,"APAGAR",FONT_BIG);
							EstadoMensajeCaliente = 1;
						}
						else
						{
							EstadoMensajeCaliente = 0;
						}

					}
					else
					{
						if(temperaturaPlaca > 35 || temperaturaTermopar > 35)
						{
							if(!EstadoMensajeCaliente){
								oledFill(0);
								oledWriteString(1,1,"Placa",FONT_BIG);
								oledWriteString(1,4,"Caliente",FONT_BIG);
								EstadoMensajeCaliente = 1;
							}
						}
						else if(luz1 || luz2) {
								oledWriteString(1,1,"LUZ",FONT_BIG);
								oledWriteString(1,4,"ALTA",FONT_BIG);
						}
						else
						{
							EstadoMensajeCaliente = 0;
						}
					}
					//Añadir el codigo de la BRAM
				break;
    	    }
    	    motorPasoAPaso(sentido, paso);
    	    motorDCfun(sentido, duty);
    	    servogrados(grados);

    }

    cleanup_platform();
    return 0;
}

float termoparSPI(XGpio cs){
	float termopar;
	float temp_NTC=0, resist_NTC=0, tension_NTC=0, beta=3950.0, TO=293.15;
	double exponente=0.0, potencia=0.0;
	XGpio_DiscreteWrite(&cs, 1, 0);
	usleep(100);
	MCP3008_buffer[0]= 0x01;
	MCP3008_buffer[1]= 0xA0;
	MCP3008_buffer[2]= 0x00;
	XSpi_Transfer(&SpiInstance, MCP3008_buffer, MCP3008_buffer, 3);
	termopar=((MCP3008_buffer[1] & 0x3) *256 + MCP3008_buffer[2]);
	tension_NTC=termopar/310.30;
	resist_NTC=(100/tension_NTC)*(3.3-tension_NTC);
	exponente=-beta/TO;
	potencia = exp(exponente);
	temp_NTC=(beta/(log(resist_NTC/(100*potencia)))-273.15)*1.4;
	XGpio_DiscreteWrite(&cs, 1, 1);
	usleep(10);
	return temp_NTC;
}
float corrienteSPI(XGpio cs){
	float valADC_CORRIENTE;
	float corriente = 0.0;
	XGpio_DiscreteWrite(&cs, 1, 0);
	usleep(10);
	MCP3008_buffer[0]= 0x01;
	MCP3008_buffer[1]= 0x90;
	MCP3008_buffer[2]= 0x00;
	XSpi_Transfer(&SpiInstance, MCP3008_buffer, MCP3008_buffer, 3);
	valADC_CORRIENTE = (MCP3008_buffer[1] & 0x3) *256 + MCP3008_buffer[2];
	corriente = (valADC_CORRIENTE - 769) * (1.475 / (800 - 769));
	if(corriente < 0) corriente=0;
	XGpio_DiscreteWrite(&cs, 1, 1);
	usleep(10);
	return corriente;
}
float temperaturaSPI(XGpio cs){
	float valADC_TEMP;
	float temperatura= 0.0;
	XGpio_DiscreteWrite(&cs, 1, 0);
	usleep(10);
	MCP3008_buffer[0]= 0x01;
	MCP3008_buffer[1]= 0x80;
	MCP3008_buffer[2]= 0x00;
	XSpi_Transfer(&SpiInstance, MCP3008_buffer, MCP3008_buffer, 3);
	valADC_TEMP = (MCP3008_buffer[1] & 0x3) * 256 + MCP3008_buffer[2];
	temperatura = valADC_TEMP / (5000/1024);
	XGpio_DiscreteWrite(&cs, 1, 1);
	usleep(10);
	return temperatura;

}
int botones(XGpio pulsadoryled, int valor){
	if (XGpio_DiscreteRead(&pulsadoryled, 1) & (1UL << 0))//El primer pulsador esta a 1?
	{
		valor = 1;
	}
	if (XGpio_DiscreteRead(&pulsadoryled, 1) & (1UL << 1))//El segundo pulsador esta a 1?
	{
		valor = 2; //Para acceder a un uncio led ejemplo 0x1 0x2
	}
	if (XGpio_DiscreteRead(&pulsadoryled, 1) & (1UL << 2))//El tercer pulsador esta a 1?
	{
		valor = 3; //Encender todas
	}
	if (XGpio_DiscreteRead(&pulsadoryled, 1) & (1UL << 3))//El cuarto pulsador esta a 1?
	{
		valor = 4; //Apagar todas
	}
	return valor;
}
void luces(XGpio pulsadoryled, int valor){
	switch(valor){
		case 1:
			XGpio_DiscreteWrite(&pulsadoryled, 2, 0x1); //Para acceder a un uncio led ejemplo 0x1 0x2
		break;

		case 2:
			XGpio_DiscreteWrite(&pulsadoryled, 2, 0x2); //Para acceder a un uncio led ejemplo 0x1 0x2
		break;

		case 3:
			XGpio_DiscreteWrite(&pulsadoryled, 2, 0x4); //Para acceder a un uncio led ejemplo 0x1 0x2
		break;

		case 4:
			XGpio_DiscreteWrite(&pulsadoryled, 2, 0x8); //Para acceder a un uncio led ejemplo 0x1 0x2
		break;
	}
}
int FinalCarrera1(XGpio FC){
	return XGpio_DiscreteRead(&FC, 1);
}
int FinalCarrera2(XGpio FC){
	return XGpio_DiscreteRead(&FC, 2);
}
int interruptorfuncion(XGpio interruptor, XGpio cal){
	int valor=XGpio_DiscreteRead(&interruptor, 1);
	if (valor > 0) {
		XGpio_DiscreteWrite(&cal, 1, 1);
	}
	else{
		XGpio_DiscreteWrite(&cal, 1, 0);
	}
	return valor;
}
