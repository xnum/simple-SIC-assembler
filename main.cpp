#include <iostream>
#include <map>
#include <vector>
#include "input.h"
#include "ins_set.h"

using namespace std;

const int MAX_LEN = 200;

int main(int argc, char *argv[])
{
	ins_set sic_ins_set;
    Minput src("code.txt");

    int LOCTAB[ MAX_LEN ] = {};
    string SYMTAB_SYM[ MAX_LEN ];
    int    SYMTAB_LOC[ MAX_LEN ];
    
    int LOCTAB_top = 0;
    int SYMTAB_top = 0;
    
    int LOCCTR = 0;

    if( true != src.getCode() )
    {
        cout << "unable to open file" << endl; 
        return 0x1; 
    }

    if( src.code[0].ins == "START" )
    {
        istringstream iss( src.code[0].value );
        iss >> hex >> LOCCTR;
    }
    
    LOCTAB[ LOCTAB_top++ ] = LOCCTR;
    int _top = 1;
    while( src.code[_top].ins != "END" )
    {
        // label process
        if( src.code[_top].label != "" )
        {
            int ptr = SYMTAB_top;
            for( int index = 0 ; index < SYMTAB_top ; ++index )
            {
                if( SYMTAB_SYM[ index ] == src.code[_top].label )
                    ptr = index; 
            }
            if( ptr != SYMTAB_top )
            {
                cout << "Error: Symbol " << src.code[_top].label << " already decleared" << endl; 
                return 0x2;
            }
            else
            {
                SYMTAB_SYM[ SYMTAB_top ] = src.code[_top].label; 
                SYMTAB_LOC[ SYMTAB_top ] = LOCCTR; 
                SYMTAB_top++;
            }
        }
        // end label process
    
        LOCTAB[ LOCTAB_top++ ] = LOCCTR;
        // op code process
		int result = sic_ins_set.getInsByte( src.code[_top].ins );
        
		if( result != 0 )
        {
            LOCCTR += result; 
            if( result == 3 && src.code[_top].extra != "" )
                LOCCTR++;
        }
        else if( src.code[_top].ins == "WORD" )
        {
            LOCCTR += 3;
        }
        else if( src.code[_top].ins == "RESW" )
        {
            LOCCTR += 3 * atoi( src.code[_top].value.c_str() );
        }
        else if( src.code[_top].ins == "RESB" )
        {
            LOCCTR += atoi( src.code[_top].value.c_str() );
        }
        else if( src.code[_top].ins == "BYTE" )
        {
            string str = src.code[_top].value;
            if( str[1] == '\'' )
            {
                replace( str.begin() , str.end() , '\'' , '\n' );
                istringstream _iss( str );
                char c ;
                string tok;
                _iss >> c >> tok;
                if( c == 'X' || c == 'x' )
                {
                    double leng = tok.size() / 2;
                    LOCCTR += int(leng+0.5);
                }
                else if( c == 'C' || c == 'c' )
                {
                   LOCCTR += tok.size(); 
                }
            
            }
            else
                LOCCTR += 1; 
        }
        else
        {
            cout << "Error: invalid op code " 
                 << src.code[_top].label 
                 << src.code[_top].ins 
                 << src.code[_top].value
                 << src.code[_top].extra 
                 << endl;
                 
            return 0x3;
        }
        //end op code process
        
        ++_top;
    }
    
   //  after pass 1
   //  symbol address and all location record at SYMTAB and LOCTAB
   putchar('H');
   printf("%-6s", src.code[0].label.c_str());
   printf("%06X", LOCTAB[0]);
   printf("%06X", LOCCTR-LOCTAB[0]);
   putchar('\n');


	_top = 1;
    int code_length = 0;
    string buffer;
	while( src.code[_top].ins != "END" )
	{
        if( code_length == 0 )
        {
            putchar('T');
            printf( "%06X", LOCTAB[_top] );
            buffer = "";
        }
		int result = sic_ins_set.getInsByte( src.code[_top].ins );
		if( result != 0 )
		{
            string symbol = src.code[_top].value;
            replace( symbol.begin() , symbol.end() , ',' , '\n' );
            string ex;
            istringstream iss( symbol );
            iss >> symbol >> ex;
			int bytecode = (sic_ins_set.getInsformat( src.code[_top].ins ));
            int address = 0;
            
            int ptr = SYMTAB_top;
            for( int index = 0 ; index < SYMTAB_top ; ++index )
                if( SYMTAB_SYM[index] == symbol )
                    ptr = index;
            if( ptr != SYMTAB_top )
            {
                address = SYMTAB_LOC[ ptr ]; 
                if( ex == "X" || ex == "x" )
                    address |= 1 << 15; 
            }
            else if( symbol != "" )
			{
				cout << "Error: unable to find symbol: ins=" << src.code[_top].ins << " val=" << src.code[_top].value << endl;
                return 0x4;
			}
			
			bytecode |= address;
            char out[10] = {};
            sprintf( out , "%06X" , bytecode );
            buffer += out;
            code_length += ( LOCTAB[_top+1]-LOCTAB[_top] );
		}
		else if( src.code[_top].ins == "BYTE" )
		{
			if( src.code[_top].value[1] == '\'' )
			{
				string str( src.code[_top].value.begin()+2 , src.code[_top].value.end()-1 );
				if( src.code[_top].value[0] == 'C' )
				{
					istringstream iss( str );
					char n = 0;
					int sum = 0;
                    int cl = 0;
					while( iss.get(n) )
                    {
                        cl++;
                        sum = (sum << 8) + n;
                    }
					string format = "%0";
                    format += cl*2+'0';
                    format += "X";	
                    char out[10] = {};
                    sprintf( out , format.c_str() , sum );
                    buffer += out;
                    code_length += cl;
                }
				if( src.code[_top].value[0] == 'X' )
				{
					istringstream iss( str );
					int _n;
					iss >> hex >> _n;
                    
                    char out[10] = {};
                    sprintf( out , "%02X" , _n );
                    buffer += out;
                    code_length += 1;
				}
			}
		}
		else if( src.code[_top].ins == "RESB" )
		{
            code_length += ( LOCTAB[_top+1]-LOCTAB[_top] );
		}
		else if( src.code[_top].ins == "WORD" )
		{
		    string str = src.code[_top].value;
            istringstream iss( str );
            int n;
            iss >> n;
            char out[10] = {};
            sprintf( out , "%06X" , n );
            buffer += out;
            code_length += ( LOCTAB[_top+1]-LOCTAB[_top] );
		}
		else if( src.code[_top].ins == "RESW" )
		{
            code_length += ( LOCTAB[_top+1]-LOCTAB[_top] );
		}
		else
		{
			cout << "Error: don't find ins: " << src.code[_top].ins << endl;
            return 0x5;
		}
		
		if( code_length+3 > 30 )
        {
           printf("%02X%s\n" , buffer.size()/2 , buffer.c_str() );
           buffer = "";
           code_length = 0;
        }
		
		++_top;
	}
	
    printf("%02X%s\n" , code_length , buffer.c_str() );
    buffer = "";
    code_length = 0;
    putchar('E');
    printf("%06X", LOCTAB[0]);
    putchar('\n');
     
     
    return 0;
}
