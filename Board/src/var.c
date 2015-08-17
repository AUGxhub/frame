#include "var.h"
#include "MK60_port.h"
#include "MK60_gpio.h"
#include  "MK60_PIT.h" 
uint8 blockF =0;//扫到横线的标志
uint8 turn90F = 0;//判断为直角转弯
uint8 turnF =0;//判断左右转 1 为左转 2 为右转
uint8 blockF_off =0;//横线检测屏蔽标志
uint8  leftEdgeF=0;//
uint8  rightEdgeF=0;//左右边线点的数量
uint8 flag2 =0;
uint8 zhiFlag=0;
uint16 count =0;
extern uint8 Flag_Turn ;
//逐行滤波 只有一行超过规定的点数后 才能采集到 否则无效
void erasePot(uint8 *data,uint16 width,uint16 hight)
{
	int h, w, less,temp;
	less =7;
	
	//一行超过 less 个点视为该行有效
	for(h=0;h<height;h++)
	{
		temp=0;
		//统计一行黑点的个数
		for(w=0;w<weight;w++)
		{
			if(*(data+w+h*weight)==LED_IMAGE_WHITE)temp++;
		}
		if(temp<less) 
		{
			for(w=0;w<weight;w++)
			{
				*(data+w+h*weight) = LED_IMAGE_BLACK;	
			}
		}	 
		*(data+0+h*weight)=0;
		*(data+79+h*weight)=0;
	}			
}


