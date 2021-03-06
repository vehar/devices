/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               TouchPanel.c
** Descriptions:            The TouchPanel application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2010-11-7
** Version:                 v1.0
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             
** Modified date:           
** Version:                 
** Descriptions:            
**
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "TouchPanel.h"
#include <stdio.h>

SPI_HandleTypeDef  hspi1;

/* Private variables ---------------------------------------------------------*/
Matrix matrix ;
Coordinate  display ;
uint8_t PMode2;
Example example;
My_color my_color;

void TP_CS(uint8_t i){if(i == 1)
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
					else		
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
}
uint8_t TP_INT_IN(){ if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_6)==GPIO_PIN_SET)return 1; else return 0;}

/* DisplaySample LCD坐标上对应的ads7843采样AD值 如：LCD 坐标45,45 应该的X Y采样ADC分别为3388,920 */	
Coordinate ScreenSample[3];
/* LCD上的坐标 */
Coordinate DisplaySample[3] = {{45,45},{45,270},{190,190}};	 // 

//Coordinate DisplaySample[3] = {{20,20},{20,300},{220,160}};

/* Private define ------------------------------------------------------------*/
#define THRESHOLD 2   /* 差值门限 */


void HexToBCD(uint16_t Num);

 //--------------------------------------
// Convet HEX 2 byte to 7-Segment code
//--------------------------------------
void HexToBCD(uint16_t Num)
{
uint16_t temp = Num;
uint16_t temp2;
temp2= temp%10; 
example.S0=(unsigned char)temp2; // 襦爨� 祀噤� 鲨麴� 矬耱� 狍溴�5
temp =  temp/10; temp2=temp;// 镱塍鬣屐 镥疴 4 麒耠�
example.S1=(unsigned char)temp2%10; // 镱塍鬣屐 麇蜮屦蝮� 鲨麴�
temp =  temp/10; temp2=temp;	 // 镥疴 3 麒耠�
example.S2=(unsigned char)temp%10; // 镱塍鬣屐 蝠弪 鲨麴�
temp =  temp/10; temp2=temp;	 // 镥疴 2 麒耠�
example.S3=(unsigned char)temp2%10; // 镱塍鬣屐 怛铕簋 鲨麴�
temp = (uint16_t)temp/10;	 // 镥疴 1 麒耠�
example.S4=(unsigned char)temp; // 镱塍鬣屐 镥疴簋 襦祗� 耱囵� 鲨麴�
}	





/*******************************************************************************
* Function Name  : ADS7843_SPI_Init
* Description    : ADS7843 SPI 初始化
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void ADS7843_SPI_Init(void) 
{ 
  
 GPIO_InitTypeDef GPIO_InitStruct;

  /* USER CODE BEGIN SPI1_MspInit 0 */

  /* USER CODE END SPI1_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_SPI1_CLK_ENABLE();
  


  /* USER CODE BEGIN SPI1_MspInit 1 */

  /* USER CODE END SPI1_MspInit 1 */
  
	
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  HAL_SPI_Init(&hspi1);
	
} 

/*******************************************************************************
* Function Name  : TP_Init
* Description    : ADS7843端口初始化
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void TP_Init(void) 
{ 
  GPIO_InitTypeDef GPIO_InitStruct;
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  /* TP_CS */

	  /*Configure GPIO pin : PA4 */
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
  /* TP_IRQ */

	  /*Configure GPIO pin : PB6 */
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	    /**SPI1 GPIO Configuration    
    PA5     ------> SPI1_SCK
    PA6     ------> SPI1_MISO
    PA7     ------> SPI1_MOSI 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  TP_CS(1); 
	
  ADS7843_SPI_Init(); 
} 

/*******************************************************************************
* Function Name  : DelayUS
* Description    : 延时1us
* Input          : - cnt: 延时值
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void DelayUS(uint32_t cnt)
{
  uint16_t i;
  for(i = 0;i<cnt;i++)
  {
     uint8_t us = 6; /* 设置值为12，大约延1微秒 */    
     while (us--)     /* 延1微秒	*/
     {
       ;   
     }
  }
}


