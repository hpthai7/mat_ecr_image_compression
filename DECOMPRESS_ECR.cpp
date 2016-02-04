#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <dos.h>
#include <dir.h>
#include <ctype.h>

#define log2(a) ((log(a))/(log(2)))
#define WID 1024
#define HEI 1024

struct tagBITMAPFILEHEADER
   {
		unsigned short int 	wType;
		unsigned long  int	dwSize;
		unsigned short int	wReserved1;
		unsigned short int	wReserved2;
		unsigned long  int	dwOffBits;
	};

struct tagBITMAPINFOHEADER
	{
		unsigned long  int	dwSize;
		unsigned long  int	dwWidth;
		unsigned long  int	dwHeight;
		unsigned short int	wPlanes;
		unsigned short int	wBitCount;
		unsigned long  int	dwCompression;
		unsigned long  int	dwSizeImage;
		unsigned long  int	dwXPelsPerMeter;
		unsigned long  int	dwYPelsPerMeter;
		unsigned long  int	dwClrUsed;
		unsigned long  int	dwClrImportant;
	};

struct tagRGBQUAD
	{
		unsigned char	bBlue;
		unsigned char	bGreen;
		unsigned char	bRed;
		unsigned char	bReserved;
	};
struct tagFRE
   { unsigned value : 24;
   };
// Define functions
char bit (int index,unsigned short x); // convert DEC->BIN
int value(int b4,int b3, int b2, int b1, int b0);
void cal_fre(int i);
void lowerIt(char str[]);
void cal_fre1(int i);

int cal_w (struct tagFRE f[][32]);
int findsymbol(int i,int j,int b0 );
int findsymbol1(int i,int j,int b0 );

void decode(int &i, struct tagFRE f[32],
 				unsigned long &x,unsigned long &y,
				unsigned long v,unsigned long u,
           	int &posin,int &valin,int bit_no,FILE *filename,unsigned long &counter);
void decode1(int &i, struct tagFRE f[32],
 				unsigned long &x,unsigned long &y,
				unsigned long v,unsigned long u,
           	int &posin,int &valin,int bit_no,FILE *filename,unsigned long &counter);           	
int input(int &vitri,int &byte,FILE *codedfile,unsigned long chieudai,
		unsigned long &counter);
		
void decoder(int way);
void decoder1(int way);

void restorebmp(int way);
void create_iwt_matrix(void);
void check(int way);

void readinfo(void);
void readinfo1(void);
void findcp(void);
void findcp1(void);

void readcp();
void readcp1();
// Define global variables
char orig_name[MAXFILE];       /* original filename */
short alpict[HEI][WID];
float B[HEI][WID];
float C[HEI][WID];
float D[HEI][WID];
unsigned char paletteRev;
unsigned char pict[HEI][WID];  /* matrix content data of bmp */
unsigned short n_grey;          /* Number of real gray level in bmp file */
unsigned short p1[1024];         /* Store pseudo gray level table */
char **bit_plane[8];
char **bit_plane1[8];
char **refer_plane[7];
char **refer_plane1[7];
int wid,hei;                   /* Width and Heigth of bmp file */
int w;								 /* W value use for C.B.J algorithm */
int bit_no,row,col,way;
unsigned long fi, fim1, u;     /* f[i],f[i-1],u */
struct tagFRE f[8][32],p_stored[8][32],p_stored1[8][32];
unsigned long len,len1;             /* Store len codeword of each bitplane(3 ways)*/
unsigned long position;
unsigned long offset;          /* Save offset of data image for compare later!!*/
FILE *f_c;                  /* File temp to create codeword */
char intensity[32];

float alpha[8],beta[8],alpha1[8],beta1[8];
int lencp,lencp1;
int *temp,*temp1;
short mini,maxi,mini1;
unsigned long p_sto1code;
unsigned long p_stocode;
unsigned long p_stocom;
unsigned long p_sto1com;
unsigned long al;
unsigned long al1;
short b;

void main(void)
{

  int i,j,bit,k,m;
 do

 {

  clrscr();

  textcolor(YELLOW);
  cprintf ("Program decompresses for bmp image\n\r");
  cprintf ("GVHD: TS. Dang Thanh Tin\n\r");
  cprintf ("SVTH: Tran Dai Viet - Ho Phu Thai\n\r");
  
  // Create refer plane for decompress

   for (i = 0; i < 7; i++)
  	 { refer_plane[i] = (char **) calloc (HEI*WID, sizeof (char));
    	if (refer_plane[i] == (char **)NULL)
      {  printf ("Cannot allocate dynamic array for reference planes !");
      	getch();
         exit (1);
    	}
  	 }

  for(bit=0;bit<7;bit++)
  	 for(k=0;k<HEI;k++)
    	 for(m=0;m<WID;m++)
    	 	*((char *)refer_plane[bit]+k*WID+m)=0;

   // Create bitplanes
   for (int i = 0; i < 8; i++)
		{ bit_plane[i] = (char **) calloc (HEI*WID, sizeof (char));
   	  if (bit_plane[i] == (char **)NULL)
    	  {  printf ("Cannot allocate dynamic array for bit planes !");
           getch();
      	  exit (1);
    	  }
  		}
//////////////////////////////////////////////////////////////////
   // Create refer plane1 for decompress

   for (i = 0; i < 7; i++)
  	 { refer_plane1[i] = (char **) calloc (HEI*WID, sizeof (char));
    	if (refer_plane1[i] == (char **)NULL)
      {  printf ("Cannot allocate dynamic array for reference planes !");
      	getch();
         exit (1);
    	}
  	 }

  for(bit=0;bit<7;bit++)
  	 for(k=0;k<HEI;k++)
    	 for(m=0;m<WID;m++)
    	 	*((char *)refer_plane1[bit]+k*WID+m)=0;

   // Create bitplane1
   for (int i = 0; i < 8; i++)
		{ bit_plane1[i] = (char **) calloc (HEI*WID, sizeof (char));
   	  if (bit_plane1[i] == (char **)NULL)
    	  {  printf ("Cannot allocate dynamic array for bit planes !");
           getch();
      	  exit (1);
    	  }
  		}  		
  //////////////////////////////////////////////////////////////////////////////

  readinfo();

  // Decoded file

  decoder (way);
  
  if ((way==2)&&((maxi-mini)>256))
  {
  	readinfo1();
   decoder1(way);
  }

  // Restore bmp file

  restorebmp(way);
  // Check source file and restore file
/*for (i=0;i<7;i++)
             {
               for (j=0;j<7;j++)
               {
                 printf(" %d ",pict[i][j]);
               }
               printf("\n");
             }
             getch();*/
  check(way);
  /*
  // Free memory
  for (i = 0; i < 8; i++)

  free(bit_plane[i]);
  for (i = 0; i < 7; i++)
  free(refer_plane[i]);

  */

  cprintf("\n\rEnter to continue, another key to exit");

 }
while(getch()==13);
}

