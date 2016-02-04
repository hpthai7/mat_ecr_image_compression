#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <dir.h>
#include <dos.h>
#include <ctype.h>

#define log2(a) ((log(a))/(log(2)))
#define WID 1024
#define HEI 1024

struct tagRGBQUAD
	{
		unsigned char	bBlue;
		unsigned char	bGreen;
		unsigned char	bRed;
		unsigned char	bReserved;
	} RGB[256];

struct tagFRE
   { unsigned value : 24;
   };

// Define functions
char bit(int index,unsigned short x); // convert DEC->BIN
char bit_last(int index,unsigned short x); // convert DEC->BIN

void create_matrix_image (void);
void create_fwt_matrix (void);
void sta (void);

void make_bp(int Graycode);
void make_bp1(int Graycode);

void cal_fre(int i);
void cal_fre(int i);

void encode(int i, struct tagFRE f[],
				unsigned long &x,unsigned long &y,int &r,int &j,
            unsigned long v,unsigned long u,
            int &posout,int &valout,FILE *filename,unsigned long &m);
void output(int bit,int &vitri,int &byte,FILE *codefile,unsigned long &m);

void encoder (int way);
void encoder1 (int way);

void findcp(void);
void findcp1(void);

void compressCP(void);
void compressCP1(void);

void codefile(int way);
void codefile1(int way);

int value(int b4,int b3, int b2, int b1, int b0);
int cal_w (struct tagFRE f[][32]);
int findsymbol(int i,int j,int b0);
int findsymbol1(int i,int j,int b0);
void lowerIt(char str[]);
// Define global variables
unsigned char paletteRev;
unsigned char orpict[HEI][WID];  /* matrix content data of bmp */
short pict[HEI][WID];
short pict1[HEI][WID];
float B[HEI][WID];
float C[HEI][WID];
float E[HEI][WID];

unsigned short n_grey;          /* Number of real gray level in bmp file */
unsigned short p1[1024];         /* Store pseudo gray level table */

char orig_name[MAXFILE];       /* original filename */
char **bit_plane[8];
char **bit_plane1[8];
char **bit_plane_CP[8];
char **bit_plane_CP1[8];
char **refer_plane[7];
char **refer_plane1[7];

char h,m,s;           			 /* Store time to run program for each way */
int timer[3];
int wid,hei;                   /* Width and Heigth of bmp file */
int w;								 /* W value use for C.B.J algorithm */
int bit_no,row,col;
unsigned long fi, fim1, u;     /* f[i],f[i-1],u */
struct tagFRE f[8][32],p_stored[8][32],p_stored1[8][32];
unsigned long len[4],len1[4];          /* Store len codeword of each bitplane(3 ways) */
unsigned long offset;          /* Save offset of data image for compare later */
unsigned long size[4],size1[4];
FILE *f_c[4],*f_c1[4];                  /* File temp to create codeword */
int intensity[256];
float alpha[8],beta[8],alpha1[8],beta1[8];
int lencp,lencp1;
short mini, maxi,mini1;
unsigned long p_sto1code;
unsigned long p_stocode;
unsigned long p_stocom;
unsigned long p_sto1com;
unsigned long al;
unsigned long al1;
short b;
////////////////////////////////////////////////////////////////////////////////

void main()
{  int way;
   int level;
   int i,j,k,m,bit;
   struct time t;

	do
	{  clrscr();
   	textcolor(YELLOW);
		cprintf ("SuperCompressor program : compresses an image (default for BMP file)\n\r");
		cprintf ("Cai tien chuong tinh nen anh\n\r");
		cprintf ("GVHD: TS. Dang Thanh Tin\n\r");
		cprintf ("SVTH: Tran Dai Viet - Ho Phu Thai\n\r");
		
   	//Create the matrix from the original image file
  		create_matrix_image ();
  		for (i=0;i<7;i++)
             {
               for (j=0;j<7;j++)
               {
                 printf(" %d ",orpict[i][j]);
               }
               printf("\n");
             }
   	// change original image to pseudo gray value image
  		sta();
  		getch();
       for (i=0;i<hei;i++)
         for (j=0;j<wid;j++)
           pict[i][j]=orpict[i][j];
   	// Find and map Crosspoint
     	for (i = 0; i < 7; i++)
  	 		{ refer_plane[i] = (char **) calloc (hei*wid, sizeof (char));
    		  if (refer_plane[i] == (char **)NULL)
      			{  printf ("Cannot allocate dynamic array for reference planes !");
               	getch();
						exit (1);
               }
  	 		}

    	// Bit_plane decomposition
    	for (i = 0; i < 8; i++)
	  		{ 	bit_plane[i] = (char **) calloc (hei*wid, sizeof (char));
    	 		if (bit_plane[i] == (char **)NULL)
    	 			{  printf ("Cannot allocate dynamic array for bit planes !");
                  getch();
      				exit (1);
    				}
  	  		}

      // Initialize value for refer_plane
     	for(bit_no=0;bit_no<7;bit_no++)
     		for(k=0;k<hei;k++)
	  			for(m=0;m<wid;m++)
     				*((char *)refer_plane[bit_no]+k*wid+m)=0;
     				
///////////////////////////////////////////////////////////////
// Find and map Crosspoint
     	for (i = 0; i < 7; i++)
  	 		{ refer_plane1[i] = (char **) calloc (hei*wid, sizeof (char));
    		  if (refer_plane1[i] == (char **)NULL)
      			{  printf ("Cannot allocate dynamic array for reference planes !");
               	getch();
						exit (1);
               }
  	 		}

    	// Bit_plane decomposition
    	for (i = 0; i < 8; i++)
	  		{ 	bit_plane1[i] = (char **) calloc (hei*wid, sizeof (char));
    	 		if (bit_plane1[i] == (char **)NULL)
    	 			{  printf ("Cannot allocate dynamic array for bit planes !");
                  getch();
      				exit (1);
    				}
  	  		}

      // Initialize value for refer_plane
     	for(bit_no=0;bit_no<7;bit_no++)
     		for(k=0;k<hei;k++)
	  			for(m=0;m<wid;m++)
     				*((char *)refer_plane1[bit_no]+k*wid+m)=0;
     				
    	i=0;
     	switch(i)
     	{
     					 // Don't use GrayCode and Crosspoint
     		case 0: { gettime(&t);   /* Get system time to compare */
               	 clrscr();
               	 h=t.ti_hour;
               	 m=t.ti_min;
                	 s=t.ti_sec;
     					 cprintf("Compress without Graycode\n\r");
	     				 // Create biplane without Graycode
   	             make_bp(0);
      	          // Encode bitplanes
         	       encoder(0);
            	    // Write to .out file
   					 codefile(0);
                	 gettime(&t);
                	 timer[0]=(t.ti_hour-h)*3600+(t.ti_min-m)*60+(t.ti_sec-s);
               // 	 getch();
                 }

                 	// Use Graycode
     		case 1: { gettime(&t);
               	 clrscr();
               	 h=t.ti_hour;
	                m=t.ti_min;
    	             s=t.ti_sec;
     					 cprintf("Compress with Graycode\n\r");
     		 			 // Create biplane with Graycode
            	    make_bp(1);
               	 // Encode bitplanes
                	 encoder(1);
                 	 // Write to .out file
   				    codefile(1);
                   gettime(&t);
                   timer[1]=(t.ti_hour-h)*3600+(t.ti_min-m)*60+(t.ti_sec-s);
                 //  getch();
                 }

                 // Use Graycode and optimize probability
     		case 2: { gettime(&t);
                	 clrscr();
 	                h=t.ti_hour;
   	             m=t.ti_min;
      	          s=t.ti_sec;
 					 cprintf("Compress with Graycode and Optimize probability\n\r");
                     create_fwt_matrix();
                     /*level=0;
                     for (i=0;i<hei;i++)
             {
               for (j=0;j<wid;j++)
               { if (pict[i][j]>=256)
                  level=level+1;
               }
              }
              //printf("level=%d\n",level);
             for (i=0;i<7;i++)
             {
               for (j=0;j<7;j++)
               {
                 printf(" %d ",pict[i][j]);
               }
               printf("\n");
             }*/
                    for (i=0;i<hei;i++)
                      for (j=0;j<wid;j++)
                         pict1[i][j]=pict[i][j];
					 // Create biplane with Graycode
               	     make_bp(1);
                	 // Find crosspoint
                	 findcp();
                	 printf("Finish create crosspoint A&R map\n");
                	 //Encode bitplanes
                	 encoder(2);
                   // Write to .out file
   				 	 codefile(2);
                	 // compress crosspoint map
                	 printf("Compressing crosspoint map for .out file\n");
	                compressCP();
          if ((maxi-mini)<256)
          {
          	gettime(&t);
           timer[2]=(t.ti_hour-h)*3600+(t.ti_min-m)*60+((int)t.ti_sec-(int)s);
          	size1[2]=size[2];
          	break;
          }
          else
          {
          	//gettime(&t);
             clrscr();
 	         h=t.ti_hour;
             m=t.ti_min;
             s=t.ti_sec;
             for (i=0;i<hei;i++)
               for (j=0;j<wid;j++)
                 pict[i][j]=pict1[i][j];
             cprintf("Compress with Graycode and Optimize probability\n\r");
                     // Create biplane with Graycode
               	     make_bp1(1);
                	 // Find crosspoint
                	 findcp1();
                	 printf("Finish create crosspoint A&R map\n");
                	 //Encode bitplanes
                	 encoder1(2);
                   // Write to .out file
   				 	 codefile1(2);

                	 // compress crosspoint map
                	 printf("Compressing crosspoint map for .out file\n");
	                compressCP1();
   	             gettime(&t);
      	          timer[2]=(t.ti_hour-h)*3600+(t.ti_min-m)*60+((int)t.ti_sec-(int)s);    
          }
                  }
      }

   for (i = 0; i < 8; i++)
    free(bit_plane[i]);
   for (i = 0; i < 7; i++)
    free(refer_plane[i]);
   for (i = 0; i < 8; i++)
    free(bit_plane1[i]);
   for (i = 0; i < 7; i++)
    free(refer_plane1[i]); 
   fcloseall();
   clrscr();
   printf("Enter to view result :\n");
   getch();
   textcolor(YELLOW);
   // Printf result to compare
   cprintf("Compare   |CR without GC | CR with GC | CR with GC and OP\n\r");
   printf("%s.dta      %8.5lf       %8.5lf     %8.5lf\n",orig_name,
   1./(len[0])*(double)(hei*wid)*8,1./(len[1])*(double)(hei*wid)*8.,1./(len[2])*(double)(hei*wid)*8.);
   printf("==========================================================\n\r");
   printf("%s.out      %8.5lf       %8.5lf     %8.5lf\n",orig_name,
   size[3]/(double)size[0],size[3]/(double)size[1],size[3]/(double)size1[2]);
   printf("==========================================================\n\r");
   printf("Time      %8ds       %8ds     %8ds \n",timer[0],timer[1],timer[2]);
   cprintf("\n\rEnter to continue, another key to exit");
}
while(getch()==13);
}




