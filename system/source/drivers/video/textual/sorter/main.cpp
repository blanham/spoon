#include <stdio.h>
#include <stdlib.h>

#include <ctype.h>


#include "font.h"

struct pair
{
	int key;
	int value;
	int height;
	int width;
};


struct pair intense[256];


int calc()
{
	for ( int i = 0; i < 256; i++ )
	{

		int count = 0;
		int left = 8;
		int top = 16;
		int bottom = 0;
		int right = 0;
		
		for ( int x = 0; x < 16; x++ )
		 for ( int y = 0; y < 8; y++ )
		 {
			if ( system_font[i][x][y] != 0 ) 
			{
				if ( x > bottom ) bottom = x;
				if ( x < top ) top = x;

				if ( y > right ) right = y;
				if ( y < left ) left = y;
				
				count += 1;
			}
		 }
		
		intense[i].key = i;
		intense[i].value = count;
		intense[i].width = right - left;
		intense[i].height = bottom - top;
	}

	return 0;
}


int sort()
{
	for ( int i = 0; i < 255; i++ )
	 for ( int j = (i+1); j < 256; j++ )
	 {
		struct pair temp;

		if ( intense[i].value > intense[j].value )
		{
			temp = intense[i];
			intense[i] = intense[j];
			intense[j] = temp;
		}
		else if ( intense[i].value == intense[j].value )
		{
				
			if ( (intense[i].width * intense[i].height) >
				 (intense[j].width * intense[j].height) ) 
			{
				temp = intense[i];
				intense[i] = intense[j];
				intense[j] = temp;
			}
			
		}
			 
	 }

	return 0;
}


int print()
{
		
	for ( int i = 0; i < 16; i++ )
	 {
		for ( int j = 0; j < 16; j++ )
		{
			int pos = j * 16 + i;
				
			if ( isprint(intense[pos].key) != 0 )
				printf( "%5c,%2i", intense[pos].key,  intense[pos].value );
			else
				printf( "%5c,%2i", ' ', intense[pos].value );

		}
		printf("\n");
	 }
	
// ----------------
//
//		

		printf("\n");


	
	char last_char = ' ';
	
	for ( int i = 0; i < 256; i++ )
	 {
		if ( isprint(intense[i].key) != 0 )
		{
			printf( "%5c,%2i\n", intense[i].key,  intense[i].value );
			last_char = intense[i].key;
		}
		else
		{
			printf( "%5c,%2i\n", last_char, intense[i].value );
		}
	 }

	printf("\n");


	
	return 0;

}




int dump()
{
	
	printf("%s","char real_intensity[256] = { ");
		
	for ( int i = 0; i < 255; i++ )
	 {
		printf("%i, ", intense[i].key );
	 }


	printf("%i };\n", intense[255].key );

	// -----------------
	
	printf("%s","char intensity[256] = { ");
	
	char last_char = ' ';	
	
	for ( int i = 0; i < 255; i++ )
	 {
		if ( isprint(  intense[i].key ) != 0 )
		{
			printf("%i, ", intense[i].key );
			last_char =  intense[i].key;
		}
		else
		{
			printf("%i, ", last_char );
		}
	 }


	if ( isprint( intense[255].key ) != 0 )
		printf("%i };\n", intense[255].key );
	else
		printf("%i };\n", last_char );


	
	return 0;

}


int main( int argc, char *argv )
{

	calc();
	sort();
	print();


	dump();

	
		
	return 0;
}


