//#**************************************************************************#*/
//#     System name :   uart_task.c                                         #*/	
//#     File name   :   uart_task.c                              	         #*/
//#     Summary     :                                                        #*/
//#     Author      :   Tuanhd                                               #*/
//#     Date        :   12/1/2017                                            #*/
//#                                                                          #*/
//#     Copyright (c) 2016 VNPT-Technology                                   #*/
//#                        All rights reserved.                              #*/
//#**************************************************************************#*/
//#  Version                   : Date        : Infomation                    #*/
//#  Major Ver.Minor Ver.Rev   : YYYY/MM/DD  :                               #*/
//#--------------------------------------------------------------------------#*/
//#     1.0.0                  : 12/1/2017  : New making                     #*/
//#**************************************************************************#*/

/******************************************************************************
 * Includes
 ******************************************************************************/
#include "main.h"
#include "function_wqi.h"

/*---------------------------   STRUCT extern    -----------------------------*/
extern board_config 		        wqi_boards_config;
extern wqi_data                     wqi_data_read;
extern wqi_calculate                wqi_calculates;
/*---------------------------   static value    ------------------------------*/
//static struct wqi_data_read wqi_data_read = { 6.5,	4.9,	28,		0.7,	4 };
						   //pH,	 DO,	temp,	NH4,	Muddy

float coeff_BP_NH4_Muddy[3][5] = { 
                                    {/*<=*/0.1, 0.2,  0.5,   1,/*>=*/ 5},  
                                    {      100,  75,   50,  25,       1}, 
                                    {	     5,	 20,   30,	70,	    100} 
                                 };	


float coeff_DO[2][8] =  {
                            {20, 50, 75,  88,  112, 125,   150, 200},  //BP_DO
                            {25, 50, 75, 100,  100,  75,	50,	 25}    //q_DO
                        };
                     
float coeff_pH[2][4] =  {   
                            {5.5,    6,   8.5,	 9},	//BP_pH
                            { 50,  100,   100,	50} //q_pH
                        };			    

static uint32_t j,i;
float BP_NH4_Up, BP_NH4_Down;
float BP_Muddy_Up, BP_Muddy_Down;
float q_NH4_Muddy_Up, q_NH4_Muddy_Down;
float DO_full;
float q_DO_Up, q_DO_Down;
float BP_DO_Up, BP_DO_Down;
float q_pH_Up, q_pH_Down;
float BP_pH_Up, BP_pH_Down;