////////////////////////////////////////////////////////////////////////////////

// Implementation functions
char bit (int index,unsigned short x)
{ int mask;
  switch(index)
  {
  case 0 : 	mask=0x01;
            break;
  case 1	:	mask=0x02;
            break;
  case 2	:  mask=0x04;
            break;
  case 3	:  mask=0x08;
            break;
  case 4	:  mask=0x10;
            break;
  case 5 :  mask=0x20;
            break;
  case 6	:  mask=0x40;
            break;
  case 7	:  mask=0x80;
            break;
  };
  return (char)((x & mask)? 1:0);
}

void create_iwt_matrix(void) // Inverse transform
{
	int i,j;
	if (mini>-205)
	{
		for (i=0;i<hei;i++)
		for (j=0;j<wid;j++)
		alpict[i][j] = alpict[i][j] + mini;
	}	
	else 
	{
	for (i=0;i<hei;i++)
	 {
		for (j=0;j<wid;j++)
	   {
		if ((i<hei/2)&&(j<wid/2))
		alpict[i][j] = alpict[i][j] + mini1;
		else
		alpict[i][j] = alpict[i][j] + mini;
	   }	
	 }
	}
	for (i=0;i<hei;i++)
	for (j=0;j<wid;j++)
		C[i][j] = (float)alpict[i][j]-b;
	// Columns' transformation
	for (j=0;j<wid;j++)
	{
		for (i=1;i<hei/2;i++)
		B[2*i-1][j] = C[i-1][j] - floor((C[i+hei/2-1][j]+C[i+hei/2][j]+2)/4);
		B[hei-1][j] = C[hei/2-1][j] - floor((C[hei-1][j]+1)/2);
	}
	for (j=0;j<wid;j++)
	{
		for (i=1;i<hei/2;i++)
		B[2*i][j] = C[i+hei/2][j] + floor((B[2*i-1][j]+B[2*i+1][j]+1)/2);
		B[0][j] = C[hei/2][j] + floor((float)B[1][j]+1/2);
	}
	// Rows' transformation
	for (i=0;i<hei;i++)
	{
		for (j=1;j<wid/2;j++)
		D[i][2*j-1] = B[i][j-1] - floor((B[i][j+wid/2-1]+B[i][j+wid/2]+2)/4);
		D[i][wid-1] = B[i][wid/2-1] - floor((B[i][wid-1]+1)/2);
	}
	for (i=0;i<hei;i++)
	{
		for (j=1;j<wid/2;j++)
		D[i][2*j] = B[i][j+wid/2] + floor((D[i][2*j-1]+D[i][2*j+1]+1)/2);
		D[i][0] = B[i][wid/2] + floor((float)D[i][1]+1/2);
	}
	for (i=0;i<hei;i++)
	for (j=0;j<wid;j++)
		pict[i][j] = (unsigned char)D[i][j];

}

int value(int b4,int b3, int b2, int b1, int b0)
{    return b4*16 + b3*8 + b2*4 + b1*2 + b0; }


int findsymbol(int i,int j,int b0 )
{ int symbol;
  int b1,b2,b3,b4;
   if(bit_no==7)
  b4=0;
  else
  b4=*((char *)bit_plane[bit_no+1] +i*wid+j);
  if(i==0&&j==0)
    	b3=b2=b1=0;
  else if(i==0&&j!=0)
  	{ 	b3=b2=0;
    	b1=*((char *)bit_plane[bit_no]+i*wid+j-1);
  	}
  else if(i!=0&&j==0)
  	{ 	b3=b1=0;
    	b2=*((char *)bit_plane[bit_no]+(i-1)*wid+j);
  	}
  else if(i!=0 && j!=0)
  	{	b3 = *((char *)bit_plane[bit_no]+(i-1)*wid+j-1);
    	b2 = *((char *)bit_plane[bit_no]+(i-1)*wid+j);
    	b1 = *((char *)bit_plane[bit_no]+i*wid+j-1);
  	}

  symbol=value(b4,b3,b2,b1,b0);
  return symbol;
}

int findsymbol1(int i,int j,int b0 )
{ int symbol;
  int b1,b2,b3,b4;
   if(bit_no==7)
  b4=0;
  else
  b4=*((char *)bit_plane1[bit_no+1] +i*wid+j);
  if(i==0&&j==0)
    	b3=b2=b1=0;
  else if(i==0&&j!=0)
  	{ 	b3=b2=0;
    	b1=*((char *)bit_plane1[bit_no]+i*wid+j-1);
  	}
  else if(i!=0&&j==0)
  	{ 	b3=b1=0;
    	b2=*((char *)bit_plane1[bit_no]+(i-1)*wid+j);
  	}
  else if(i!=0 && j!=0)
  	{	b3 = *((char *)bit_plane1[bit_no]+(i-1)*wid+j-1);
    	b2 = *((char *)bit_plane1[bit_no]+(i-1)*wid+j);
    	b1 = *((char *)bit_plane1[bit_no]+i*wid+j-1);
  	}

  symbol=value(b4,b3,b2,b1,b0);
  return symbol;
}

int cal_w (struct tagFRE f[][32])
{
  int i,j,temp1=0,temp2=0,dw=0,dw1=0;
 for(i=0;i<8;i++)
  for (j = 0; j < 32; j+=2)
    	{  u = f[i][j+1].value;
         if (f[i][j].value==0 && f[i][j+1].value==0)
      	continue;

         if (f[i][j].value!=0 && f[i][j+1].value!=f[i][j].value)
      		{	temp1 = (int)(log2((double)u/f[i][j].value));
		      	temp2 = (int)(log2( (double)u/(f[i][j+1].value- f[i][j].value)));
      			dw1=(temp1<=temp2?temp2:temp1);
      		}
      	dw=(dw<=dw1?dw1:dw);
    	}

   return dw;
}

