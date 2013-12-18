#include "input.h"

Minput :: Minput()
{    
    ;
}

Minput :: Minput(string file_path)
{
    fs.open( file_path.c_str() , ios::in );
}

bool Minput :: chk_file()
{
    return fs.is_open();
}

bool Minput :: getCode()
{
    if( false == chk_file() )
        return false;
    
    code.clear();

    while( fs.good() )
    {
        string line;
        getline( fs , line );
        replace( line.begin() , line.end() , '\t' , '\n'); 
        
        istringstream iss ( line );
        
        string a,b,c,d;
        iss >> a >> b >> c;


        if( c == "" )
        {
            c = b;
            b = a;
            a = "";
        }
        
        //iss >> d;

        //cout << a << "\t" << b << "\t" << c << "\t" << d << endl;
        if( a[0] != '.' && b[0] != '.' && c[0] != '.' )
        {
            string t = c;
            replace( t.begin() , t.end() , ',' , '\n');
            istringstream is( t );
            iss >> c >> d;
            
            Minstru tmp( a ,b ,c ,d );
            code.push_back( tmp );
        }
    }

    return true;
}
