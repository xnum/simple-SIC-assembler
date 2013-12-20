#include <iostream>
#include "input.h"
#include "ins_set.h"

using namespace std;

const int MAX_LEN = 200;

int main(int argc, char *argv[])
{
	ins_set sic_ins_set;            /*初始化SIC指令集資料*/
    Minput src("code.txt");         /*初始化原始碼資料*/
    int LOCTAB[ MAX_LEN ] = {};     /*初始化程式碼位址紀錄表格*/
    string SYMTAB_SYM[ MAX_LEN ];   /*初始化符號位址紀錄表格(符號名稱)*/
    int    SYMTAB_LOC[ MAX_LEN ];   /*初始化符號位址紀錄表格(符號位址)*/
    int LOCTAB_top = 0;             /*程式碼位址紀錄表格指標*/
    int SYMTAB_top = 0;             /*符號位址紀錄表格指標*/
    int LOCCTR = 0;                 /*紀錄PASS1處理的程式碼目前地址*/
    
    if( true != src.getCode() )     /*如果無法取得原始碼*/
    {
        cout << "unable to open file" << endl; 
        return 0x1; 
    }

    /*如果第1行是START就依照他給的位址來當START ADDRESS*/
    if( src.code[0].ins == "START" )
    {
        istringstream iss( src.code[0].value ); /*先把數值放進字串流內*/
        iss >> hex >> LOCCTR;  /*因為原始內容是HEX 要用HEX的方式讀取資料*/
    }
    
    LOCTAB[ LOCTAB_top++ ] = LOCCTR; /*寫入表格內*/
    
    int _top = 1; // 原始碼行數指標
    while( src.code[_top].ins != "END" )
    {
        // LABEL段處理程式碼 
        if( src.code[_top].label != "" )
        {
            int ptr = SYMTAB_top; // set pointer to end of SYMTAB
            for( int index = 0 ; index < SYMTAB_top ; ++index )
            {
                if( SYMTAB_SYM[ index ] == src.code[_top].label )
                    ptr = index; 
            }
            
            // if found data already in SYMTAB , pointer will point the label index
            if( ptr != SYMTAB_top )
            {
                cout << "Error: Symbol " << src.code[_top].label << " already decleared" << endl; 
                return 0x2;
            }
            else
            {
                // if not found , put data to SYMTAB and increase SYMTAB_top
                SYMTAB_SYM[ SYMTAB_top ] = src.code[_top].label; 
                SYMTAB_LOC[ SYMTAB_top ] = LOCCTR; 
                SYMTAB_top++;
            }
        }
        // END LABEL PROCESS CODE
    
        // write location to location table
        LOCTAB[ LOCTAB_top++ ] = LOCCTR;
        
        // op code process
        // search if the instruction of this line is available
		int result = sic_ins_set.getInsByte( src.code[_top].ins );
        
        // not equal zero means the instruction is available and result shows its byte format
		if( result != 0 )
        {
            // add to LOCCTR , locate code address
            LOCCTR += result; 
            
        }
        else if( src.code[_top].ins == "WORD" )
        {
            // word == 3 bytes
            LOCCTR += 3;
        }
        else if( src.code[_top].ins == "RESW" )
        {
            // value is the reserve words number
            // use atoi to convert string to number
            LOCCTR += 3 * atoi( src.code[_top].value.c_str() );
        }
        else if( src.code[_top].ins == "RESB" )
        {
            // same operation but is bytes there
            LOCCTR += atoi( src.code[_top].value.c_str() );
        }
        else if( src.code[_top].ins == "BYTE" )
        {
            // contant bytes
            // only calculate the length there
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
            // bad opcode
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
   
   // write header record first
   putchar('H');
   printf("%-6s", src.code[0].label.c_str());
   printf("%06X", LOCTAB[0]);
   printf("%06X", LOCCTR-LOCTAB[0]);
   putchar('\n');


   // do PASS2 and write out text record
	_top = 1;
    int code_length = 0;
    string buffer;
	while( src.code[_top].ins != "END" )
	{
        if( code_length == 0 )
        {
            // open a new line and write its first instrction's address
            putchar('T');
            printf( "%06X", LOCTAB[_top] );
            buffer = "";
        }
        
        // find ins's byte format and its opcode
		int result = sic_ins_set.getInsByte( src.code[_top].ins );
		if( result != 0 )
		{
            // convert value( if is symbol ) to its address
            string symbol = src.code[_top].value;
            // symbol is our value and , split the regster ex: BUFFER,X
            replace( symbol.begin() , symbol.end() , ',' , '\n' );
            string ex;
            istringstream iss( symbol );
            iss >> symbol >> ex;
            // ex get regster if have
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
            // write object code to buffer 
            buffer += out;
            // add code length 
            code_length += ( LOCTAB[_top+1]-LOCTAB[_top] );
		}
		else if( src.code[_top].ins == "BYTE" )
		{
            // [1] must be ' and [0] is C or X
			if( src.code[_top].value[1] == '\'' )
			{
                // get substring from [2] to end
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
            // code must be zeros so just add code length and do nothing
            code_length += ( LOCTAB[_top+1]-LOCTAB[_top] );
		}
		else if( src.code[_top].ins == "WORD" )
		{
            // same as byte but multi 3
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
            // likes RESB , sub LOCTAB to get length
            code_length += ( LOCTAB[_top+1]-LOCTAB[_top] );
		}
		else
		{
			cout << "Error: don't find ins: " << src.code[_top].ins << endl;
            return 0x5;
		}
		
		// when buffer is more than 30 if write in a 3 byte format , so we print it to file
		if( code_length+3 > 30 )
        {
           printf("%02X%s\n" , buffer.size()/2 , buffer.c_str() );
           buffer = "";
           code_length = 0;
        }
		
		++_top;
	}
	
	// write end record
    printf("%02X%s\n" , code_length , buffer.c_str() );
    buffer = "";
    code_length = 0;
    putchar('E');
    printf("%06X", LOCTAB[0]);
    putchar('\n');
     
     
    return 0;
}
