//  TW.c
//  Original Author: Peter Meyer (1998-01-05)
//  Revised by: Diamond Bond (2020-09-10)
//  Calculate the value of the timewave at a point.

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define FALSE 0
#define TRUE  1
#define NUM_POWERS 48
#define PREC 16
#define NUM_SETS 4
#define NUM_DATA_POINTS 384 
#define CALC_PREC       10   /*  precision in calculation  */
                             /*  of wave values            */

double powers[NUM_POWERS];
//  Powers of (normally) 64.
//  Due to the limitations of double precision
//  floating point arithmetic these values are
//  exact only up to powers[8] for powers of 64.

int wave_factor = 64;   //  default wave factor 
int number_set;

char *usage = "\nUse: TW days_to_zero_date days days ... [wf=nn]."
  "\nwf = wave factor (default 64, range 2-10000)\n";
  
char temp[32];

char *set_name[NUM_SETS] = { "Kelley", "Watkins", "Sheliak", "Huang Ti" };  

char *strlwr(char *str)
{
  unsigned char *p = (unsigned char *)str;

  while (*p) {
     *p = tolower((unsigned char)*p);
      p++;
  }

  return str;
}

//  The number sets.
int w[NUM_SETS][NUM_DATA_POINTS] = 
{
  {
#include "DATA.TW1" //  half-twist
  },
  { 
#include "DATA.TW2" //  no half-twist
  },
  { 
#include "DATA.TW3" //  Sheliak 
  }, 
  { 
#include "DATA.TW4" //  HuangTi (no half-twist)
  } 
};

void set_powers(void);
double f(double x, int number_set);
double v(double y, int number_set);
double mult_power(double x, int i);
double div_power(double x, int i);

/*-----------------------------*/
void main(int argc, char *argv[])
{
double dtzp;
int i, j, ch;

if ( argc == 1 )
    {
    printf("%s",usage);
    exit(1);
    } 

for ( i=1; i<argc; i++ )
    {
    strlwr(argv[i]);
    if ( !memcmp(argv[i],"wf=",3) )
        {
        wave_factor = atoi(&argv[i][3]);
        if ( wave_factor < 2 || wave_factor > 10000 )
        {
        printf("%s",usage);
        exit(2);
        }
    }
else
    {
    ch = argv[i][0];
    if ( ! ( ( ch == '.' )
     || ( (unsigned int)(ch-'0') <= 9 ) ) )
        {
        printf("%s",usage);
        exit(3);
        }
    }
}

set_powers();
printf("\nWave factor = %d\n",wave_factor);

for ( i=1; i<argc; i++ )
    {
    if ( memcmp(argv[i],"wf=",3) )
        {
        dtzp = atof(argv[i]);
        sprintf(temp,"%.*f",PREC,dtzp);
        j = strlen(temp) - 1;
        while ( ( temp[j] == '0' ) && j > 0 )
            temp[j--] = 0;
        strcat(temp,"0 day");
        if ( dtzp != 1.0 )
            strcat(temp,"s");
        printf("\nThe value of the timewave %s prior to the zero point is\n",temp); 
        for ( number_set=0; number_set<NUM_SETS; number_set++ )
            {
            printf("%.*f (%s)\n",PREC,f(dtzp,number_set),set_name[number_set]);
            }
    
        }
    }
}

//  wave_factor is a global variable
/*-----------------*/
void set_powers(void)
{
unsigned int j;

/*  put powers[j] = wave_factor^j  */

powers[0] = (double)1;
for ( j=1; j<NUM_POWERS; j++ )
    powers[j] = wave_factor*powers[j-1];
}

/*  x is number of days to zero date  */
/*--------------*/
double f(double x,
         int number_set)
{
register i;
double sum = 0.0, last_sum = 0.0;

if ( x )
    {
    for ( i=0; x>=powers[i]; i++ )
    sum += mult_power(v(div_power(x,i),number_set),i);

    i = 0;
    do
        {
        if ( ++i > CALC_PREC+2 )
            break;
        last_sum = sum;
        sum += div_power(v(mult_power(x,i),number_set),i);
        } while ( ( sum == 0.0 ) || ( sum > last_sum ) );
    }

/*  dividing by 64^3 gives values consistent with the Apple // version
 *  and provides more convenient y-axis labels
 */
sum = div_power(sum,3);

return ( sum );
}

/*--------------*/
double v(double y,
         int number_set)
{ 
int i = (int)(fmod(y,(double)NUM_DATA_POINTS));
int j = (i+1)%NUM_DATA_POINTS;
double z = y - floor(y);

return ( z==0.0 ? (double)w[number_set][i] : 
    ( w[number_set][j] - w[number_set][i] )*z + w[number_set][i] );
}

/*  in order to speed up the calculation, if wave factor = 64
 *  then instead of using multiplication or division operation
 *  we act directly on the floating point representation;
 *  multiplying by 64^i is accomplished by adding i*0x60
 *  to the exponent (the last 2 bytes of the 8-byte representation);
 *  dividing by 64^i is accomplished by subtracting i*0x60
 *  from the exponent
 */

/*-----------------------*/
double mult_power(double x,
                  int i)
{
int *exponent = (int *)&x + 3;

if ( wave_factor == 64 )
    *exponent += i*0x60;/*  measurably faster  */
else
    x *= powers[i];

return ( x );
}

/*----------------------*/
double div_power(double x,
                 int i)
{
int *exponent = (int *)&x + 3;

if ( ( wave_factor == 64 ) && ( *exponent > i*0x60 ) )
    *exponent -= i*0x60;
else
    x /= powers[i];

return ( x );
}