////////////////////////////////////////////////////////////////////////////////
// Implementation functions
char bit(int index,unsigned short x)
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

char bit_last(int index,unsigned short x)
{ int mask;
  switch(index)
  {
  case 0 : 	mask=0x0100;
            break;
  case 1	:	mask=0x0200;
            break;
  case 2	:  mask=0x0400;
            break;
  case 3	:  mask=0x0800;
            break;
  case 4	:  mask=0x1000;
            break;
  case 5 :  mask=0x2000;
            break;
  case 6	:  mask=0x4000;
            break;
  case 7	:  mask=0x8000;
            break;
  };
  return (char)((x & mask)? 1:0);
}

void create_matrix_image (void)
{
  // Create the matrix from an image file
  FILE *f;
  char fn[MAXFILE];
  char drive[MAXDRIVE],              /* needed for fnsplit()   */
       dir[MAXDIR],                  /* needed for fnsplit()   */
       filename[MAXFILE],            /* 8 chars, orig name     */
       ext[MAXEXT];                  /* 3 chars, orig ext      */
  unsigned char temp;
  int i, j;
  short a;
  unsigned long tempoff;
  int color;
  flushall();
  printf ("Please enter an image filename (ENTER for ZELDA.BMP (256x256x256)): ");
  gets (fn);

  if (!strcmp (fn, ""))
  {	strcpy (fn, "zelda.bmp");
	b=4;	}
  else
  {	lowerIt(fn);
	if (!strcmp(fn,"zelda.bmp")||!strcmp(fn,"couple.bmp")||!strcmp(fn,"bieutinh.bmp")||
		!strcmp(fn,"blena.bmp")||!strcmp(fn,"cube.bmp"))
		b=4;
	else if (!strcmp(fn,"moon.bmp")||!strcmp(fn,"chest.bmp")||!strcmp(fn,"joint.bmp")||
		!strcmp(fn,"clown.bmp")||!strcmp(fn,"right.bmp")||!strcmp(fn,"mountain.bmp"))
		b=3;
	else if (!strcmp(fn,"mandrill.bmp"))
		b=14;
	else if (!strcmp(fn,"girl.bmp")||!strcmp(fn,"fruit1.bmp"))
		b=2;
	else if (!strcmp(fn,"lena.bmp")||!strcmp(fn,"disp8.bmp")||!strcmp(fn,"scott.bmp")||
		!strcmp(fn,"oranges.bmp"))
		b=1;
	else if (!strcmp(fn,"stan.bmp")||!strcmp(fn,"fruit.bmp"))
		b=5;
	else
	{	printf("Enter an arbitrary integer from the set {1,2,3,...,20}\n");
		printf("\t(If decompression fails, try a different value).\n");
		printf("a = ");
		scanf("%d",&a);
		b=a;	}
  }

  // Get filename and store orig_name for output file code
  fnsplit(fn, drive, dir, filename, ext);    /* get filename    */
  strcpy(orig_name,filename);

  if(( f = fopen(fn, "rb")) == NULL)
  {
    printf ("Cannot open BMP image file for compressing\n");
    printf ("Press anykey for END\n");
    getch();
    exit (1);
  }

  // Read the file
  textcolor (YELLOW);
  cprintf ("\n\rPlease wait while reading the image file\n\r");
  // Read offset of bmp file
  fseek(f,10,0);
  fread(&offset,sizeof(long),1,f);
  // Read the image dimension
  fseek (f, 18, 0);
  fread (&wid, sizeof(short), 1, f);
  fseek (f, 22, 0);
  fread (&hei, sizeof(short), 1, f);

  printf ("Hei x wid = %d x %d \n", hei, wid);

  // Read and compare color palette with standard color pallete;
  fseek(f,54,0);
  color= (offset-54)/4;
  for( i=0;i<color;i++)
  for(i=0;i<color;i++)
  fread(&RGB[i],sizeof(RGB[i]),1,f);
  // Compare two color pallete if 2 entry have same color index =>intensity =1
  for(i=0;i<256;i++)
  { intensity[i]=0;
    for(j=0;j<color;j++)
    if (RGB[j].bBlue==i && RGB[j].bBlue>=j)
    { intensity[i]=1;
      break;
    }
  }
      paletteRev = 0;
	if(RGB[0].bBlue==255 && RGB[255].bBlue==0)
		paletteRev = 1;
  // Read raw data from BMP file into orpict[hei][wid]: fixed
  fseek (f, offset, SEEK_SET);
  for (i = 0; i < hei; i++)
    fread (orpict[i], wid, 1, f);
  // Read size of file bmp
  fseek(f,0,SEEK_END);
  size[3]=ftell(f);
  // Reverse data for real data, because of the inversive storing in BMP file
  //    the first line of data is in the bottom of the image, ...
  for (i = 0; i < hei/2; i++)
    for (j = 0; j < wid; j++)
    {
      temp = orpict[i][j];
      orpict[i][j]  = orpict[hei-1-i][j];
      orpict[hei-1-i][j] = temp;
    }

  fclose (f);
  printf ("Finish creation of picture matrix\n");
}