/*******************************************************************************
* Function Name  : WR_CMD
* Description    : 向 ADS7843写数据
* Input          : - cmd: 传输的数据
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void WR_CMD (uint8_t cmd)  
{ 
  /* Wait for SPI1 Tx buffer empty */ 
 // while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET); 
  /* Send SPI1 data */ 
 // SPI_I2S_SendData(SPI1,cmd); 
  /* Wait for SPI1 data reception */ 
 // while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET); 
  /* Read SPI1 received data */ 
 // SPI_I2S_ReceiveData(SPI1); 
	
	HAL_SPI_Transmit(&hspi1,&cmd,1,100);
} 

/*******************************************************************************
* Function Name  : RD_AD
* Description    : 读取ADC值
* Input          : None
* Output         : None
* Return         : ADS7843返回二字节数据
* Attention		 : None
*******************************************************************************/
static int RD_AD(void)  
{ 
  unsigned short buf,temp; 
  /* Wait for SPI1 Tx buffer empty */ 
 // while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET); 
  /* Send SPI1 data */ 
 // SPI_I2S_SendData(SPI1,0x0000); 
	uint16_t tmp = 0x0000;
	//HAL_SPI_Transmit(&hspi1,(uint8_t*)&tmp,1,100);
  /* Wait for SPI1 data reception */ 
  //while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET); 
  /* Read SPI1 received data */ 
  //temp=SPI_I2S_ReceiveData(SPI1); 
	HAL_SPI_Receive(&hspi1,(uint8_t*)&temp,1,100);
  buf=temp<<8; 
  //DelayUS(1); 
  //while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET); 
  /* Send SPI1 data */ 
  //SPI_I2S_SendData(SPI1,0x0000); 
	//HAL_SPI_Transmit(&hspi1,(uint8_t*)&tmp,1,100);
  /* Wait for SPI1 data reception */ 
 // while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET); 
  /* Read SPI1 received data */ 
 // temp=SPI_I2S_ReceiveData(SPI1); 
 HAL_SPI_Receive(&hspi1,(uint8_t*)&temp,1,100);
  buf |= temp; 
  buf>>=3; 
  buf&=0xfff; 
  return buf; 
} 

/*******************************************************************************
* Function Name  : Read_X
* Description    : 读取ADS7843通道X+的ADC值 
* Input          : None
* Output         : None
* Return         : ADS7843返回通道X+的ADC值
* Attention		 : None
*******************************************************************************/
int Read_X(void)  
{  
  int i; 
  TP_CS(0); 
  DelayUS(1); 
  WR_CMD(CHX); 
 // DelayUS(1); 
  i=RD_AD(); 
  TP_CS(1); 
  return i;    
} 

/*******************************************************************************
* Function Name  : Read_Y
* Description    : 读取ADS7843通道Y+的ADC值
* Input          : None
* Output         : None
* Return         : ADS7843返回通道Y+的ADC值
* Attention		 : None
*******************************************************************************/
int Read_Y(void)  
{  
  int i; 
  TP_CS(0); 
  DelayUS(1); 
  WR_CMD(CHY); 
  //DelayUS(1); 
  i=RD_AD(); 
  TP_CS(1); 
  return i;     
} 

/*******************************************************************************
* Function Name  : TP_GetAdXY
* Description    : 读取ADS7843 通道X+ 通道Y+的ADC值
* Input          : None
* Output         : None
* Return         : ADS7843返回 通道X+ 通道Y+的ADC值 
* Attention		 : None
*******************************************************************************/
void TP_GetAdXY(int *x,int *y)  
{ 
  int adx,ady; 
  adx=Read_X(); 
  ady=Read_Y(); 
  *x=adx; 
  *y=ady; 
} 

/*******************************************************************************
* Function Name  : TP_DrawPoint
* Description    : 在指定座标画点
* Input          : - Xpos: Row Coordinate
*                  - Ypos: Line Coordinate 
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void TP_DrawPoint(uint16_t Xpos,uint16_t Ypos)
{
 /* LCD_SetPoint(Xpos,Ypos,my_color);    
  LCD_SetPoint(Xpos+1,Ypos,my_color);
  LCD_SetPoint(Xpos,Ypos+1,my_color);
  LCD_SetPoint(Xpos+1,Ypos+1,my_color);	*/
}	