void cal_fre(int i)
{
   // Neu la bit 0 xet trong 3 truong hop
   if (i % 2 == 0)
        {  if (6 >= bit_no && bit_no >= 0 && *((char *)refer_plane[6-bit_no]+row*wid+col )==2)
          	{  // Bit 0 trong vung R tang xac suot
            	fim1 = 0;
            	fi = f[bit_no][i].value+(unsigned long)floor(beta[bit_no]*(f[bit_no][i+1].value-f[bit_no][i].value));
            	u = f[bit_no][i+1].value;
          	}
           else if (6 >= bit_no && bit_no >= 0 && *((char *)refer_plane[6-bit_no]+row*wid+col )==1)
            {  // Bit 0 trong vung A giam xac suot
            	fim1 = 0;
            	fi = f[bit_no][i].value-(unsigned long)floor(alpha[bit_no]*f[bit_no][i].value);
            	u = f[bit_no][i+1].value;
          	}
           else
          	{  // Bit 0 khong nam trong hai vung A, R, tinh theo tan so ban dau
            	fim1 = 0;
            	fi = f[bit_no][i].value;
            	u = f[bit_no][i+1].value;
          	}
        }
   // Neu la bit 1 thi cung xet 3 truong hop
   else
        {  if (6 >= bit_no && bit_no >= 0 && *( (char *)refer_plane[6-bit_no]+row*wid+col )==1)
          	{  // Bit 1 trong vung A tang xac suot
            	fim1 =f[bit_no][i-1].value-(unsigned long)floor(alpha[bit_no]*f[bit_no][i-1].value) ;
            	fi = f[bit_no][i].value;
            	u = f[bit_no][i].value;
          	}
           else if (6 >= bit_no && bit_no >= 0 && *( (char *)refer_plane[6-bit_no]+row*wid+col )==2)
          	{  // Bit 1 trong vung R giam xac suot
            	fim1 =f[bit_no][i-1].value + (unsigned long)floor(beta[bit_no]*(f[bit_no][i].value-f[bit_no][i-1].value));
            	fi = f[bit_no][i].value;
            	u = f[bit_no][i].value;
          	}
           else
          	{
            	fim1 = f[bit_no][i-1].value;
            	fi = f[bit_no][i].value;
            	u = f[bit_no][i].value;
          	}
        }
}

void cal_fre1(int i)
{
   // Neu la bit 0 xet trong 3 truong hop
   if (i % 2 == 0)
        {  if (6 >= bit_no && bit_no >= 0 && *((char *)refer_plane1[6-bit_no]+row*wid+col )==2)
          	{  // Bit 0 trong vung R tang xac suot
            	fim1 = 0;
            	fi = f[bit_no][i].value+(unsigned long)floor(beta1[bit_no]*(f[bit_no][i+1].value-f[bit_no][i].value));
            	u = f[bit_no][i+1].value;
          	}
           else if (6 >= bit_no && bit_no >= 0 && *((char *)refer_plane1[6-bit_no]+row*wid+col )==1)
            {  // Bit 0 trong vung A giam xac suot
            	fim1 = 0;
            	fi = f[bit_no][i].value-(unsigned long)floor(alpha1[bit_no]*f[bit_no][i].value);
            	u = f[bit_no][i+1].value;
          	}
           else
          	{  // Bit 0 khong nam trong hai vung A, R, tinh theo tan so ban dau
            	fim1 = 0;
            	fi = f[bit_no][i].value;
            	u = f[bit_no][i+1].value;
          	}
        }
   // Neu la bit 1 thi cung xet 3 truong hop
   else
        {  if (6 >= bit_no && bit_no >= 0 && *( (char *)refer_plane1[6-bit_no]+row*wid+col )==1)
          	{  // Bit 1 trong vung A tang xac suot
            	fim1 =f[bit_no][i-1].value-(unsigned long)floor(alpha1[bit_no]*f[bit_no][i-1].value) ;
            	fi = f[bit_no][i].value;
            	u = f[bit_no][i].value;
          	}
           else if (6 >= bit_no && bit_no >= 0 && *( (char *)refer_plane1[6-bit_no]+row*wid+col )==2)
          	{  // Bit 1 trong vung R giam xac suot
            	fim1 =f[bit_no][i-1].value + (unsigned long)floor(beta1[bit_no]*(f[bit_no][i].value-f[bit_no][i-1].value));
            	fi = f[bit_no][i].value;
            	u = f[bit_no][i].value;
          	}
           else
          	{
            	fim1 = f[bit_no][i-1].value;
            	fi = f[bit_no][i].value;
            	u = f[bit_no][i].value;
          	}
        }
}

void lowerIt(char str[])
{
  int i=0;
  char c;
  while (str[i])
  {
    c=str[i];
    str[i] = tolower(c);
    i++;
  }
}

void decode(int &i, struct tagFRE f[32],
				unsigned long &x,unsigned long &y,
				unsigned long v,unsigned long u,
            int &posin,int &valin,int bit_no,FILE *filename,unsigned long &counter)

{
  int j;
  unsigned long e,z;
  while (y<v)
    {
    j=input(posin,valin,filename,len,counter);
    x=x*2+j-1;
    y=y*2;
    }
  e=(unsigned long)floor(((float)u*(2*x+1)-1)/(2*y));
  i-=2;
  unsigned long temp;
  do{ i++;
         if (i % 2 == 0)
        {
          temp = f[i].value;

          if (6 >= bit_no && bit_no >= 0 && *((char *)refer_plane[6-bit_no]+row*wid+col )==2) // bit 0 trong vung R tang xs
          	  temp =f[i].value +(unsigned long)(beta[bit_no]*(f[i+1].value-f[i].value));

          if (6 >= bit_no && bit_no >= 0 && *((char *)refer_plane[6-bit_no]+row*wid+col)==1) // bit 0 trong vung A giam xs
              	temp =f[i].value -(unsigned long)(alpha[bit_no]*f[i].value);
        }
       else
        {
          temp = f[i].value;


          if (6 >= bit_no && bit_no >= 0 && *( (char *)refer_plane[6-bit_no]+row*wid+col )==1)
          temp = f[i].value;

          if (6 >= bit_no && bit_no >= 0 && *((char *)refer_plane[6-bit_no]+row*wid+col )==2)
          temp =f[i].value;
         }
   }
  while (temp<=e);
  cal_fre(i);
  z=(unsigned long)floor(y*((float)fim1/u)+ 0.5);
  x-=z;
  y=(unsigned long)floor(y*((float)fi/u)  + 0.5)-z;
}