/*Init*-------------------------------------------------------------------------
* Task Name : wqi_DO_calculate_hours
* Comments  : 
*           
*END*-------------------------------------------------------------------------*/
void wqi_DO_calculate_hours(float data_value)
{
    //float WQI_OF_DO = 0;
    // 2.3 Calculate WQI DO
	// DO_full means saturated
	DO_full = 14.652 - 0.41022*wqi_data_read.TEMP + 0.007991*(wqi_data_read.TEMP*wqi_data_read.TEMP) - 0.000077774*(wqi_data_read.TEMP*wqi_data_read.TEMP*wqi_data_read.TEMP);
	// DO % bao hoa
	DO_full = wqi_data_read.DO / DO_full * 100;

	for (i = 0; i < 8; i++)
	{	
		if ((DO_full <= coeff_DO[0][0]) || (DO_full >= coeff_DO[0][7]))			//20 || 200
		{
			wqi_calculates.WQI_DO = 1;
		}
		else if ((DO_full >= coeff_DO[0][3]) && (DO_full <= coeff_DO[0][4]))	//88 - 112
		{
			wqi_calculates.WQI_DO = 100;
		}
		else if ((DO_full == coeff_DO[0][1]) || (DO_full == coeff_DO[0][6]))	//50 || 150
		{	
			wqi_calculates.WQI_DO = 50;
		}
		else if ((DO_full == coeff_DO[0][2]) || (DO_full == coeff_DO[0][5]))	//75 || 125
		{
			wqi_calculates.WQI_DO = 75;
		}
		else if ((DO_full > coeff_DO[0][i]) && (DO_full < coeff_DO[0][i + 1]) && (DO_full < coeff_DO[0][3]))	//<88
		{
			q_DO_Up = coeff_DO[1][i + 1];
			q_DO_Down = coeff_DO[1][i];
			BP_DO_Up = coeff_DO[0][i + 1];
			BP_DO_Down = coeff_DO[0][i];
            
			wqi_calculates.WQI_DO = (q_DO_Up - q_DO_Down) / (BP_DO_Up - BP_DO_Down)*(DO_full - BP_DO_Down) + q_DO_Down;		//Cong thuc 02
		}
		else if ((DO_full > coeff_DO[0][i]) && (DO_full < coeff_DO[0][i + 1]) && (DO_full > coeff_DO[0][4]))	//<88
		{
			q_DO_Up = coeff_DO[1][i + 1];
			q_DO_Down = coeff_DO[1][i];
			BP_DO_Up = coeff_DO[0][i + 1];
			BP_DO_Down = coeff_DO[0][i];
            
			wqi_calculates.WQI_DO = (q_DO_Down - q_DO_Up) / (BP_DO_Up - BP_DO_Down)*(BP_DO_Up - DO_full) + q_DO_Up;		//Cong thuc 01
		}
	}

   //return WQI_OF_DO;
}
/*Init*-------------------------------------------------------------------------
* Task Name : wqi_PH_calculate_hours
* Comments  : 
*           
*END*-------------------------------------------------------------------------*/
void wqi_PH_calculate_hours(float data_value)
{
    // 2.4 Calculate WQI pH
	if ((wqi_data_read.PH <= coeff_pH[0][0]) || (wqi_data_read.PH >= coeff_pH[0][3]))			//5.5 || 9
	{	
		wqi_calculates.WQI_PH = 1;
	}
	else if ((wqi_data_read.PH >= coeff_pH[0][1]) && (wqi_data_read.PH <= coeff_pH[0][2]))	//6 || 8.5
	{	
		wqi_calculates.WQI_PH = 100;
	}
	else if ((wqi_data_read.PH > coeff_pH[0][0]) && (wqi_data_read.PH < coeff_pH[0][1]))
	{
		BP_pH_Up = coeff_pH[0][1];
		BP_pH_Down = coeff_pH[0][0];
		q_pH_Up = coeff_pH[1][1];
		q_pH_Down = coeff_pH[1][0];
        
		wqi_calculates.WQI_PH = (q_pH_Up - q_pH_Down) / (BP_pH_Up - BP_pH_Down)*(wqi_data_read.PH - BP_pH_Down) + q_pH_Down;		//Cong thuc 02
	}
	else if ((wqi_data_read.PH > coeff_pH[0][2]) && (wqi_data_read.PH < coeff_pH[0][3]))
	{
		BP_pH_Up = coeff_pH[0][3];
		BP_pH_Down = coeff_pH[0][2];
		q_pH_Up = coeff_pH[1][3];
		q_pH_Down = coeff_pH[1][2];
        
		wqi_calculates.WQI_PH = (q_pH_Down - q_pH_Up) / (BP_pH_Up - BP_pH_Down)*(BP_pH_Up - wqi_data_read.PH) + q_pH_Up;			//Cong thuc 01
	}  
}
/*Init*-------------------------------------------------------------------------
* Task Name : wqi_NH4_calculate_hours
* Comments  : 
*           
*END*-------------------------------------------------------------------------*/
void wqi_NH4_calculate_hours(float data_value)
{
    // 2. Calculate WQI of parameters
	// 2.1 Calculate WQI NH4

	for (j = 0; j < 5; j++)
	{
		if (coeff_BP_NH4_Muddy[0][j] == wqi_data_read.NH4)
		{
			wqi_calculates.WQI_NH4 = coeff_BP_NH4_Muddy[1][j];
		}
		else if (coeff_BP_NH4_Muddy[0][0] > wqi_data_read.NH4)
		{
			wqi_calculates.WQI_NH4 = coeff_BP_NH4_Muddy[1][0];
		}
		else if (coeff_BP_NH4_Muddy[0][4] < wqi_data_read.NH4)
		{
			wqi_calculates.WQI_NH4 = coeff_BP_NH4_Muddy[1][4];
		}
		else if ((coeff_BP_NH4_Muddy[0][j] < wqi_data_read.NH4) && (coeff_BP_NH4_Muddy[0][j + 1] > wqi_data_read.NH4))
		{
			q_NH4_Muddy_Up = coeff_BP_NH4_Muddy[1][j + 1];
			q_NH4_Muddy_Down = coeff_BP_NH4_Muddy[1][j];
			BP_NH4_Up = coeff_BP_NH4_Muddy[0][j + 1];
			BP_NH4_Down = coeff_BP_NH4_Muddy[0][j];
            
			wqi_calculates.WQI_NH4 = ((q_NH4_Muddy_Down - q_NH4_Muddy_Up) / (BP_NH4_Up - BP_NH4_Down)*(BP_NH4_Up - wqi_data_read.NH4)) + q_NH4_Muddy_Up;
		}
	}  
}
/*Init*-------------------------------------------------------------------------
* Task Name : wqi_Turbi_calculate_hours
* Comments  : 
*           
*END*-------------------------------------------------------------------------*/
void wqi_Turbi_calculate_hours(float data_value)
{
    // 2.2 Calculate WQI Turbidity
	for (j = 0; j < 5; j++)
	{
		if (coeff_BP_NH4_Muddy[2][j] == wqi_data_read.TURBIDITY)
		{
			wqi_calculates.WQI_TURBIDITY = coeff_BP_NH4_Muddy[1][j];
		}
		else if (coeff_BP_NH4_Muddy[2][0] > wqi_data_read.TURBIDITY)
		{
			wqi_calculates.WQI_TURBIDITY = coeff_BP_NH4_Muddy[1][0];
		}
		else if (coeff_BP_NH4_Muddy[2][4] < wqi_data_read.TURBIDITY)
		{
			wqi_calculates.WQI_TURBIDITY = coeff_BP_NH4_Muddy[1][4];
		}
		else if ((coeff_BP_NH4_Muddy[2][j] < wqi_data_read.TURBIDITY) && (coeff_BP_NH4_Muddy[2][j + 1] > wqi_data_read.TURBIDITY))
		{
			q_NH4_Muddy_Up = coeff_BP_NH4_Muddy[1][j + 1];
			q_NH4_Muddy_Down = coeff_BP_NH4_Muddy[1][j];
			BP_Muddy_Up = coeff_BP_NH4_Muddy[2][j + 1];
			BP_Muddy_Down = coeff_BP_NH4_Muddy[2][j];
            
			wqi_calculates.WQI_TURBIDITY = (q_NH4_Muddy_Down - q_NH4_Muddy_Up) / (BP_Muddy_Up - BP_Muddy_Down)*(BP_Muddy_Up - wqi_data_read.TURBIDITY) + q_NH4_Muddy_Up;
		}
	}  
}