/*******************************************************************************
* Function Name  : DrawCross
* Description    : 在指定座标画十字准星
* Input          : - Xpos: Row Coordinate
*                  - Ypos: Line Coordinate 
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void DrawCross(uint16_t Xpos,uint16_t Ypos)
{
 /* LCD_DrawLine(Xpos-15,Ypos,Xpos-2,Ypos,0xffff);
  LCD_DrawLine(Xpos+2,Ypos,Xpos+15,Ypos,0xffff);
  LCD_DrawLine(Xpos,Ypos-15,Xpos,Ypos-2,0xffff);
  LCD_DrawLine(Xpos,Ypos+2,Xpos,Ypos+15,0xffff);
  
  LCD_DrawLine(Xpos-15,Ypos+15,Xpos-7,Ypos+15,RGB565CONVERT(184,158,131));
  LCD_DrawLine(Xpos-15,Ypos+7,Xpos-15,Ypos+15,RGB565CONVERT(184,158,131));

  LCD_DrawLine(Xpos-15,Ypos-15,Xpos-7,Ypos-15,RGB565CONVERT(184,158,131));
  LCD_DrawLine(Xpos-15,Ypos-7,Xpos-15,Ypos-15,RGB565CONVERT(184,158,131));

  LCD_DrawLine(Xpos+7,Ypos+15,Xpos+15,Ypos+15,RGB565CONVERT(184,158,131));
  LCD_DrawLine(Xpos+15,Ypos+7,Xpos+15,Ypos+15,RGB565CONVERT(184,158,131));

  LCD_DrawLine(Xpos+7,Ypos-15,Xpos+15,Ypos-15,RGB565CONVERT(184,158,131));
  LCD_DrawLine(Xpos+15,Ypos-15,Xpos+15,Ypos-7,RGB565CONVERT(184,158,131));	  	*/
}	
	
/*******************************************************************************
* Function Name  : Read_Ads7846
* Description    : 得到滤波之后的X Y
* Input          : None
* Output         : None
* Return         : Coordinate结构体地址
* Attention		 : None
*******************************************************************************/
Coordinate *Read_Ads7846(void)
{
  static Coordinate  screen;
  int m0,m1,m2,TP_X[1],TP_Y[1],temp[3];
  uint8_t count=0;
  int buffer[2][9]={{0},{0}};  /* 坐标X和Y进行多次采样 */
  //ADS7843_SPI_Init(); 		
  do					       /* 循环采样9次 */
  {		   
    TP_GetAdXY(TP_X,TP_Y);  
	buffer[0][count]=TP_X[0];  
	buffer[1][count]=TP_Y[0];
	count++;  
  }
  while((TP_INT_IN()==0)&& count<9);  /* TP_INT_IN为触摸屏中断引脚,当用户点击触摸屏时TP_INT_IN会被置低 */
  if(count>=9)   /* 成功采样9次,进行滤波 */ 
  {

    /* 为减少运算量,分别分3组取平均值 */
    temp[0]=(buffer[0][0]+buffer[0][1]+buffer[0][2])/3;
	temp[1]=(buffer[0][3]+buffer[0][4]+buffer[0][5])/3;
	temp[2]=(buffer[0][6]+buffer[0][7]+buffer[0][8])/3;
	/* 计算3组数据的差值 */
	m0=temp[0]-temp[1];
	m1=temp[1]-temp[2];
	m2=temp[2]-temp[0];
	/* 对上述差值取绝对值 */
	m0=m0>0?m0:(-m0);
    m1=m1>0?m1:(-m1);
	m2=m2>0?m2:(-m2);
	/* 判断绝对差值是否都超过差值门限，如果这3个绝对差值都超过门限值，则判定这次采样点为野点,抛弃采样点，差值门限取为2 */
	if( m0>THRESHOLD  &&  m1>THRESHOLD  &&  m2>THRESHOLD ) return 0;
	/* 计算它们的平均值，同时赋值给screen */ 
	if(m0<m1)
	{
	  if(m2<m0) 
	    screen.x=(temp[0]+temp[2])/2;
	  else 
	    screen.x=(temp[0]+temp[1])/2;	
	}
	else if(m2<m1) 
	  screen.x=(temp[0]+temp[2])/2;
	else 
	  screen.x=(temp[1]+temp[2])/2;

	/* 同上 计算Y的平均值 */
    temp[0]=(buffer[1][0]+buffer[1][1]+buffer[1][2])/3;
	temp[1]=(buffer[1][3]+buffer[1][4]+buffer[1][5])/3;
	temp[2]=(buffer[1][6]+buffer[1][7]+buffer[1][8])/3;
	m0=temp[0]-temp[1];
	m1=temp[1]-temp[2];
	m2=temp[2]-temp[0];
	m0=m0>0?m0:(-m0);
	m1=m1>0?m1:(-m1);
	m2=m2>0?m2:(-m2);
	if(m0>THRESHOLD&&m1>THRESHOLD&&m2>THRESHOLD) return 0;

	if(m0<m1)
	{
	  if(m2<m0) 
	    screen.y=(temp[0]+temp[2])/2;
	  else 
	    screen.y=(temp[0]+temp[1])/2;	
    }
	else if(m2<m1) 
	   screen.y=(temp[0]+temp[2])/2;
	else
	   screen.y=(temp[1]+temp[2])/2;

	return &screen;
  }  
  return 0; 
}
	 