void decode1(int &i, struct tagFRE f[32],
				unsigned long &x,unsigned long &y,
				unsigned long v,unsigned long u,
            int &posin,int &valin,int bit_no,FILE *filename,unsigned long &counter)

{
  int j;
  unsigned long e,z;
  while (y<v)
    {
    j=input(posin,valin,filename,len1,counter);
    x=x*2+j-1;
    y=y*2;
    }
  e=(unsigned long)floor(((float)u*(2*x+1)-1)/(2*y));
  i-=2;
  unsigned long temp;
  do{ i++;
         if (i % 2 == 0)
        {
          temp = f[i].value;

          if (6 >= bit_no && bit_no >= 0 && *((char *)refer_plane1[6-bit_no]+row*wid+col )==2) // bit 0 trong vung R tang xs
          	  temp =f[i].value +(unsigned long)(beta1[bit_no]*(f[i+1].value-f[i].value));

          if (6 >= bit_no && bit_no >= 0 && *((char *)refer_plane1[6-bit_no]+row*wid+col)==1) // bit 0 trong vung A giam xs
              	temp =f[i].value -(unsigned long)(alpha1[bit_no]*f[i].value);
        }
       else
        {
          temp = f[i].value;


          if (6 >= bit_no && bit_no >= 0 && *( (char *)refer_plane1[6-bit_no]+row*wid+col )==1)
          temp = f[i].value;

          if (6 >= bit_no && bit_no >= 0 && *((char *)refer_plane1[6-bit_no]+row*wid+col )==2)
          temp =f[i].value;
         }
   }
  while (temp<=e);
  cal_fre1(i);
  z=(unsigned long)floor(y*((float)fim1/u)+ 0.5);
  x-=z;
  y=(unsigned long)floor(y*((float)fi/u)  + 0.5)-z;
}

int input(int &vitri,int &byte,FILE *codedfile,unsigned long chieudai,
		unsigned long &counter)
{
  counter++;
  if (vitri==-1)
    {
    byte=fgetc(codedfile);
    vitri=7;
    }
  if (counter>chieudai) return 2;
  else
    {
    div_t y;
    y=div(byte,(int)pow(2,vitri));
    byte=y.rem;
    vitri-=1;
    return (y.quot+1);
    }
}

void decoder (int way)                                            // void encode
{
  int i, j, temp, t;
  int b0;


  // store fre tables of 8 bit planes
for(i=0;i<8;i++)
  	for (j = 0; j < 32; j++)
      f[i][j].value=p_stored[i][j].value;

  // Fre. table of the each  bit plane
  for(i=0;i<8;i++)
   for (j = 0; j < 32; j+=2)
     f[i][j+1].value += f[i][j].value;

  // Calculate w
  w=cal_w(f);

  // Decoding
  cprintf ("Begin decoding 8 first bitplanes\n\r");
  unsigned long v;
  v=pow(2,w);
  unsigned long yy = 1;
  unsigned long xx=0;                                        // CALCULATE VALUE OF Y0=2^W
  int posin=-1,valin=0;
  unsigned long counter=0;


  for(bit_no=7;bit_no>=0;bit_no--)
  {
    row=col=0;
    while(row<hei)
    {
      b0=1;
      int symbol;
      symbol = findsymbol(row,col, b0);
      u=f[bit_no][symbol].value;
      decode(symbol,f[bit_no],xx,yy,v,u,posin,valin,bit_no,f_c,counter);
      *((char *)bit_plane[bit_no]+row*wid+col)=symbol-findsymbol(row,col,0);

      col++;
         if(col==wid)
         {col=0; row++;
         }
     }
     printf("Decode bit plane %i passed\n",bit_no);

}
     cprintf("End decoding 8 first bitplanes\n\r");
     fcloseall();
}

void decoder1 (int way)                                            // void encode
{
  int i, j, temp, t;
  int b0;


  // store fre tables of 8 bit planes
for(i=0;i<8;i++)
  	for (j = 0; j < 32; j++)
      f[i][j].value=p_stored1[i][j].value;

  // Fre. table of the each  bit plane
  for(i=0;i<8;i++)
   for (j = 0; j < 32; j+=2)
     f[i][j+1].value += f[i][j].value;

  // Calculate w
  w=cal_w(f);

  // Decoding
  cprintf ("Begin decoding 8 later bitplanes\n\r");
  unsigned long v;
  v=pow(2,w);
  unsigned long yy = 1;
  unsigned long xx=0;                                        // CALCULATE VALUE OF Y0=2^W
  int posin=-1,valin=0;
  unsigned long counter=0;


  for(bit_no=7;bit_no>=0;bit_no--)
  {
    row=col=0;
    while(row<hei)
    {
      b0=1;
      int symbol;
      symbol = findsymbol1(row,col, b0);
      u=f[bit_no][symbol].value;
      decode1(symbol,f[bit_no],xx,yy,v,u,posin,valin,bit_no,f_c,counter);
      *((char *)bit_plane1[bit_no]+row*wid+col)=symbol-findsymbol1(row,col,0);

      col++;
         if(col==wid)
         {col=0; row++;
         }
     }
     printf("Decode bit plane %i passed\n",bit_no+8);

}
     cprintf("End decoding 8 later bitplanes\n\r");
     fcloseall();
}