//计算出两个跑道边缘 并计算出中心线 中心参考线
uint8 FindEdge(uint8 *data,uint16 width,uint16 hight,  int * position)	
{
	int h, w, LeftIndex_1,RightIndex_1,LeftIndex_2,RightIndex_2;
	int lineFlagRight[height],lineFlagLeft[height];//左右线采集到的标志  0为没有采集到边线  1 为采集到边线
	int line[height][4];//储存边线的位置
	uint16 ati[height] ;//当前位置
	int mark1,mark2,markL[height];
	//////////////////////////////判断直角弯 
	/**/
	int zuodiuxian[2];//丢线检测
	int youdiuxian[2];//
	uint16 black;
	uint8 chuizhiheixian[height];


	//直角弯检测
	if (blockF = 1 )
	{
		black =0;
		for(h=11;h<14;h++)
		{
			for(w=0;w<weight;w++)
			{

				
				if(*(data  + w +h*weight) == 0) black ++;
				if(black > (3*80) )black =0;
			}
		}
		if(black >(3 *55))
		{
			
			
			//printf("head is out\n\r");
			//左右丢线检测
			zuodiuxian[0] =0;
			zuodiuxian[1] =0;
			youdiuxian[0] =0;
			youdiuxian[1] =0;
			for(h=0;h<height;h++)
			{
				if(Flag_Turn ==3)
				{
					if( *(data  + 78 +h*weight) == 255) youdiuxian[0]++;	
					if( *(data  + 0 +h*weight) == 255)  zuodiuxian[0]++;
				}
				else if (Flag_Turn ==1)
				{
					if( *(data  + 0 +h*weight) == 255)  zuodiuxian[0]++;
					
					
				}
				else if (Flag_Turn ==2)
				{
					if( *(data  + 78 +h*weight) == 255) youdiuxian[0]++;	
					
				}
			}
			if(zuodiuxian[0] > 15) zuodiuxian[1] =1;
			if(youdiuxian[0] > 5) youdiuxian[1] =1;
			
			if((youdiuxian[1] ==1)||(zuodiuxian[1] ==1)) //
			{		
				//printf("zhijiao\n\r");
				if(zuodiuxian[0] > youdiuxian[0])
				{
					turnF =1;
				}
				else if(zuodiuxian[0] > youdiuxian[0])
				{
					turnF = 2;
				}
				else 
				{
					turnF =2;
				}			
			}
			
		}
	}



	//从中间向两边进行扫描跑道边缘线
	//强制补线 
	for(h=0;h<height;h++)
	{		
		for(w=0;w<weight;w++)
		{
			if(w == 0 )*(data+w+h*weight)=0;//设置左边线
			if(w == 79)*(data+w+h*weight)=0;//设置左边线
		}	
	}
	
	for(h=0;h<height;h++)
	{
		
		for(w=39;w>0;w--)
		{
			//左边
			if((*(data+w+h*weight)==255)&&(*(data+w-1+h*weight)==0)){LeftIndex_1 =w; lineFlagLeft[h]=1;}	//黑线左边缘
			else lineFlagLeft[h]=0;
			//if((*(data+w+h*weight)==255)&&(*(data+w+1+h*weight)==0))	RightIndex_1 =w;//黑线右边缘
			line[h][0]=LeftIndex_1;

			//line[h][1]=RightIndex_1;
			
		}
		for(w=40;w<weight;w++)
		{
			if((*(data+w+h*weight)==0)&&(*(data+w-1+h*weight)==255))	{LeftIndex_2 =w; lineFlagRight[h]=1;}//黑线左边缘
			else lineFlagRight[h] =0;
			//if((*(data+w+h*weight)==0)&&(*(data+w+1+h*weight)==255))	RightIndex_2 =w;//黑线右边缘
			line[h][2]=LeftIndex_2;
			
			
			//line[h][3]=RightIndex_2;
		}
	}

	if(blockF_off ==0)
	{
		//横线标志检测    
		//横线 直角弯判断 区分
		if(blockF >=2) blockF =0;	
		for(h=30;h<50;h++)
		{
			mark1 =0;
			for(w = line[h][0];w<39;w++)
			{
				if(  *(data  + w +h*weight) == 255 ) mark1 ++;
			}
			for(w=line[h][2];w>40;w--)
			{
				if( *(data  + w +h*weight) == 255) mark2 ++;
			}
			//printf("mark is %d \n\r",mark);
			if((mark1 < (Kmark)/2) && (mark2 < (Kmark)/2)) markL[h] = 1;
			else markL[h] =0;
		}
		for(h=30;h<50;h++)
		{ 
			if(blockF ==0)
			{
				if((markL[h]==1)&&(markL[h+1]==1)&&(markL[h+2]==1))
				{
					blockF ++;
					if(blockF ==1)
					{
						
						*position=1;
						//printf("hengxian;\n\r");
						gpio_init (PTD7,GPO, 1);	
					}
					else if(blockF ==2)
					{
						*position=0;
						//blockF=0;
						//blockF_off =1;//扫一次标志
						gpio_init (PTD7,GPO, 0);	
					}
					break;
				}	}
			if(blockF ==1)
			{
				if((markL[h]==1)&&(markL[h+1]==1)&&(markL[h+2]==1))
				{
					blockF ++;
					if(blockF ==1)
					{
						
						*position=1;
						//printf("hengxian;\n\r");
						gpio_init (PTD7,GPO, 1);	
					}
					else if(blockF ==2)
					{
						*position=0;
						//blockF=0;
						//blockF_off =1;//扫一次标志
						gpio_init (PTD7,GPO, 0);	
					}
					break;
				}	}
		}		
		
	}
	//显示中心线 和中心参考线做比较 输出给其他控制函数
	for(h=0;h<height;h++)
	{
		ati[h]=(line[h][0]+line[h][2])/2;
		for(w=0;w<weight;w++)
		{			
			if((height>30)&&(w==ati[h]))*(data+w+h*weight)=LED_IMAGE_WHITE;//显示合成后的路径
		}	
	}
	
	
	//判断前方路况
	if(turnF ==1 )//左转弯
	{
		turnF = 0;
		return 5;
	}
	else if (turnF ==2)//右转弯
	{
		turnF = 0;
		return 75;
	}
	else 
	{
		return (ati[height-15]+ati[height-7]+ati[height-8]+ati[height-9]+ati[height-10]+ati[height-11])/6;
	}

}
//求根公式 内部使用 