void create_fwt_matrix(void) // Forward transform
{
	int i,j;
	// S and D
	for (i=0;i<hei;i++)
	for (j=0;j<wid;j++)
		E[i][j] = (float)orpict[i][j];
	for (i=0;i<hei;i++)
	{
		for (j=1;j<wid/2;j++)
		{
		B[i][j+wid/2] = E[i][2*j] - floor((E[i][2*j-1]+E[i][2*j+1]+1)/2);
		B[i][wid/2] = E[i][0] - floor((float)E[i][1]+1/2);
		}
	}

	for (i=0;i<hei;i++)
	{
		for (j=1;j<wid/2;j++)
		{
		B[i][j-1] = E[i][2*j-1] + floor((B[i][j+wid/2-1]+B[i][j+wid/2]+2)/4);
		B[i][wid/2-1] = E[i][wid-1] + floor((B[i][wid-1]+1)/2);
		}
	}

	// S' and three Ds
	for (j=0;j<wid;j++)
	{
		for (i=1;i<hei/2;i++)
		{
		C[i+hei/2][j] = B[2*i][j] - floor((B[2*i-1][j]+B[2*i+1][j]+1)/2);
		C[hei/2][j] = B[0][j] - floor((float)B[1][j]+1/2);
		}
	}

	for (j=0;j<wid;j++)
	{
		for (i=1;i<hei/2;i++)
		{
		C[i-1][j] = B[2*i-1][j] + floor((C[i+hei/2-1][j]+C[i+hei/2][j]+2)/4);
		C[hei/2-1][j] = B[hei-1][j] + floor((C[hei-1][j]+1)/2);
		}
	}

	for (i=0;i<hei;i++)
	{
	for (j=0;j<wid;j++)
	{
	   	pict[i][j] = (short)C[i][j];
	}
	}

	mini = 0;maxi = 0;mini1 = 0;
	for (i=0;i<hei;i++)
	{
	for (j=0;j<wid;j++)
	{
		if (pict[i][j] < mini)
			mini = pict[i][j];
		if (pict[i][j] > maxi)
			maxi = pict[i][j];
	}
	}
	for (i=0;i<hei/2;i++)
	{
	for (j=0;j<wid/2;j++)
	{
		if (pict[i][j] < mini1)
			mini1 = pict[i][j];
	}
	}
	printf("mini = %d ,mini1 = %d and maxi = %d\n",mini,mini1,maxi);
	if (mini>-205)
	{
		for (i=0;i<hei;i++)
		for (j=0;j<wid;j++)
		pict[i][j] = pict[i][j] - mini+b;
	}	
	else 
	{
	for (i=0;i<hei;i++)
	 {
		for (j=0;j<wid;j++)
	   {
		if ((i<hei/2)&&(j<wid/2))
		pict[i][j] = pict[i][j] - mini1+b;
		else
		pict[i][j] = pict[i][j] - mini+b;
	   }	
	 }
	}
}


void sta (void)
{
  short i,j,k;
  unsigned short p[1024];

  for (i = 0; i < 1024; i++)
   { p[i] = 0;
     p1[i]=0;
   }

  for (i = 0; i < hei; i++)
    for (j = 0; j < wid; j++)
      p[orpict[i][j]] ++;

  for (i = 0, j = 0; i < 1024; i++)
    if (p[i])
    {
      p1[j] = (unsigned short)i;
      j++;
    }

  n_grey = j;
  printf ("Number of gray level : n_Grey = %d \n", n_grey);

  for (i = 0; i < hei; i++)
    for (j = 0; j < wid; j++)
      for (k = 0; k < n_grey; k++)
       if (orpict[i][j] == p1[k])
        { orpict[i][j] = (unsigned short)k;
          break;
        }
}

void make_bp(int Graycode)
{
  int i, j;
  char gray_bit;

  // Read data from the matrix and change to 8 bit planes, 8 bit files
  // with Gray code if Graycode=1
  for (bit_no = 7; bit_no >= 0; bit_no--)
  	 for (i = 0; i < hei; i++)
  		for (j = 0; j < wid; j++)
      {
         gray_bit = bit(bit_no,pict[i][j]);
      	if(Graycode==1 && bit_no<7)
        		gray_bit = bit(bit_no,pict[i][j]) ^ bit(bit_no+1,pict[i][j]);

         *((char *)bit_plane[bit_no]+i*wid+j) = gray_bit;
      }

  printf ("Finish creation of 8 bit planes matrix\n");
}

void make_bp1(int Graycode)
{
  int i, j;
  char gray_bit;

  // Read data from the matrix and change to 8 bit planes, 8 bit files
  // with Gray code if Graycode=1
  for (bit_no = 7; bit_no >= 0; bit_no--)
  	 for (i = 0; i < hei; i++)
  		for (j = 0; j < wid; j++)
      {
         gray_bit = bit_last(bit_no,pict[i][j]);
      	if(Graycode==1 && bit_no<7)
        		gray_bit = bit_last(bit_no,pict[i][j]) ^ bit_last(bit_no+1,pict[i][j]);

         *((char *)bit_plane1[bit_no]+i*wid+j) = gray_bit;
      }

  printf ("Finish creation of 8 bit planes matrix\n");
}

int value(int b4,int b3, int b2, int b1, int b0)
{    return b4*16 + b3*8 + b2*4 + b1*2 + b0; }


int findsymbol(int i,int j,int b0)
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

int findsymbol1(int i,int j,int b0)
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

void encode(int i, struct tagFRE f[],
				unsigned long &x,unsigned long &y,	int &r,int &j,
            unsigned long v,unsigned long u,
            int &posout,int &valout,FILE *filename,unsigned long &m)

{
  int q;
  unsigned long t,z;

  if (i==-1)
    {
    	t=(unsigned long)(floor(x/v)+1);
    	if (t>2)
      	{	j=j+1;
      		q=1;
      		t-=2;
         }
    	else q=2;

    	m++;
    	output(j-1,posout,valout,filename,m);

    	while (r>0)
      	{  m++;
      		output(q-1,posout,valout,filename,m);
      		r-=1;
      	}

    	m++;
    	output(t-1,posout,valout,filename,m);

    	if (posout!=7)
      		fputc(valout,filename);
    }
  else
    {
    	z=(unsigned long)floor(y*((float)fim1/u)+ 0.5);
    	x+=z;
    	y=(unsigned long)floor(y*((float)fi/u)  + 0.5)-z;
    	div_t a;

    	while (y<v)
      	{
      		t=(unsigned long)floor(x/v)+1;
      		a=div(x,v);
      		x=a.rem*2;
      		y=y*2;

      		if (t==2) r+=1;
      		else
					{
						if (t>2)
	  						{  j+=1;
                     	q=1;
                        t-=2;
	  						}
		 				else q=2;

       				m++;
       				output(j-1,posout,valout,filename,m);
       				j=t;

       				while (r>0)
	  						{  m++;
	  							output(q-1,posout,valout,filename,m);
	  							r-=1;
	  						}
					}
    		}
    }
}