void restorebmp(int way)
{
   struct tagBITMAPFILEHEADER bmfh;
   struct tagBITMAPINFOHEADER bmih;
   struct tagRGBQUAD rgb[256];
   char outfile_name[MAXFILE+MAXEXT];
   char ext[10];
   int i,j;
   char gray_bit;
   FILE *restore;


 for(i=0;i<hei;i++)
 for(j=0;j<wid;j++)
 { 
   int temp=0;
   int bit[8];
 for (bit_no = 7; bit_no >= 0; bit_no--)
  { bit[bit_no]=*((char *)bit_plane[bit_no]+i*wid+j);
    if(way!=0 && bit_no<7)
    bit[bit_no] = bit[bit_no]^bit[bit_no + 1];
     // convert qua gia tri thap phan de luu vao alpict[i][j]
    temp+=bit[bit_no]*pow(2,bit_no);
  }
  alpict[i][j]=temp;
 }
 if ((way==2)&&((maxi-mini)>256))
 {
   for(i=0;i<hei;i++)
   for(j=0;j<wid;j++)
   { 
     int temp1=0;
     int bit1[8]; 
   for (bit_no = 7; bit_no >= 0; bit_no--)
   { bit1[bit_no]=*((char *)bit_plane1[bit_no]+i*wid+j);
    if(way!=0 && bit_no<7)
    bit1[bit_no] = bit1[bit_no]^bit1[bit_no + 1];
     // convert qua gia tri thap phan de luu vao alpict[i][j]
    temp1+=bit1[bit_no]*pow(2,bit_no+8);
    }
  alpict[i][j]=alpict[i][j]+temp1;
   }
}
 if (way==2)
create_iwt_matrix();
 else
 {
 	for (i=0;i<hei;i++)
 	for (j=0;j<wid;j++)
 	pict[i][j]=alpict[i][j];
 }
  // change gray value
 for(i=0;i<hei;i++)
 for(j=0;j<wid;j++)
  pict[i][j]=p1[pict[i][j]];
/*for (i=0;i<7;i++)
             {
               for (j=0;j<7;j++)
               {
                 printf(" %d ",pict[i][j]);
               }
               printf("\n");
             }
             getch();*/
 unsigned char temp;
 for (i = 0; i < hei/2; i++)
    for (j = 0; j < wid; j++)
    {
      temp = pict[i][j];
      pict[i][j]  = pict[hei-i-1][j];
      pict[hei-i-1][j] = temp;
    }

    // Create BITMAPFILEHEADER
  bmfh.wType = 0x4d42;
  bmfh.dwSize = wid*hei+1078;
  bmfh.wReserved1 = 0;
  bmfh.wReserved2 = 0;
  bmfh.dwOffBits = 0x0436;
  // Create BITMAPINFOHEADER
  bmih.dwSize = 40;
  bmih.dwWidth = wid;
  bmih.dwHeight = hei;
  bmih.wPlanes = 1;
  bmih.wBitCount = 8;
  bmih.dwCompression = 0;
  bmih.dwSizeImage = hei*wid;
  bmih.dwXPelsPerMeter = 0;
  bmih.dwYPelsPerMeter = 0;
  bmih.dwClrUsed = 0;
  bmih.dwClrImportant = 0;
  // Create color palette
  char tempc[256];
  int count=0;
  for(i=0;i<32;i++)
  for(j=0;j<8;j++)
   {tempc[8*i+j]=bit(j,intensity[i]);
    if (tempc[8*i+j]==1)
    { rgb[count].bBlue=(unsigned char)(8*i+j);
 		rgb[count].bGreen=(unsigned char)(8*i+j);
 		rgb[count].bRed=(unsigned char)(8*i+j);
 		rgb[count].bReserved=0;
      count++;
    }
   }
  if (count < 256)
	{
		if (paletteRev == 1)
		for (i=0;i<256;i++)
		{
			rgb[i].bBlue = 255 - i;
			rgb[i].bGreen = 255 - i;
			rgb[i].bRed = 255 - i;
			rgb[i].bReserved = 0;
		}
	}

  // Create bmp  file
  strcpy(outfile_name,orig_name);
  sprintf(ext,"%d_gn.bmp",way);
  strcat(outfile_name,ext);
  if ( (restore = fopen (outfile_name, "wb")) == NULL )
    	{ printf ("Cannot create bmp file");
        getch();
        exit (1);
   	}
 fwrite (&bmfh, sizeof(struct tagBITMAPFILEHEADER), 1, restore);
 fwrite (&bmih, sizeof(struct tagBITMAPINFOHEADER), 1,restore);
 for (i=0; i<256; i++)
 fwrite (&rgb[i], sizeof(struct tagRGBQUAD), 1, restore);
 fseek(restore,0x0436,SEEK_SET);
  for(i=0;i<hei;i++)
  for(j=0;j<wid;j++)
     fwrite(&pict[i][j],sizeof(pict[i][j]),1,restore);
 fclose(restore);
 cprintf("Finish restore bmp file\n\r");
  for(i=0;i<8;i++)
  free(bit_plane[i]);
  for(i=0;i<7;i++)
  free(refer_plane[i]);

}



////////////////////////////////////////////////////////////////////////////////
void check(int way)
 {   unsigned char picts,pictd;
     int same;
     char orgfile_name[MAXFILE+MAXEXT];
     char resfile_name[MAXFILE+MAXEXT];
     char ext[10];
     FILE *source,*restored;

     strcpy(orgfile_name,orig_name);
  	  sprintf(ext,".bmp");
     strcat(orgfile_name,ext);
  if ( (source = fopen (orgfile_name, "rb")) == NULL )
    	{ printf ("Cannot open source bmp file");
        exit (1);
   	}
  
     strcpy(resfile_name,orig_name);
     sprintf(ext,"%d_gn.bmp",way);
     strcat(resfile_name,ext);
  if ( (restored = fopen (resfile_name, "rb")) == NULL )
    	{ printf ("Cannot open restored bmp file");
        exit (1);
   	}

  // Read the image dimension
  fseek (source, 18, 0);
  fread (&wid, sizeof(short), 1, source);
  fseek (source, 22, 0);
  fread (&hei, sizeof(short), 1, source);

  // Create pitcs and pictd
  // Load data of source bmp and decoded bmp into pict matrixs to compare
  // Read raw data from BMP file into pict[hei][wid]: fixed
  fseek (source, offset, 0);
  fseek (restored, 0x0436, 0);
  same=1;

  for (int i = 0; i < hei*wid; i++)

  { fread (&picts,sizeof(picts), 1, source);
    fread (&pictd,sizeof(pictd), 1, restored);
    // Compare two matrix
     if(picts!=pictd)
         { same=0;

           printf("Decode failed! There are many different beetween two file!\n");
           printf("value of sourc %d, value of restored %d\n",picts,pictd);
           printf("Error from : Row= %d * Col= %d \n", i/wid,i%wid);
           break;
          }
  }
    if(same==1)
    printf("Decode sucessfully! Don't have any different beetween two file!\n");

   fcloseall();

}