unsigned int m_sqrt(unsigned int x)
{
	uint8 ans=0,p=0x80;
	while(p!=0)
	{
		ans+=p;
		if(ans*ans>x)
		{
			ans-=p;
		}
		p=(uint8)(p/2);
	}
	return(ans);
}


//计算出两个跑道边缘 并计算出中心线 中心参考线
float FindEdge2(uint8 *data,uint8 width,uint8 hight)	
{
	uint8 w, h;//temp;//临时变量 做循环计数
	int i,j;//有正负的临时变量
	uint8 lineLeft[height],lineRight[height],line[height][weight],lineP[height];//存贮路径
	uint8 *tmpdata = (uint8*) malloc( hight * width * sizeof(uint8));//开辟一个临时空间存储各个过程的处理数据
	memcpy((char*)tmpdata,(char*)data,  hight * width * sizeof(uint8));
	//1。全部扫一边 上升跳变和下降跳变都记录 如果有记录为白点（oled 上的亮点） 存为一张新图
	for(h=0;h<height;h++)
	{
		for (w=0;w<weight;w++)
		{	
			*(tmpdata+w+h*weight)=LED_IMAGE_BLACK;//非线的部分 设置为黑色 不显示
			if((*(data+w+h*weight)==255)&&(*(data+w-1+h*weight)==0))	*(tmpdata+w+h*weight) =	LED_IMAGE_WHITE;
			if((*(data+w+h*weight)==255)&&(*(data+w+1+h*weight)==0))	*(tmpdata+w+h*weight) =	LED_IMAGE_WHITE;	
			if((*(data+w-1+h*weight)==255)&&(*(data+w+h*weight)==0))	*(tmpdata+w+h*weight) =	LED_IMAGE_WHITE;	
			if((*(data+w+1+h*weight)==255)&&(*(data+w+h*weight)==0))	*(tmpdata+w+h*weight) =	LED_IMAGE_WHITE;	
		}
	}
	//2。查找上一步图里的 所有的白点 周围的8个点 如果有超过2个(不包含自己)点 则视为在某条直线上 记录为白点 否则删除掉 不做记录 存为一张图
	//不扫四个角的像素点
	
	/*
	for(h=1;h<height-1;h++)
	{
		for(w=1;w<weight-1;w++)
		{	
		if(*(tmpdata+w+h*weight)==LED_IMAGE_WHITE)
		{
			temp=0;//每计算一个点 把统计的点数重记下
			for(i=-1;i<=1;i++)//-1 到 +1
			{
				for(j=-1;j<=1;j++)
				{
					if(*(tmpdata+w+h*weight)==*(tmpdata+w+h*weight   +i+h ))temp++;//temp存储数到的周围白点个数
				}
			}				
		//判断是否为一条线上的点 如果不是则去掉这个点 
		if(temp>2) *(tmpdata+w+h*weight) = LED_IMAGE_WHITE;
			else   *(tmpdata+w+h*weight) = LED_IMAGE_BLACK;		
		}//end if

		}//end for h
	}//end for w*/
	//3。重复第二步 以消除横竖 短线 （先不做 看上一步效果而定 ）
	//4。从左往右扫全图 如果扫到 白点 继续想右扫 直到扫到距这个白点的第二个白点 则把这两个白点合成一个白点 

	for(h=0;h<height;h++)
	{
		for(w=0;w<weight;w++)
		{
			
			if( *(tmpdata+w+h*weight) == LED_IMAGE_WHITE)
			{
				//数到第一个点
				for(i=w;i<weight;i++)	
				{
					if(*(tmpdata+w+h*weight  +i) == LED_IMAGE_WHITE)
					{
						//数到第二个点
						for(j=i;j<weight;j++)
						{
							if(*(tmpdata+w+h*weight +j) == LED_IMAGE_WHITE)  
							{
								lineLeft[h]=w+j/2;
								break;
							}
						}//end for j
						//没有点 则补线 //4.1。如果超过一半没有右边的点 则给右边补一条边线 作为右边的白点
						lineLeft[h]=(w+79)/2;
						break;//跳出i 循环					
					}								
				}//end for i
			}		
		}//end for w
	}//end for h
	//5。从右往左扫全图 如果扫到白点 继续向左扫 直到扫到距离这个白点的第二个白点 则把这两个白点合成一个白点
	
	for(h=0;h<height;h++)
	{
		for(w=79;w>0;w--)
		{
			
			if( *(tmpdata+w+h*weight) == LED_IMAGE_WHITE)
			{
				//数到第一个点
				for(i= w;i>0;i--)	
				{
					if(*(tmpdata+w+h*weight  +i) == LED_IMAGE_WHITE)
					{
						//数到第二个点
						for(j=i;j>0;j--)
						{
							if(*(tmpdata+w+h*weight +j) == LED_IMAGE_WHITE)  
							{
								lineRight[h]=w+j/2;
								break;
							}
						}//end for j
						//没有点 则补线 //5.1。如果超过一边没有左边的点 则给左边补一条边线 作为左边的白点
						lineRight[h]=(w+0)/2;
						break;//跳出i 循环					
					}								
				}//end for i
			}		
		}//end for w
	}//end for h
	
	//6。对4 和 5 白点的位置做平均 然后合成路径
	for(h=0;h<height;h++)
	{
		for(w=0;w<weight;w++)
		{
			line[h][w]= LED_IMAGE_BLACK;//默认没有线
			lineP[h] = (lineLeft[h] + lineRight[h])/2;
			if (w ==lineP[h]  )
			{
				line[h][w]= LED_IMAGE_WHITE;
				*(tmpdata+w+h*weight) = LED_IMAGE_WHITE;//显示计算后中线
			}
			
		}
	}
	
	//7。计算路径曲率  取 h0 = 5  h1 = 	10  h2 = 15
	float k = 2 * ((lineP[h1] - lineP [h0]) * (h2 - h0)  -  (lineP[h2] - lineP[h0]) * (h1 - h0) )  //分子
	/
	(  m_sqrt( ( h1 - h0)*(h1 - h0) + (lineP[h1] - lineP[h0]) *(lineP[h1] - lineP[h0]) ) //分母1 
	+ m_sqrt( (h2 - h1) *(h2 - h1)  + (lineP[h2] - lineP[h1]) *(lineP[h2] - lineP[h1]) ) //分母2
	+ m_sqrt( (h2 - h0) *(h2 - h0)  + (lineP[h2] - lineP[h0]) *(lineP[h2] - lineP[h0]) ) //分母3
	) ;
	
	//8。判断曲率 如果为负 则向右转 如果为正 则向左转 （直接返回这个值）
	memcpy((char*)data,(char*)tmpdata,  hight * width * sizeof(uint8));
	free(tmpdata);
	return k;

}



//平滑处理 去除噪点等影响
void gaussianFilter(uint8 *data,uint16 width,uint16 hight)
{
	int i, j, index, sum;
	int templates[9] = { 1, 2, 1,2, 4, 2, 1, 2, 1 };
	//int Laplacian[9] ={ -1, -1, -1, -1, 9, -1, -1, -1, -1 };
	sum = hight * width * sizeof(uint8);
	uint8 *tmpdata = (uint8*) malloc(sum);
	memcpy((char*)tmpdata,(char*)data, sum);
	for(i = 1;i < hight - 1;i++)
	{
		for(j = 1;j < width - 1;j++)
		{
			index = sum = 0;
			for(int m = i - 1;m < i + 2;m++)
			{
				for(int n = j - 1; n < j + 2;n++)
				{
					sum += 
					tmpdata[m * width + n] * 
					templates[index++];
				}
			}
			data[i * width + j] = sum / 16;
		}
	}
	free(tmpdata);
}