/* 下面是触摸屏到液晶屏坐标变换的转换函数 */
/* 只有在LCD和触摸屏间的误差角度非常小时,才能运用下面公式 */


/*******************************************************************************
* Function Name  : setCalibrationMatrix
* Description    : 计算出 K A B C D E F
* Input          : None
* Output         : None
* Return         : 返回1表示成功 0失败
* Attention		 : None
*******************************************************************************/
FunctionalState setCalibrationMatrix( Coordinate * displayPtr,
                          Coordinate * screenPtr,
                          Matrix * matrixPtr)
{

  FunctionalState retTHRESHOLD = ENABLE ;
  /* K＝(X0－X2) (Y1－Y2)－(X1－X2) (Y0－Y2) */
  matrixPtr->Divider = ((screenPtr[0].x - screenPtr[2].x) * (screenPtr[1].y - screenPtr[2].y)) - 
                       ((screenPtr[1].x - screenPtr[2].x) * (screenPtr[0].y - screenPtr[2].y)) ;
  if( matrixPtr->Divider == 0 )
  {
    retTHRESHOLD = DISABLE;
  }
  else
  {
    /* A＝((XD0－XD2) (Y1－Y2)－(XD1－XD2) (Y0－Y2))／K	*/
    matrixPtr->An = ((displayPtr[0].x - displayPtr[2].x) * (screenPtr[1].y - screenPtr[2].y)) - 
                    ((displayPtr[1].x - displayPtr[2].x) * (screenPtr[0].y - screenPtr[2].y)) ;
	/* B＝((X0－X2) (XD1－XD2)－(XD0－XD2) (X1－X2))／K	*/
    matrixPtr->Bn = ((screenPtr[0].x - screenPtr[2].x) * (displayPtr[1].x - displayPtr[2].x)) - 
                    ((displayPtr[0].x - displayPtr[2].x) * (screenPtr[1].x - screenPtr[2].x)) ;
    /* C＝(Y0(X2XD1－X1XD2)+Y1(X0XD2－X2XD0)+Y2(X1XD0－X0XD1))／K */
    matrixPtr->Cn = (screenPtr[2].x * displayPtr[1].x - screenPtr[1].x * displayPtr[2].x) * screenPtr[0].y +
                    (screenPtr[0].x * displayPtr[2].x - screenPtr[2].x * displayPtr[0].x) * screenPtr[1].y +
                    (screenPtr[1].x * displayPtr[0].x - screenPtr[0].x * displayPtr[1].x) * screenPtr[2].y ;
    /* D＝((YD0－YD2) (Y1－Y2)－(YD1－YD2) (Y0－Y2))／K	*/
    matrixPtr->Dn = ((displayPtr[0].y - displayPtr[2].y) * (screenPtr[1].y - screenPtr[2].y)) - 
                    ((displayPtr[1].y - displayPtr[2].y) * (screenPtr[0].y - screenPtr[2].y)) ;
    /* E＝((X0－X2) (YD1－YD2)－(YD0－YD2) (X1－X2))／K	*/
    matrixPtr->En = ((screenPtr[0].x - screenPtr[2].x) * (displayPtr[1].y - displayPtr[2].y)) - 
                    ((displayPtr[0].y - displayPtr[2].y) * (screenPtr[1].x - screenPtr[2].x)) ;
    /* F＝(Y0(X2YD1－X1YD2)+Y1(X0YD2－X2YD0)+Y2(X1YD0－X0YD1))／K */
    matrixPtr->Fn = (screenPtr[2].x * displayPtr[1].y - screenPtr[1].x * displayPtr[2].y) * screenPtr[0].y +
                    (screenPtr[0].x * displayPtr[2].y - screenPtr[2].x * displayPtr[0].y) * screenPtr[1].y +
                    (screenPtr[1].x * displayPtr[0].y - screenPtr[0].x * displayPtr[1].y) * screenPtr[2].y ;
  }
  return( retTHRESHOLD ) ;
}