// Co the sai la tu day day.
void readinfo(void)
{
	char fn[MAXFILE];
  	char drive[MAXDRIVE],              /* needed for fnsplit()   */
        dir[MAXDIR],                  /* needed for fnsplit()   */
        filename[MAXFILE],            /* 8 chars, orig name     */
        ext[MAXEXT];                  /* 3 chars, orig ext      */
  	int i,j,k,m,bit;
    short a;

  flushall();
  printf ("Please enter an code filename (ENTER for ZELDA.OUT2): ");
  gets (fn);

  if (!strcmp (fn, ""))
  {	strcpy (fn, "zelda.out2");fnsplit(fn, drive, dir, filename, ext);
	b=4;	}
  else
  {	lowerIt(fn);fnsplit(fn, drive, dir, filename, ext);
	if (!strcmp(filename,"zelda")||!strcmp(filename,"couple")||!strcmp(filename,"bieutinh")||
		!strcmp(filename,"blena")||!strcmp(filename,"cube"))
		b=4;
	else if (!strcmp(filename,"moon")||!strcmp(filename,"chest")||!strcmp(filename,"joint")||
		!strcmp(filename,"clown")||!strcmp(filename,"right")||!strcmp(filename,"mountain"))
		b=3;
	else if (!strcmp(filename,"mandrill"))
		b=14;
	else if (!strcmp(filename,"girl")||!strcmp(filename,"fruit1"))
		b=2;
	else if (!strcmp(filename,"lena")||!strcmp(filename,"disp8")||!strcmp(filename,"scott")||
		!strcmp(filename,"oranges"))
		b=1;
	else if (!strcmp(filename,"stan")||!strcmp(filename,"fruit"))
		b=5;
	else
	{	printf("Enter an arbitrary integer from the set {1,2,3,...,20}\n");
		printf("\t(If decompression fails, try a different value).\n");
		printf("a = ");
		scanf("%d",&a);
		b=a;	}
  }

  // Save origin file name ( not include extension)
  strcpy(orig_name,filename);

  if(( f_c = fopen(fn, "rb")) == NULL)
  {
    printf ("Cannot open coded  file for decompressing\n");
    printf ("Press anykey for END\n");
    getch();
    exit (1);
  }

  cprintf("Please wait while reading information\n\r");

  // Read header of file for decompress data
  fread(&wid,sizeof(wid),1,f_c);
  fread(&hei,sizeof(hei),1,f_c);
  fread(&paletteRev,sizeof(paletteRev),1,f_c);
  printf("paletteRev = %d\n",paletteRev);
  fread(&mini,sizeof(mini),1,f_c);
  fread(&mini1,sizeof(mini1),1,f_c);
  fread(&maxi,sizeof(maxi),1,f_c);
  printf("mini=%d ,mini1=%d & maxi=%d\n",mini,mini1,maxi);
  // Doc bang intensity duoi dang byte
  for(i=0;i<32;i++)
  fread(&intensity[i],sizeof(intensity[i]),1,f_c);

  fread(&offset,sizeof(offset),1,f_c);
  fread(&way,sizeof(way),1,f_c);
  fread(&n_grey,sizeof(n_grey),1,f_c);
  for (i = 0; i < n_grey; i++)
  {
  		p1[i]=fgetc(f_c);
  		//printf(" %d ",p1[i]);
  }
  

  unsigned long tempos;
  tempos=ftell(f_c);  // Save present position for return affer decompressing CP map
  // Read pro. table
 for(i=0;i<8;i++)
   	for (j = 0; j < 32; j++)
     		fread(&p_stored[i][j],sizeof(p_stored[i][j]),1,f_c);

  // Read len of codeword
  fread(&len,sizeof(len),1,f_c);

  // Read position of crosspoint map if exist
  fread(&position,sizeof(position),1,f_c);
  
  if (way==2)
  	{  readcp();
      findcp();   // Set up refer_plane for decompressing
   }
   cprintf ("Finish creation of refer planes \n\r");
   
   fseek(f_c,tempos,SEEK_SET);
   // Reread information
    // Read pro. table
   for(i=0;i<8;i++)
   	for (j = 0; j < 32; j++)
     		fread(&p_stored[i][j],sizeof(p_stored[i][j]),1,f_c);

  // Read len of codeword
  fread(&len,sizeof(len),1,f_c);

  // Read position of crosspoint map if exist
  fread(&position,sizeof(position),1,f_c);
  
}


