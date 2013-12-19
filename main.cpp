#include <iostream>
#include <map>
#include <vector>
#include "input.h"
#include "ins_set.h"

using namespace std;

int main(int argc, char *argv[])
{
	ins_set sic_ins_set;
    Minput src("code.txt");

    vector<int> LOCTAB;
    int _LOCCTR = 0;
    map<string ,int> SYMTAB;
    
    int ERR_FLAG = 0;

    if( true != src.getCode() )
    {
        cout << "unable to open file" << endl; 
        return -1; 
    }

    if( src.code[0].ins == "START" )
    {
        istringstream iss( src.code[0].value );
        iss >> hex >> _LOCCTR ;
    }
    LOCTAB.push_back( _LOCCTR );
    int i = 1;
    while( src.code[i].ins != "END" )
    {
        // label process
        if( src.code[i].label != "" )
        {
            map<string ,int>::iterator it = SYMTAB.find( src.code[i].label ); 
            if( it != SYMTAB.end() )
            {
                cout << "Symbol " << src.code[i].label << "already decleared" << endl; 
                ERR_FLAG |= 0x1;
            }
            else
            {
                SYMTAB[ src.code[i].label ] = _LOCCTR; 
            }
        }
        // end label process
    
        LOCTAB.push_back( _LOCCTR );
        // op code process
		int result = sic_ins_set.getInsByte( src.code[i].ins );
        
		if( result != 0 )
        {
            _LOCCTR += result; 
            if( result == 3 && src.code[i].extra != "" )
                _LOCCTR++;
        }
        else if( src.code[i].ins == "WORD" )
        {
            _LOCCTR += 3;
        }
        else if( src.code[i].ins == "RESW" )
        {
            _LOCCTR += 3 * atoi( src.code[i].value.c_str() );
        }
        else if( src.code[i].ins == "RESB" )
        {
            _LOCCTR += atoi( src.code[i].value.c_str() );
        }
        else if( src.code[i].ins == "BYTE" )
        {
            string str = src.code[i].value;
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
                    _LOCCTR += int(leng+0.5);
                }
                else if( c == 'C' || c == 'c' )
                {
                   _LOCCTR += tok.size(); 
                }
            
            }
            else
                _LOCCTR += 1; 
        }
        else
        {
            cout << "invalid op code " << src.code[i].label << src.code[i].ins << "." <<src.code[i].value << src.code[i].extra << endl;
            ERR_FLAG |= 0x2; 
        }
        //end op code process
        
        ++i;
    }
    
    /* check 
    for( int i = 0 ; i < src.code.size() ; ++i )
        cout << hex << LOCTAB[i] << "\t" << src.code[i].label << "\t" << src.code[i].ins << "\t" << src.code[i].value << "\t" << src.code[i].extra << endl;
    cout << hex << "total length = " << _LOCCTR-LOCTAB[0] << endl;
    */
    
     
   
   //  after pass 1
   //  symbol address and all location record at SYMTAB and LOCTAB
   putchar('H');
   printf("%-6s", src.code[0].label.c_str());
   printf("%06X", LOCTAB[0]);
   printf("%06X", _LOCCTR-LOCTAB[0]);
   putchar('\n');


	vector<int> object;
	object.push_back( 0 );
	
	i = 1;
    int code_length = 0;
    string buffer;
	while( src.code[i].ins != "END" )
	{
        if( code_length == 0 )
        {
            putchar('T');
            printf( "%06X", LOCTAB[i] );
            buffer = "";
        }
		int result = sic_ins_set.getInsByte( src.code[i].ins );
		if( result != 0 )
		{
            string symbol = src.code[i].value;
            replace( symbol.begin() , symbol.end() , ',' , '\n' );
            string ex;
            istringstream iss( symbol );
            iss >> symbol >> ex;
            map<string ,int>::iterator it = SYMTAB.find( symbol ); 
			int bytecode = (sic_ins_set.getInsformat( src.code[i].ins ));
            int address = 0;
            if( it != SYMTAB.end() )
            {
                address = SYMTAB[ symbol ]; 
                if( ex == "X" || ex == "x" )
                    address |= 1 << 15; 
            }
            else
			{
				//cout << "unable to find symbol: ins=" << src.code[i].ins << " val=" << src.code[i].value << endl;
			}
			bytecode |= address;
            char out[10] = {};
            sprintf( out , "%06X" , bytecode );
            buffer += out;
            code_length += ( LOCTAB[i+1]-LOCTAB[i] );
			//object.push_back( bytecode );
		}
		else if( src.code[i].ins == "BYTE" )
		{
			if( src.code[i].value[1] == '\'' )
			{
				string str( src.code[i].value.begin()+2 , src.code[i].value.end()-1 );
				if( src.code[i].value[0] == 'C' )
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
					//object.push_back( sum );
					string format = "%0";
                    format += cl*2+'0';
                    format += "X";	
                    char out[10] = {};
                    sprintf( out , format.c_str() , sum );
                    buffer += out;
                    code_length += cl;
                }
				if( src.code[i].value[0] == 'X' )
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
		else if( src.code[i].ins == "RESB" )
		{
            code_length += ( LOCTAB[i+1]-LOCTAB[i] );
			//object.push_back( 0 );
		}
		else if( src.code[i].ins == "WORD" )
		{
		    string str = src.code[i].value;
            istringstream iss( str );
            int n;
            iss >> n;
            char out[10] = {};
            sprintf( out , "%03X" , n );
            buffer += out;
            code_length += ( LOCTAB[i+1]-LOCTAB[i] );
            //object.push_back(n);
		}
		else if( src.code[i].ins == "RESW" )
		{
            buffer += "000";
			//object.push_back( 0 );
            code_length += ( LOCTAB[i+1]-LOCTAB[i] );
		}
		else
		{
			cout << "don't find ins: " << src.code[i].ins << endl;
			object.push_back( 0 );
		}
		
		if( code_length+3 > 30 )
        {
           printf("%02X%s\n" , code_length , buffer.c_str() );
           buffer = "";
           code_length = 0;
        }
		
		++i;
	}
	
    printf("%02X%s\n" , code_length , buffer.c_str() );
    buffer = "";
    code_length = 0;
     putchar('E');
     printf("%06X", LOCTAB[0]);
     putchar('\n');
     
     /* for check after pass 2
    for( size_t i = 0 ; i < LOCTAB.size() ; ++i )
    {
        cout << hex << LOCTAB[i] << "\t" << dec 
             << src.code[i].label << "\t"
             << src.code[i].ins << "\t"
             << src.code[i].value << "\t";
        printf("%06x\n",object[i]);
    }
    */
     
    return 0;
}