/*******************************************************************************
* Function Name  : getDisplayPoint
* Description    : 通过 K A B C D E F 把通道X Y的值转换为液晶屏坐标
* Input          : None
* Output         : None
* Return         : 返回1表示成功 0失败
* Attention		 : None
*******************************************************************************/
FunctionalState getDisplayPoint(Coordinate * displayPtr,
                     Coordinate * screenPtr,
                     Matrix * matrixPtr )
{
  FunctionalState retTHRESHOLD =ENABLE ;

  if( matrixPtr->Divider != 0 )
  {
    /* XD = AX+BY+C */        
    displayPtr->x = ( (matrixPtr->An * screenPtr->x) + 
                      (matrixPtr->Bn * screenPtr->y) + 
                       matrixPtr->Cn 
                    ) / matrixPtr->Divider ;
	/* YD = DX+EY+F */        
    displayPtr->y = ( (matrixPtr->Dn * screenPtr->x) + 
                      (matrixPtr->En * screenPtr->y) + 
                       matrixPtr->Fn 
                    ) / matrixPtr->Divider ;
  }
  else
  {
    retTHRESHOLD = DISABLE;
  }
  return(retTHRESHOLD);
} 

/*******************************************************************************
* Function Name  : TouchPanel_Calibrate
* Description    : 校准触摸屏
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void TouchPanel_Calibrate(uint8_t CalSiNo)
{
  uint8_t i;
  Coordinate * Ptr;

	GetFormat();

  for(i=0;i<3;i++)
  {
  // LCD_Clear(Black);
  if(!CalSiNo){	// 赅腓狃钼赅 镳铒桉囗� 玎疣眄邋
//   	ScreenSample[0].x = 400; ScreenSample[1].x = 3535; ScreenSample[2].x = 1957;
//	ScreenSample[0].y = 560; ScreenSample[1].y = 604; ScreenSample[2].y	=3528;

//	潆� 祛邈� 滂耧脲� 篑蜞眍忤螯 耠邃簋� 镟疣戾蝠� 赅腓狃钼觇 疣磬 镱-箪铍鬣龛�
 	ScreenSample[0].x = 0xD10; ScreenSample[1].x = 0x355; ScreenSample[2].x = 0x06D2;
	ScreenSample[0].y = 0x0369; ScreenSample[1].y = 0x35B; ScreenSample[2].y	= 0x0C36;


   }
   else {  // 赅腓狃钼牦 溴豚屐 玎眍泐

   //GUI_Text(10,10,"Touch crosshair to calibrate",0xffff,Black);
   HAL_Delay(500);
   DrawCross(DisplaySample[i].x,DisplaySample[i].y);
   do
   {
   Ptr=Read_Ads7846();
   }
   while( Ptr == (void*)0 );
   ScreenSample[i].x= Ptr->x; ScreenSample[i].y= Ptr->y;

  }	}
  setCalibrationMatrix( &DisplaySample[0],&ScreenSample[0],&matrix ) ;  /* 送入值得到参数 */	   
  //LCD_Clear(my_color);
} 

//***********************************************
uint8_t LCD_TouchRead(Coordinate * displayLCD){
	uint16_t px,py,x,y;
	int p,f;


	GetFormat();							 /*obtiene valor de formato de pantalla activo*/
	p=0;
	f=0;

	while(f<30){
		getDisplayPoint(&display, Read_Ads7846(), &matrix ) ;	 /*lee la posici髇 del puntero sobre la pantalla*/
		px=display.x;
		py=display.y;
		if((PMode2==0)&&(py<400)){x=px,y=py;p=1;break;}
		if((PMode2==1)&&(px<400)){x=px,y=py;p=1;break;}
		f++;
	}

	if(f>=30){p=0;x=400;y=400;}
	displayLCD->x = x;
  displayLCD->y =	y;

  return(p);
	}
//*********************************
void GetFormat(void)				  /*obtiene valor PMode2 de Formato de pantalla*/
	{
	PMode2=LCD_FormatRead();
	}
//*********************************
uint8_t LCD_FormatRead(void){
	uint8_t value;
	value=1;  /*Pmode=1 Landscape vertical, Pmode=0 Landscape Apaisada*/	//PMode;
	return(value);
}//********************************




/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