void readinfo1(void)
{
  	char filename[MAXFILE+MAXEXT];
     char ext[10];
  	int i,j,k,m,bit;


     strcpy(filename,orig_name);
  	  sprintf(ext,".out2");
     strcat(filename,ext);

  if(( f_c = fopen(filename, "rb")) == NULL)
  {
    printf ("Cannot open coded  file for decompressing\n");
    printf ("Press anykey for END\n");
    getch();
    exit (1);
  }

  cprintf("Please wait while reading information\n\r");

 
  	fseek(f_c,0,SEEK_END);
  unsigned long p_sto1;
  p_sto1=ftell(f_c)-16;  // Save present position for return affer decompressing CP map
  fseek(f_c,p_sto1,SEEK_SET);
  fread(&p_sto1code,sizeof(p_sto1code),1,f_c);
  //printf("p_sto1code=%d\n",p_sto1code);
  fseek(f_c,p_sto1code,SEEK_SET);
  // Read pro. table
 for(i=0;i<8;i++)
 {
	for (j = 0; j < 32; j++)
	{
		fread(&p_stored1[i][j],sizeof(p_stored1[i][j]),1,f_c);
		//printf(" %d ",p_stored1[i][j]);
	}
     //printf("\n");		
 }
  // Read len1 of codeword
  fread(&len1,sizeof(len1),1,f_c);
   //printf("len1=%d\n",len1);
  // Read position of crosspoint map if exist
  // p_sto1com==size1[way]==size1[2]
  fread(&p_sto1com,sizeof(p_sto1com),1,f_c);

  ///////////////////////////////////////////////
   
   if ((maxi-mini)>256)
  {
   readcp1();
   findcp1();   // Set up refer_plane for decompressing
  }
   cprintf ("Finish creation of refer plane1 \n\r");
   
   fseek(f_c,p_sto1code,SEEK_SET);
   // Reread information
    // Read pro. table
  for(i=0;i<8;i++)
 {
	for (j = 0; j < 32; j++)
	{
		fread(&p_stored1[i][j],sizeof(p_stored1[i][j]),1,f_c);
		//printf(" %d ",p_stored1[i][j]);
	}
     //printf("\n");		
 }

  // Read len of codeword
  fread(&len1,sizeof(len1),1,f_c);

  // Read position of crosspoint map if exist
  fread(&p_sto1com,sizeof(p_sto1com),1,f_c);
  //printf("p_sto1com=%d\n",p_sto1com);
  
}


void readcp(void)     // Read data from coded file and decompress crosspoint map
{
	int i,j,bit,k,m;

   // Create pro. table
    for(i=0;i<8;i++)
   	for (j = 0; j < 32; j++)
   		p_stored[i][j].value=0;

   // Jump to location of cp map
   fseek(f_c,position,SEEK_SET);
   // Doc lai ban do crosspoint

   // Read pro. table
    for(i=3;i<7;i++)
    {
   	for (j = 0; j < 32; j++)
   	{
   		fread(&p_stored[i][j],sizeof(p_stored[i][j]),1,f_c);
   		//printf(" %d ",p_stored[i][j]);
   	}
   	//printf("\n");
    }
   // Read len of codeword
   fread(&len,sizeof(len),1,f_c);
   // Decompress crosspoint map ( Use Gray Code and don't optimize crosspoint )
  // Result save in bit_plane[i] matrix
    for(bit_no=7;bit_no>=0;bit_no--)
  {  if(bit_no<7 && bit_no>=3)
     continue;
     else{
  for(i=0;i<hei;i++)
  for(j=0;j<wid;j++)
  *((char *)bit_plane[bit_no]+i*wid+j) =0;}
  }
  int b0;
  // store fre tables of 8 bit planes
 for(i=0;i<8;i++)
  	for (j = 0; j < 32; j++)
      f[i][j].value=p_stored[i][j].value;

  // Fre. table of the each  bit plane
 for(i=0;i<8;i++)
   for (j = 0; j < 32; j+=2)
     f[i][j+1].value += f[i][j].value;

  // Calculate w
  w=cal_w(f);

  // Decoding
  cprintf ("Begin decoding cross point map\n\r");
  unsigned long v;
  v=pow(2,w);
  unsigned long yy = 1;
  unsigned long xx=0;                                        // CALCULATE VALUE OF Y0=2^W
  int posin=-1,valin=0;
  unsigned long counter=0;


  for(bit_no=7;bit_no>=0;bit_no--)
  {  if(bit_no==7||bit_no<3)
     continue;
    row=col=0;
    while(row<hei)
    {
      b0=1;
      int symbol;
      symbol = findsymbol(row,col, b0);
      u=f[bit_no][symbol].value;
      decode(symbol,f[bit_no],xx,yy,v,u,posin,valin,bit_no,f_c,counter);
      *((char *)bit_plane[bit_no]+row*wid+col)=symbol-findsymbol(row,col,0);

      col++;
         if(col==wid)
         {col=0; row++;
         }
     }
     printf("Decode bit plane %i passed\n",bit_no);

}
     cprintf("End decoding crosspoint map\n\r");

   unsigned long a;
   fseek(f_c,0,SEEK_END);
   if (maxi-mini<256)
   a=ftell(f_c)-4;
   else if (maxi-mini>=256)
   a=ftell(f_c)-8;
   //printf("a=%d\n",a);
   fseek(f_c,a,SEEK_SET);
   fread(&al,sizeof(al),1,f_c);
   //printf("al=%d\n",al);
   fseek(f_c,al,SEEK_SET);
    
   for(i=0;i<8;i++)
   {
   fread(&alpha[i],sizeof(alpha[i]),1,f_c);
   //printf("%f\n",alpha[i]);
   }
   // Write beta
   for(i=0;i<8;i++)
   {
   fread(&beta[i],sizeof(beta[i]),1,f_c);
   //printf("%f\n",beta[i]);
   }
   fread(&lencp,sizeof(lencp),1,f_c);
   //printf("lencp=%d\n",lencp);
   temp = (int *) calloc (lencp/8+1, sizeof (int));
   for(i=0;i<lencp/8+1;i++)
   {
   temp[i]= fgetc(f_c);
   //printf(" %d ",temp[i]);
   }
}