void output(int bit,int &vitri,int &byte,FILE *codedfile,unsigned long &m)
{

  if (m>=3)
    {
    	if (bit==1)
      	byte+=(int)pow(2,vitri);

    	vitri-=1;
    	if (vitri==-1)
        	{
      	putc(byte,codedfile);
      	vitri=7;
      	byte=0;
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

void encoder (int way)
{
  int i, j, temp;
  int b0;

  char datfile_name[MAXFILE+MAXEXT];
  char ext[10];

  for(i=0;i<8;i++)
  	for (j = 0; j < 32; j++)
  	  f[i][j].value = 0;

  // The 7th bit plane: probability
  for (bit_no = 7; bit_no >= 0; bit_no--)
  { if((bit_no==7||bit_no<3)&& way==3)
    continue;
  	 for (i = 0; i < hei; i++)
    for (j = 0; j < wid; j++)
    	{ b0 = *((char *)bit_plane[bit_no]+i*wid+j);
        temp = findsymbol (i,j,b0);
        f[bit_no][temp].value++;
      }
  }
  // store probability tables of 8 bit planes
 for(i=0;i<8;i++)
  	for (j = 0; j < 32; j++)
   	  p_stored[i][j].value = f[i][j].value;

  // Fre. table of the each  bit plane
 for(i=0;i<8;i++)
  for (j = 0; j < 32; j+=2)
     	  f[i][j+1].value += f[i][j].value;

  // Calculate w
  w=cal_w(f);
  printf("Value of W = %d\n",w);

  // CREATE FILE CONTENT CODEWORD
  strcpy(datfile_name,orig_name);
  if(way<3)
  {
  sprintf(ext,".dta%d",way);
  strcat(datfile_name,ext);
  }
  else
  sprintf(datfile_name,"refer.dta");

  if ( (f_c[way] = fopen (datfile_name, "wb")) == NULL )
  {
    printf ("Cannot create coding file");
    exit (1);
  }

  // Encoding
  cprintf ("Begin coding\n\r");
  unsigned long v;
  int rr,jj,posout,value;
  unsigned long xx,yy;

  len[way]=0;
  v=pow(2,w);
  yy = v;
  rr=0,
  jj=2,
  posout=7,
  value=0;                                      // EACH BITPLANE SAVE 0ST ROW AND 0ST COL
  xx=0;

  for (bit_no = 7; bit_no >= 0; bit_no--)      // ENCODE EACH BITPLANE FROM 7 TO 0
  	{
      if((bit_no>=7 ||bit_no<3)&& way==3)
      continue;
  		for (row = 0; row < hei; row++)                         // (hei -1) lines
      	for (col = 0; col < wid; col++)                       // (wid -1) matrix / line
      		{ // FIND SYMBOL VALUE
        			b0 = *((char *)bit_plane[bit_no]+row*wid+col);
        			i=findsymbol(row,col,b0);
        			cal_fre(i);
        			encode(i,f[bit_no],xx,yy,rr,jj,v,u,posout,value,f_c[way],len[way]);
      		}
      printf("Bitplane %d compress passed\n",bit_no);

   }
  encode(-1,f[bit_no],xx,yy,rr,jj,v,u,posout,value,f_c[way],len[way]);
  len[way]-=2;
  cprintf("Length of codeword is  %lu \n\r",len[way]);
  //////////////////////////////////////////////
  cprintf ("End coding\n\r");
  fclose(f_c[way]);
}

void encoder1 (int way)
{
  int i, j, temp;
  int b0;

  char datfile_name[MAXFILE+MAXEXT];
  char ext[10];

  for(i=0;i<8;i++)
  	for (j = 0; j < 32; j++)
  	  f[i][j].value = 0;

  // The 7th bit plane: probability
  for (bit_no = 7; bit_no >= 0; bit_no--)
  { if((bit_no>1)&& way==3)
    continue;
  	 for (i = 0; i < hei; i++)
    for (j = 0; j < wid; j++)
    	{ b0 = *((char *)bit_plane1[bit_no]+i*wid+j);
        temp = findsymbol1 (i,j,b0);
        f[bit_no][temp].value++;
      }
  }
  // store probability tables of 8 bit planes
 for(i=0;i<8;i++)
  	for (j = 0; j < 32; j++)
   	  p_stored1[i][j].value = f[i][j].value;

  // Fre. table of the each  bit plane
 for(i=0;i<8;i++)
  for (j = 0; j < 32; j+=2)
     	  f[i][j+1].value += f[i][j].value;

  // Calculate w
  w=cal_w(f);
  printf("Value of W = %d\n",w);

  // CREATE FILE CONTENT CODEWORD
  strcpy(datfile_name,orig_name);
  if(way<3)
  {
  sprintf(ext,".dta%d",way);
  strcat(datfile_name,ext);
  }
  else
  sprintf(datfile_name,"refer.dta");

  if ( (f_c1[way] = fopen (datfile_name, "wb")) == NULL )
  {
    printf ("Cannot create coding file");
    exit (1);
  }

  // Encoding
  cprintf ("Begin coding\n\r");
  unsigned long v;
  int rr,jj,posout,value;
  unsigned long xx,yy;

  len1[way]=0;
  v=pow(2,w);
  yy = v;
  rr=0,
  jj=2,
  posout=7,
  value=0;                                      // EACH BITPLANE SAVE 0ST ROW AND 0ST COL
  xx=0;

  for (bit_no = 7; bit_no >= 0; bit_no--)      // ENCODE EACH BITPLANE FROM 7 TO 0
  	{
      if((bit_no>1)&& way==3)
      continue;
  		for (row = 0; row < hei; row++)                         // (hei -1) lines
      	for (col = 0; col < wid; col++)                       // (wid -1) matrix / line
      		{ // FIND SYMBOL VALUE
        			b0 = *((char *)bit_plane1[bit_no]+row*wid+col);
        			i=findsymbol1(row,col,b0);
        			cal_fre1(i);
        			encode(i,f[bit_no],xx,yy,rr,jj,v,u,posout,value,f_c1[way],len1[way]);
      		}
      printf("Bitplane %d compress passed\n",bit_no+8);

   }
  encode(-1,f[bit_no],xx,yy,rr,jj,v,u,posout,value,f_c1[way],len1[way]);
  len1[way]-=2;
  cprintf("Length of codeword is  %lu \n\r",len1[way]);
  //////////////////////////////////////////////
  cprintf ("End coding\n\r");
  fclose(f_c1[way]);
}

void codefile(int way)
{ FILE *out[3];
  FILE *refer[8];
  char ext[10];
  int i,j,c;
  char outfile_name[MAXFILE+MAXEXT];
  char datfile_name[MAXFILE+MAXEXT];

  strcpy(outfile_name,orig_name);
  sprintf(ext,".out%d",way);
  strcat(outfile_name,ext);

  if ( (out[way] = fopen (outfile_name, "wb")) == NULL )
  {
    printf ("Cannot create out coding file");
    getch();
    exit (1);
  }

  // Open .dat file to write in .out file
   strcpy(datfile_name,orig_name);
   sprintf(ext,".dta%d",way);
   strcat(datfile_name,ext);
   if ( (f_c[way] = fopen (datfile_name, "rb")) == NULL )
  	{
    printf ("Cannot open dat coding file");
    exit (1);
  	}

  // Write header of file
  rewind(out[way]);
  fwrite(&wid,sizeof(wid),1,out[way]);
  fwrite(&hei,sizeof(hei),1,out[way]);
  fwrite(&paletteRev,sizeof(paletteRev),1,out[way]);
  printf("paletteRev = %d\n",paletteRev);
  fwrite(&mini,sizeof(mini),1,out[way]);
  fwrite(&mini1,sizeof(mini1),1,out[way]);
  fwrite(&maxi,sizeof(maxi),1,out[way]);

  // Write intensity of color pallete
  for(i=0;i<256;i+=8)
  { char tempv=0;
    for(j=0;j<8;j++)
    tempv += intensity[i+j]*pow(2,j);
    // Write to file
    fwrite(&tempv,sizeof(tempv),1,out[way]);
  }
  fwrite(&offset,sizeof(offset),1,out[way]);
  fwrite(&way,sizeof(way),1,out[way]);
  fwrite(&n_grey,sizeof(n_grey),1,out[way]);
  for (i = 0; i < n_grey; i++)
  {
  		fputc(p1[i],out[way]);
  		//printf(" %d ",p1[i]);
  }
//getch();
  // write pro. table
for(i=0;i<8;i++)
     for (j = 0; j < 32; j++)
     	  fwrite(&p_stored[i][j],sizeof(p_stored[i][j]),1,out[way]);

  // write len of codeword
  fwrite(&len[way],sizeof(len[way]),1,out[way]);

  // Write tempoffset of dat file
  unsigned long tempoffset;
  tempoffset=ftell(out[way]);
  fwrite(&len[way],sizeof(len[way]),1,out[way]);
  // Write data to file
  while((c=fgetc(f_c[way]))!=EOF)
   { fputc(c,out[way]); }

  fseek(out[way],0,SEEK_END);
  size[way]=ftell(out[way]);
  // Rewrite offset data of file
  fseek(out[way],tempoffset,SEEK_SET);
  fwrite(&size[way],sizeof(size[way]),1,out[way]);

  fcloseall();
}

void codefile1(int way)
{ FILE *out[3];
  FILE *refer[8];
  char ext[10];
  int i,j,c;
  char outfile_name[MAXFILE+MAXEXT];
  char datfile_name[MAXFILE+MAXEXT];

  strcpy(outfile_name,orig_name);
  sprintf(ext,".out%d",way);
  strcat(outfile_name,ext);

  if ( (out[way] = fopen (outfile_name, "rb+")) == NULL )
  {
    printf ("Cannot create out coding file");
    getch();
    exit (1);
  }

  // Open .dat file to write in .out file
   strcpy(datfile_name,orig_name);
   sprintf(ext,".dta%d",way);
   strcat(datfile_name,ext);
   if ( (f_c1[way] = fopen (datfile_name, "rb")) == NULL )
  	{
    printf ("Cannot open dat coding file");
    exit (1);
  	}

 fseek(out[way],0,SEEK_END);
 p_sto1code=ftell(out[way]);

  // write pro. table
for(i=0;i<8;i++)
{
  for (j = 0; j < 32; j++)
  {
  	fwrite(&p_stored1[i][j],sizeof(p_stored1[i][j]),1,out[way]);
  	//printf(" %d ",p_stored1[i][j]);
  }
    //printf("\n");
}
     

  // write len of codeword
  fwrite(&len1[way],sizeof(len1[way]),1,out[way]);
printf("len1=%d\n",len1[way]);
  // Write tempoffset of dat file
  unsigned long tempoffset1;
  tempoffset1=ftell(out[way]);
  fwrite(&tempoffset1,sizeof(tempoffset1),1,out[way]);
  // Write data to file
  while((c=fgetc(f_c1[way]))!=EOF)
   { fputc(c,out[way]); }

  fseek(out[way],0,SEEK_END);
  size1[way]=ftell(out[way]);
  // Rewrite offset data of file
  fseek(out[way],tempoffset1,SEEK_SET);
  fwrite(&size1[way],sizeof(size1[way]),1,out[way]);

  fcloseall();
}

void findcp(void)
{
  int i,j,first,last;
  lencp=2;
  int posout=7;
  int valueout=0;                                      // EACH BITPLANE SAVE 0ST ROW AND 0ST COL
  FILE *cp;
  cp=fopen("temp.ref","wb");
 // Gan gia tri khoi tao cho alpha va beta
 for(bit_no = 0; bit_no < 8; bit_no++ )
 	alpha[bit_no]= beta[bit_no] = 0;

 // Tao ban do dung de nen vung crosspoint
 for (i = 0; i < 8; i++)
	{ 	bit_plane_CP[i] = (char **) calloc (hei*wid, sizeof (char));
    	if (bit_plane_CP[i] == (char **)NULL)
    		{  printf ("Cannot allocate dynamic array for bit planes !");
            getch();
      		exit (1);
         }
   }


 // Khoi tao gia tri ban dau cho bit_plane_CP
 for(bit_no=0;bit_no<8;bit_no++)
 	for(int k=0;k<hei;k++)
   	for(int m=0;m<wid;m++)
      	*((char *)bit_plane_CP[bit_no]+k*wid+m)=0;

  // Giai thuat tim vung crosspoint nhu sau :
  // Ung voi tung gia tri cua n, tim cac gia tri tuong ung trong vung crosspoint
  // Neu co mot day nhieu hon 1/10 chieu dai toi da cua vung crosspoint thi lay
  // Thu 10 lam vi tri danh dau sau do

  for(bit_no =7; bit_no >4; bit_no--)
   {  int minA,maxA,minR,maxR;
      minA = pow(2,bit_no)-pow(2,bit_no-1);
      minR = pow(2,bit_no)-pow(2,bit_no-2);
      maxA = pow(2,bit_no)+pow(2,bit_no-1)-1;
      maxR = pow(2,bit_no)+pow(2,bit_no-2)-1;
      for(i=0;i<hei;i++)
   	for(j=0;j<wid;j++)
       { if(minA<=pict[i][j] && pict[i][j]<=maxA)
         *((char *)refer_plane[7-bit_no] +i*wid+j)  = 1;
         if(minR<=pict[i][j] && pict[i][j]<=maxR)
         *((char *)refer_plane[7-bit_no+1] +i*wid+j)  = 2;

       }
   }
 // Sau do quet theo hang de tim cac vung tren do
 int start;
 for(bit_no = 7; bit_no>=4;bit_no--)
  for(i=0;i<hei;i++)
   for(j=0;j<wid;j++)
    {
     if(*((char *)refer_plane[7-bit_no] +i*wid+j)==0)
     continue;
     if(*((char *)refer_plane[7-bit_no] +i*wid+j)==1)
     { start=j;
       int k=0;
       while(*((char *)refer_plane[7-bit_no] +i*wid+start)==1 && start<wid)
         { // Tim vung co toi da so diem crosspoint
           k++;
           start++;
         }

     last = start-1;
     first = last-k+1;
     if(k>=pow(2,bit_no-1))
     { *((char *)bit_plane_CP[bit_no-1]+i*wid+last)=1;
       *((char *)bit_plane_CP[bit_no-1]+i*wid+first)=1;
       lencp++;
       output(1,posout,valueout,cp,lencp);
       // Danh dau tren ban do crosspoint va tinh alpha
       int count=0;
       float fvar=0;
       for(int index=first;index<=last;index++)
       { if(bit(bit_no-1,pict[i][index])== 0 &&  *((char *)bit_plane[bit_no-1]+i*wid+index)==1)
       count++;
       }
       fvar=float(count)/(float)k;
       alpha[bit_no-1]=(alpha[bit_no-1]<fvar?fvar:alpha[bit_no-1]);
      /////////////////////////////////////////////////
     // for(int index=first;index<=last;index++)
     // *((char *)bit_plane_CP[bit_no-1]+i*wid+index)=1;
     }
     else
     { for(int temp = last;temp>=first;temp--)
       *((char *)refer_plane[7-bit_no] +i*wid+temp)=0;

     }
     j=start-1;
    }

 	 else if(*((char *)refer_plane[7-bit_no] +i*wid+j)==2)
     { start=j;
       int k=0;
     while(*((char *)refer_plane[7-bit_no] +i*wid+start)==2 && start<wid)
         { // Tim vung co toi da so diem crosspoint
           k++;
           start++;
         }
     last = start-1;
     first = last-k+1;
     if(k>=pow(2,bit_no-1))
     { *((char *)bit_plane_CP[bit_no-1]+i*wid+last)=1;
       *((char *)bit_plane_CP[bit_no-1]+i*wid+first)=1;
       lencp++;
       output(0,posout,valueout,cp,lencp);
      //for(int index=first;index<=last;index++)
      //*((char *)bit_plane_CP[bit_no-1]+i*wid+index)=1;

      // Danh dau tren ban do crosspoint va tinh beta
       int count=0;
       float fvar=0;
       for(int index=first;index<=last;index++)
       {
       if(bit(bit_no-1,pict[i][index])==1 &&  *((char *)bit_plane[bit_no-1]+i*wid+index)==0)
       count++;
       }
       fvar=float(count)/(float)k;
       beta[bit_no-1]=(beta[bit_no-1]<fvar?fvar:beta[bit_no-1]);
     }
     else
     { for(int temp = last;temp>=first;temp--)
       *((char *)refer_plane[7-bit_no] +i*wid+temp)=0;
     }
    j=start-1;
   }

 }
 if (posout!=7)
 fputc(valueout,cp);
 lencp-=2;
 printf("so vung crosspoint : %d\n",lencp);
 for (i=0; i<8 ; i++)
  printf("%f\t%f\n" ,alpha[i], beta[i]);
// Tao ma tran anh moi
for(i=0;i<hei;i++)
for(j=0;j<wid;j++)
{ int temp=0;
  for(bit_no=0;bit_no<8;bit_no++)
  { temp+=*((char *)bit_plane_CP[bit_no]+i*wid+j)*pow(2,bit_no);
  }
  pict[i][j]=temp;
}
}

void findcp1(void)
{
  int i,j,first,last;
  lencp1=2;
  int posout=7;
  int valueout=0;                                      // EACH BITPLANE SAVE 0ST ROW AND 0ST COL
  FILE *cp;
  cp=fopen("temp.ref","wb");
 // Gan gia tri khoi tao cho alpha va beta
 for(bit_no = 0; bit_no < 8; bit_no++ )
 	alpha1[bit_no]= beta1[bit_no] = 0;

 // Tao ban do dung de nen vung crosspoint
 for (i = 0; i < 8; i++)
	{ 	bit_plane_CP1[i] = (char **) calloc (hei*wid, sizeof (char));
    	if (bit_plane_CP1[i] == (char **)NULL)
    		{  printf ("Cannot allocate dynamic array for bit planes !");
            getch();
      		exit (1);
         }
   }


 // Khoi tao gia tri ban dau cho bit_plane_CP1
 for(bit_no=0;bit_no<8;bit_no++)
 	for(int k=0;k<hei;k++)
   	for(int m=0;m<wid;m++)
      	*((char *)bit_plane_CP1[bit_no]+k*wid+m)=0;

  // Giai thuat tim vung crosspoint nhu sau :
  // Ung voi tung gia tri cua n, tim cac gia tri tuong ung trong vung crosspoint
  // Neu co mot day nhieu hon 1/10 chieu dai toi da cua vung crosspoint thi lay
  // Thu 10 lam vi tri danh dau sau do

  for(bit_no =7; bit_no >=1; bit_no--)
   {  int minA,maxA;
      minA = pow(2,bit_no+8)-pow(2,bit_no+7);
      //minR = pow(2,bit_no+8)-pow(2,bit_no+6);
      maxA = pow(2,bit_no+8)+pow(2,bit_no+7)-1;
      //maxR = pow(2,bit_no+8)+pow(2,bit_no+6)-1;
      for(i=0;i<hei;i++)
   	for(j=0;j<wid;j++)
       { if((minA<=pict[i][j]) && (pict[i][j]<=maxA))
         *((char *)refer_plane1[7-bit_no] +i*wid+j)  = 1;
         //if(minR<=pict[i][j] && pict[i][j]<=maxR)
         //*((char *)refer_plane1[7-bit_no+1] +i*wid+j)  = 2;

       }
   }
   
   
   for(bit_no =7; bit_no >=2; bit_no--)
   {  int minR,maxR;
      //minA = pow(2,bit_no+8)-pow(2,bit_no+7);
      minR = pow(2,bit_no+8)-pow(2,bit_no+6);
      //maxA = pow(2,bit_no+8)+pow(2,bit_no+7)-1;
      maxR = pow(2,bit_no+8)+pow(2,bit_no+6)-1;
      for(i=0;i<hei;i++)
   	for(j=0;j<wid;j++)
       { //if(minA<=pict[i][j] && pict[i][j]<=maxA)
         //*((char *)refer_plane1[7-bit_no] +i*wid+j)  = 1;
         if((minR<=pict[i][j]) && (pict[i][j]<=maxR))
         *((char *)refer_plane1[7-bit_no+1] +i*wid+j)  = 2;

       }
   }
 // Sau do quet theo hang de tim cac vung tren do
 int start;
 for(bit_no = 7; bit_no>=1;bit_no--)
  for(i=0;i<hei;i++)
   for(j=0;j<wid;j++)
    {
     if(*((char *)refer_plane1[7-bit_no] +i*wid+j)==0)
     continue;
     if(*((char *)refer_plane1[7-bit_no] +i*wid+j)==1)
     { start=j;
       int k=0;
       while(*((char *)refer_plane1[7-bit_no] +i*wid+start)==1 && start<wid)
         { // Tim vung co toi da so diem crosspoint
           k++;
           start++;
         }

     last = start-1;
     first = last-k+1;
     if(k>=pow(2,bit_no-1))
     { *((char *)bit_plane_CP1[bit_no-1]+i*wid+last)=1;
       *((char *)bit_plane_CP1[bit_no-1]+i*wid+first)=1;
       lencp1++;
       output(1,posout,valueout,cp,lencp1);
       // Danh dau tren ban do crosspoint va tinh alpha
       int count=0;
       float fvar=0;
       for(int index=first;index<=last;index++)
       { if(bit_last(bit_no-1,pict[i][index])== 0 &&  *((char *)bit_plane1[bit_no-1]+i*wid+index)==1)
       count++;
       }
       fvar=float(count)/(float)k;
       alpha1[bit_no-1]=(alpha1[bit_no-1]<fvar?fvar:alpha1[bit_no-1]);
      /////////////////////////////////////////////////
     // for(int index=first;index<=last;index++)
     // *((char *)bit_plane_CP1[bit_no-1]+i*wid+index)=1;
     }
     else
     { for(int temp = last;temp>=first;temp--)
       *((char *)refer_plane1[7-bit_no] +i*wid+temp)=0;

     }
     j=start-1;
    }

 	 else if(*((char *)refer_plane1[7-bit_no] +i*wid+j)==2)
     { start=j;
       int k=0;
     while(*((char *)refer_plane1[7-bit_no] +i*wid+start)==2 && start<wid)
         { // Tim vung co toi da so diem crosspoint
           k++;
           start++;
         }
     last = start-1;
     first = last-k+1;
     if(k>=pow(2,bit_no-1))
     { *((char *)bit_plane_CP1[bit_no-1]+i*wid+last)=1;
       *((char *)bit_plane_CP1[bit_no-1]+i*wid+first)=1;
       lencp1++;
       output(0,posout,valueout,cp,lencp1);
      //for(int index=first;index<=last;index++)
      //*((char *)bit_plane_CP1[bit_no-1]+i*wid+index)=1;

      // Danh dau tren ban do crosspoint va tinh beta
       int count=0;
       float fvar=0;
       for(int index=first;index<=last;index++)
       {
       if(bit_last(bit_no-1,pict[i][index])==1 &&  *((char *)bit_plane1[bit_no-1]+i*wid+index)==0)
       count++;
       }
       fvar=float(count)/(float)k;
       beta1[bit_no-1]=(beta1[bit_no-1]<fvar?fvar:beta1[bit_no-1]);
     }
     else
     { for(int temp = last;temp>=first;temp--)
       *((char *)refer_plane1[7-bit_no] +i*wid+temp)=0;
     }
    j=start-1;
   }

 }
 if (posout!=7)
 fputc(valueout,cp);
 lencp1-=2;
 printf("so vung crosspoint : %d\n",lencp1);
 for (i=0; i<8 ; i++)
  printf("%f\t%f\n" ,alpha1[i], beta1[i]);
// Tao ma tran anh moi
for(i=0;i<hei;i++)
for(j=0;j<wid;j++)
{ int temp=0;
  for(bit_no=0;bit_no<8;bit_no++)
  { temp+=*((char *)bit_plane_CP1[bit_no]+i*wid+j)*pow(2,bit_no+8);
  }
  pict[i][j]=temp;
}
//printf("pict=%d\n",pict[1][2]);
//getch();
}
/*
void findcp1(void)
{
  int i,j,first,last;
  lencp1=2;
  int posout=7;
  int valueout=0;                                      // EACH BITPLANE SAVE 0ST ROW AND 0ST COL
  FILE *cp;
  cp=fopen("temp.ref","wb");
 // Gan gia tri khoi tao cho alpha va beta
 for(bit_no = 0; bit_no < 8; bit_no++ )
 	alpha1[bit_no]= beta1[bit_no] = 0;

 // Tao ban do dung de nen vung crosspoint
 for (i = 0; i < 8; i++)
	{ 	bit_plane_CP1[i] = (char **) calloc (hei*wid, sizeof (char));
    	if (bit_plane_CP1[i] == (char **)NULL)
    		{  printf ("Cannot allocate dynamic array for bit planes !");
            getch();
      		exit (1);
         }
   }


 // Khoi tao gia tri ban dau cho bit_plane_CP1
 for(bit_no=1;bit_no<8;bit_no++)
 	for(int k=0;k<hei;k++)
   	for(int m=0;m<wid;m++)
      	*((char *)bit_plane_CP1[bit_no]+k*wid+m)=0;

  // Giai thuat tim vung crosspoint nhu sau :
  // Ung voi tung gia tri cua n, tim cac gia tri tuong ung trong vung crosspoint
  // Neu co mot day nhieu hon 1/10 chieu dai toi da cua vung crosspoint thi lay
  // Thu 10 lam vi tri danh dau sau do

  for(bit_no =1; bit_no <=2; bit_no++)
   {  int minA,maxA,minR,maxR;
      minA = pow(2,bit_no+8)-pow(2,bit_no+7);
      minR = pow(2,bit_no+8)-pow(2,bit_no+6);
      maxA = pow(2,bit_no+8)+pow(2,bit_no+7)-1;
      maxR = pow(2,bit_no+8)+pow(2,bit_no+6)-1;
      for(i=0;i<hei;i++)
   	for(j=0;j<wid;j++)
       { if(minA<=pict[i][j] && pict[i][j]<=maxA)
         *((char *)refer_plane1[bit_no] +i*wid+j)  = 1;
         if(minR<=pict[i][j] && pict[i][j]<=maxR)
         *((char *)refer_plane1[bit_no+1] +i*wid+j)  = 2;

       }
   }
 // Sau do quet theo hang de tim cac vung tren do
 int start;
 for(bit_no = 1; bit_no<=3;bit_no++)
  for(i=0;i<hei;i++)
   for(j=0;j<wid;j++)
    {
     if(*((char *)refer_plane1[bit_no] +i*wid+j)==0)
     continue;
     if(*((char *)refer_plane1[bit_no] +i*wid+j)==1)
     { start=j;
       int k=0;
       while(*((char *)refer_plane1[bit_no] +i*wid+start)==1 && start<wid)
         { // Tim vung co toi da so diem crosspoint
           k++;
           start++;
         }

     last = start-1;
     first = last-k+1;
     if(k>=pow(2,bit_no-1))
     { *((char *)bit_plane_CP1[bit_no-1]+i*wid+last)=1;
       *((char *)bit_plane_CP1[bit_no-1]+i*wid+first)=1;
       lencp1++;
       output(1,posout,valueout,cp,lencp1);
       // Danh dau tren ban do crosspoint va tinh alpha
       int count=0;
       float fvar=0;
       for(int index=first;index<=last;index++)
       { if(bit_last(bit_no-1,pict[i][index])== 0 &&  *((char *)bit_plane1[bit_no-1]+i*wid+index)==1)
       count++;
       }
       fvar=float(count)/(float)k;
       alpha1[bit_no-1]=(alpha1[bit_no-1]<fvar?fvar:alpha1[bit_no+-1]);
      /////////////////////////////////////////////////
     // for(int index=first;index<=last;index++)
     // *((char *)bit_plane_CP1[bit_no-1]+i*wid+index)=1;
     }
     else
     { for(int temp = last;temp>=first;temp--)
       *((char *)refer_plane1[bit_no] +i*wid+temp)=0;

     }
     j=start-1;
    }

 	 else if(*((char *)refer_plane1[bit_no] +i*wid+j)==2)
     { start=j;
       int k=0;
     while(*((char *)refer_plane1[bit_no] +i*wid+start)==2 && start<wid)
         { // Tim vung co toi da so diem crosspoint
           k++;
           start++;
         }
     last = start-1;
     first = last-k+1;
     if(k>=pow(2,bit_no-1))
     { *((char *)bit_plane_CP1[bit_no-1]+i*wid+last)=1;
       *((char *)bit_plane_CP1[bit_no-1]+i*wid+first)=1;
       lencp1++;
       output(0,posout,valueout,cp,lencp1);
      //for(int index=first;index<=last;index++)
      //*((char *)bit_plane_CP1[bit_no-1]+i*wid+index)=1;

      // Danh dau tren ban do crosspoint va tinh beta
       int count=0;
       float fvar=0;
       for(int index=first;index<=last;index++)
       {
       if(bit_last(bit_no-1,pict[i][index])==1 &&  *((char *)bit_plane1[bit_no-1]+i*wid+index)==0)
       count++;
       }
       fvar=float(count)/(float)k;
       beta1[bit_no-1]=(beta1[bit_no-1]<fvar?fvar:beta1[bit_no+-1]);
     }
     else
     { for(int temp = last;temp>=first;temp--)
       *((char *)refer_plane1[bit_no] +i*wid+temp)=0;
     }
    j=start-1;
   }

 }
 if (posout!=7)
 fputc(valueout,cp);
 lencp1-=2;
 printf("so vung crosspoint : %d\n",lencp1);
 for (i=0; i<8 ; i++)
  printf("%f\t%f\n" ,alpha1[i], beta1[i]);
// Tao ma tran anh moi
for(i=0;i<hei;i++)
for(j=0;j<wid;j++)
{ int temp=0;
  for(bit_no=0;bit_no<8;bit_no++)
  { temp+=*((char *)bit_plane_CP1[bit_no]+i*wid+j)*pow(2,bit_no+8);
  }
  pict[i][j]=temp;
}
//printf("pict=%d\n",pict[1][2]);
//getch();
}
*/

void compressCP(void)
{
   int i,j,k,m;
   char outfile_name[MAXFILE+MAXEXT];
  	char datfile_name[MAXFILE+MAXEXT];
   char ext[MAXEXT];
   int c;
   FILE *refer,*coded,*temp;

   // Compress crosspoint map.
   //{ Use pict[i][j] as map of crosspoint}

   // Reset refer plane and bit_plane
   for(bit_no=0;bit_no<7;bit_no++)
   	for(k=0;k<hei;k++)
      	for(m=0;m<wid;m++)
				*((char *)refer_plane[bit_no]+k*wid+m)=0;

   // Make bit_planes don't use Graycode
   make_bp(0);

   // Encode bitplane
   encoder(3);
   getch();
   // Combine data of crosspoint with .out file
   strcpy(outfile_name,orig_name);
  	sprintf(ext,".out2");
  	strcat(outfile_name,ext);
   if ( (coded = fopen (outfile_name, "rb+")) == NULL )
  		{	printf ("Cannot open coding file to append");
         getch();
         exit (1);
		}
   if ( (refer = fopen ("refer.dta", "rb")) == NULL )
  	  	{  printf ("Cannot create out coding file");
      	getch();
    		exit (1);
		}
    if ( (temp = fopen ("temp.ref", "rb")) == NULL )
  		{	printf ("Cannot open coding file to append");
         getch();
         exit (1);
		}

   // Goto end of outfile file
   fseek(coded,0,SEEK_END);

   // write pro. table
   for (bit_no =3; bit_no < 7; bit_no++)
   {
   	for (i = 0; i < 32; i++)
   	{
     	 fwrite(&p_stored[bit_no][i],sizeof(p_stored[bit_no][i]),1,coded);
     	 //printf(" %d ",p_stored[bit_no][i]);
   	}
   	//printf("\n");
   }
 //getch();
   //write len of codeword
   fwrite(&len[3],sizeof(len[3]),1,coded);
   // Write data of refer file to .out file
   while((c=fgetc(refer))!=EOF)
   { 
    // printf(" %d ",c);
     fputc(c,coded);
   }

 fseek(coded,0,SEEK_END);
  al=ftell(coded);
  
  // Write alpha and beta;
   for(i=0;i<8;i++)
   fwrite(&alpha[i],sizeof(alpha[i]),1,coded);
   // Write beta
   for(i=0;i<8;i++)
   fwrite(&beta[i],sizeof(beta[i]),1,coded);
  fwrite(&lencp,sizeof(lencp),1,coded);
  while((c=fgetc(temp))!=EOF)
  { fputc(c,coded); 
    //printf(" %d ",c);
  }
 //getch();
 fseek(coded,0,SEEK_END);
 if (maxi-mini<256)
 {
 	fwrite(&al,sizeof(al),1,coded);
 	//printf("al=%d\n",al);
 }
  //getch();
   size[2]=ftell(coded);
   fcloseall();
   remove("refer.dta");
   remove("temp.ref");
}

void compressCP1(void)
{
   int i,j,k,m;
   char outfile_name[MAXFILE+MAXEXT];
  	char datfile_name[MAXFILE+MAXEXT];
   char ext[MAXEXT];
   int c;
   FILE *refer,*coded,*temp;

   // Compress crosspoint map.
   //{ Use pict[i][j] as map of crosspoint}

   // Reset refer plane and bit_plane
   for(bit_no=0;bit_no<7;bit_no++)
   	for(k=0;k<hei;k++)
      	for(m=0;m<wid;m++)
				*((char *)refer_plane1[bit_no]+k*wid+m)=0;

   // Make bit_planes don't use Graycode
   make_bp1(0);

   // Encode bitplane
   encoder1(3);
   getch();
   // Combine data of crosspoint with .out file
   strcpy(outfile_name,orig_name);
  	sprintf(ext,".out2");
  	strcat(outfile_name,ext);
   if ( (coded = fopen (outfile_name, "rb+")) == NULL )
  		{	printf ("Cannot open coding file to append");
         getch();
         exit (1);
		}
   if ( (refer = fopen ("refer.dta", "rb")) == NULL )
  	  	{  printf ("Cannot create out coding file");
      	getch();
    		exit (1);
		}
    if ( (temp = fopen ("temp.ref", "rb")) == NULL )
  		{	printf ("Cannot open coding file to append");
         getch();
         exit (1);
		}

   // Goto end of outfile file
   fseek(coded,0,SEEK_END);
   p_sto1com=ftell(coded);


   // write pro. table
   for(bit_no=0;bit_no<=1;bit_no++)
{
  for (i = 0; i < 32; i++)
  {
  	fwrite(&p_stored1[bit_no][i],sizeof(p_stored1[bit_no][i]),1,coded);
  	//printf(" %d ",p_stored1[bit_no][i]);
  }
    //printf("\n");
}

   //write len of codeword
   fwrite(&len1[3],sizeof(len1[3]),1,coded);
   printf("len1=%d\n",len1[3]);
   // Write data of refer file to .out file
   while((c=fgetc(refer))!=EOF)
   { fputc(c,coded); }

  fseek(coded,0,SEEK_END);
   al1=ftell(coded);

  // Write alpha and beta;
   for(i=0;i<8;i++)
   fwrite(&alpha1[i],sizeof(alpha1[i]),1,coded);
   // Write beta
   for(i=0;i<8;i++)
   fwrite(&beta1[i],sizeof(beta1[i]),1,coded);
  fwrite(&lencp1,sizeof(lencp1),1,coded);
  while((c=fgetc(temp))!=EOF)
  { fwrite(&c,sizeof(c),1,coded); 
  //printf(" %d ",c);
  }
  //getch();
  fseek(coded,0,SEEK_END);
  
  fwrite(&p_stocom,sizeof(p_stocom),1,coded);
  fwrite(&p_sto1code,sizeof(p_sto1code),1,coded);
  fwrite(&p_sto1com,sizeof(p_sto1com),1,coded);
  fwrite(&al,sizeof(al),1,coded);
  fwrite(&al1,sizeof(al1),1,coded);
  fseek(coded,0,SEEK_END);
  printf("p_sto1com=%d\n",p_sto1com);
   size1[2]=ftell(coded);
   
   fcloseall();
   remove("refer.dta");
   remove("temp.ref");
}