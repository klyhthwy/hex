#include <cstdio>
#include <fstream>
#include <iostream>
using namespace std;

char display_txt( unsigned char );
void output_file( fstream &, const char * );
void output_head();
void output_queue( char * );
void print_binary( unsigned char );

int main( int argc, char **argv )
{
    unsigned int addr;
    unsigned char val;

    if( argc < 3 )
    {
        printf( "\n\t** Not enough Arguments **\n" );
        return 0;
    }

    fstream fio( argv[1], ios::in | ios::out | ios::binary );
    if( fio.fail() )
    {
        printf( "\n\t** File Error **\n" );
        return 0;
    }

    switch( argv[2][0] )
    {
        case '0':
            if( argc < 4 )
            {
                printf( "\n\t** Not enough Arguments **\n" );
                fio.close();
                return 0;
            }
            sscanf( argv[2], "%x", &addr );
            fio.seekp( addr, ios::beg );
            for( int i=3; i<argc; i++ )
            {
                sscanf( argv[i], "%x", (unsigned int *)&val );
                fio.write( reinterpret_cast<char*>(&val), 1 );
            }
            break;
        case 'b':
        case 'B':
            output_file( fio, "b" );
            break;
        case 'd':
        case 'D':
            output_file( fio, "%8u " );
            break;
        case 'o':
        case 'O':
            output_file( fio, "     %03o " );
            break;
        case 'h':
        case 'H':
        case 'x':
        case 'X':
        default:
            output_file( fio, "      %02X " );
    }

    fio.close();
    return 0;
}

char display_txt( unsigned char chr )
{
    if( chr > 0x0D )
        return (char)chr;
    else if( chr == 0x0A )
        return 0x19;
    else if( chr == 0x0D )
        return 0x11;
    else
        return 0x20;
}

void output_file( fstream &fin, const char *base )
{
    int i;
    char queue[0x11];

    output_head();
    queue[0] = queue[0x10] = '\0';
    for( i=0; true; i++ )
    {
        unsigned char buf;

        fin.read( reinterpret_cast<char*>(&buf), 1 );
        if( fin.eof() )
        {
            for( i %= 0x10; i<0x10 && i!=0; i++ )
            {
                queue[i] = ' ';
                printf( "%*c ", 8, ' ' );
            }
            output_queue( queue );
            output_head();
            return;
        }

        if( i % 0x10 == 0 )
        {
            if( i>0 )
                output_queue( queue );
            printf( "\n%08X:  ", (i / 0x10) * 0x10 );
        }

        queue[i%0x10] = display_txt( buf );
        if( base[0] != 'b' )
            printf( base, buf );
        else
            print_binary( buf );
    }
}

void output_head()
{
    printf( "\n\n     Hex:  " );
    for( int i=0; i<0x10; i++ )
        printf( "%8X ", i );
    printf( "\n" );
}

void output_queue( char queue[] )
{
    printf( "   " );
    for( int i=0; i<0x10; i++ )
        printf( "%c", queue[i] );
}

void print_binary( unsigned char buf )
{
    for( int i=0; i<8; i++ )
    {
        if( ( 1 << (7-i) ) & buf )
            printf( "1" );
        else
            printf( "0" );
    }
    printf( " " );
}