void readcp1(void)     // Read data from coded file and decompress crosspoint map
{
	int i,j,bit,k,m;

   // Create pro. table
    for(i=0;i<8;i++)
   	for (j = 0; j < 32; j++)
   		p_stored1[i][j].value=0;

   // Jump to location of cp map
   fseek(f_c,p_sto1com,SEEK_SET);
  //printf("p_sto1com=%d\n",p_sto1com);
   // Doc lai ban do crosspoint
  
   // Read pro. table
    for(i=0;i<=1;i++)
    {
    	for (j = 0; j < 32; j++)
    	{
	    	fread(&p_stored1[i][j],sizeof(p_stored1[i][j]),1,f_c);
	    	//printf(" %d  ",p_stored[i][j]);
	    }
   		//printf("\n");
    }
   	
   // Read len of codeword
   fread(&len1,sizeof(len1),1,f_c);
   // Decompress crosspoint map ( Use Gray Code and don't optimize crosspoint )
  // Result save in bit_plane[i] matrix
    for(bit_no=7;bit_no>=0;bit_no--)
  {  if(bit_no>1)
     continue;
     else{
  for(i=0;i<hei;i++)
  for(j=0;j<wid;j++)
  *((char *)bit_plane1[bit_no]+i*wid+j) =0;}
  }
  int b0;
  // store fre tables of 8 bit planes
 for(i=0;i<8;i++)
  	for (j = 0; j < 32; j++)
      f[i][j].value=p_stored1[i][j].value;

  // Fre. table of the each  bit plane
 for(i=0;i<8;i++)
   for (j = 0; j < 32; j+=2)
     f[i][j+1].value += f[i][j].value;

  // Calculate w
  w=cal_w(f);
 //printf("w=%d\n",w);
  // Decoding
  cprintf ("Begin decoding cross point map\n\r");
  unsigned long v;
  v=pow(2,w);
  unsigned long yy = 1;
  unsigned long xx=0;                                        // CALCULATE VALUE OF Y0=2^W
  int posin=-1,valin=0;
  unsigned long counter=0;


  for(bit_no=7;bit_no>=0;bit_no--)
  {  if(bit_no>1)
     continue;
    row=col=0;
    while(row<hei)
    {
      b0=1;
      int symbol;
      symbol = findsymbol1(row,col, b0);
      u=f[bit_no][symbol].value;
      decode1(symbol,f[bit_no],xx,yy,v,u,posin,valin,bit_no,f_c,counter);
      *((char *)bit_plane1[bit_no]+row*wid+col)=symbol-findsymbol1(row,col,0);

      col++;
         if(col==wid)
         {col=0; row++;
         }
     }
     printf("Decode bit plane %i passed\n",bit_no+8);

}
     cprintf("End decoding crosspoint map\n\r");

   
   unsigned long a1;
   fseek(f_c,0,SEEK_END);
   a1=ftell(f_c)-4;
   fseek(f_c,a1,SEEK_SET);
   fread(&al1,sizeof(al1),1,f_c);
   fseek(f_c,al1,SEEK_SET);

   for(i=0;i<8;i++)
   	fread(&alpha1[i],sizeof(alpha1[i]),1,f_c);
   	
   
   // Write beta
   for(i=0;i<8;i++)
   	fread(&beta1[i],sizeof(beta1[i]),1,f_c);

   fread(&lencp1,sizeof(lencp1),1,f_c);
   //	printf("lencp1=%d\n",lencp1);
   temp1 = (int *) calloc (lencp1/8+1, sizeof (int));
   for(i=0;i<lencp1/8+1;i++)
   {
   	fread(&temp1[i],sizeof(temp1[i]),1,f_c);
   	//printf(" %d ",temp1[i]);
   }
   
}

void findcp(void)  // Create refer_plane from markded pict[i][j]
{ int i,j;
  char *cpAR;
  cpAR = (char *) calloc (lencp, sizeof (char));
  for(i=0;i<lencp/8+1;i++)
  for(bit_no=7;bit_no>=0;bit_no--)
  { cpAR[i*8+7-bit_no]=bit(bit_no,temp[i]);
    if(cpAR[i*8+7-bit_no]==0)
    cpAR[i*8+7-bit_no]+=2;
  }
  for(bit_no=0;bit_no<7;bit_no++)
  for(i=0;i<hei;i++)
  for(j=0;j<wid;j++)
  *((char *)refer_plane[bit_no]+i*wid+j)=0;
 
  // Tao tao lai ban do crosspoint tu ban do danh dau trong bit_plane
 int vung;
 vung=0;
 for(bit_no = 6; bit_no>=3;bit_no--)
  for(i=0;i<hei;i++)
   for(j=0;j<wid;j++)
    {
     if(*((char *)bit_plane[bit_no] +i*wid+j)==0)
     continue;
     if(*((char *)bit_plane[bit_no] +i*wid+j)==1)
     {
       do
       {
       *((char *)refer_plane[6-bit_no] +i*wid+j)=cpAR[vung];
       j++;
       }
       while(*((char *)bit_plane[bit_no] +i*wid+j)!=1);
       *((char *)refer_plane[6-bit_no] +i*wid+j)=cpAR[vung];
       vung++;
     }
    }
// Reset value of bit_plane
   for(bit_no=0;bit_no<8;bit_no++)
  for(i=0;i<hei;i++)
  for(j=0;j<wid;j++)
  *((char *)bit_plane[bit_no]+i*wid+j)=0;
  free(cpAR);
}

void findcp1(void)  // Create refer_plane from markded pict[i][j]
{ int i,j;
  char *cpAR;
  cpAR = (char *) calloc (lencp1, sizeof (char));
  for(i=0;i<lencp1/8+1;i++)
  for(bit_no=7;bit_no>=0;bit_no--)
  { cpAR[i*8+7-bit_no]=bit(bit_no,temp1[i]);
    if(cpAR[i*8+7-bit_no]==0)
    cpAR[i*8+7-bit_no]+=2;
  }
  for(bit_no=0;bit_no<7;bit_no++)
  for(i=0;i<hei;i++)
  for(j=0;j<wid;j++)
  *((char *)refer_plane1[bit_no]+i*wid+j)=0;
  
  // Tao tao lai ban do crosspoint tu ban do danh dau trong bit_plane
 int vung;
 vung=0;
 for(bit_no = 7; bit_no>=1;bit_no--)
  for(i=0;i<hei;i++)
   for(j=0;j<wid;j++)
    {
     if(*((char *)bit_plane1[bit_no] +i*wid+j)==0)
     continue;
     if(*((char *)bit_plane1[bit_no] +i*wid+j)==1)
     {
       do
       {
       *((char *)refer_plane1[7-bit_no] +i*wid+j)=cpAR[vung];
       j++;
       }
       while(*((char *)bit_plane1[bit_no] +i*wid+j)!=1);
       *((char *)refer_plane1[7-bit_no] +i*wid+j)=cpAR[vung];
       vung++;
     }
    }
// Reset value of bit_plane
   for(bit_no=0;bit_no<8;bit_no++)
  for(i=0;i<hei;i++)
  for(j=0;j<wid;j++)
  *((char *)bit_plane1[bit_no]+i*wid+j)=0;
  free(cpAR);

}
