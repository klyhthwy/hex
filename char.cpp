#include <iostream>
#include <iomanip>
#include <cstdlib>
using namespace std;

#define SIZE 4

void chr(int, char[]);
void copy(char[], const char *);
void copy(char[], char);
void output_all();
void output_chr(int);
void output_cde(char);

int main(int argc, char **argv)
{
    switch(argc)
    {
        case 1:
            output_all();
            break;
        case 2:
            output_chr(atoi(argv[1]));
            break;
        case 3:
        default:
            output_chr(atoi(argv[1]));
            output_cde(argv[2][0]);
    }
}

void chr(int i, char str[])
{
    switch(i)
    {
        case 0:
            copy(str, "NL");
            break;
        case 7:
            cout << char(7);
            copy(str, "BL");
            break;
        case 8:
            copy(str, "BS");
            break;
        case 9:
            copy(str, "TB");
            break;
        case 10:
            copy(str, "LF");
            break;
        case 13:
            copy(str, "CR");
            break;
        default:
            copy(str, char(i));
    }
}

void copy(char str[], const char *c_str)
{
    for(int i = 0; i < SIZE; i++)
    {
        str[i] = c_str[i];
    }
}

void copy(char str[], char chr)
{
    str[0] = chr;
    str[1] = '\0';
}

void output_all()
{
    char str[SIZE];

    cout << "     " << left << hex << uppercase;
    for(int i = 0; i < 0x10; i++)
    {
        cout << setw(SIZE) << i;
    }
    for(int i = 0x20; i < 0x80; i++)
    {
        if(i % 0x10 == 0)
            cout << "\n\n" << setw(2) << setfill('0')
                 << i / 0x10 * 0x10
                 << ":  " << setfill(' ');
        chr(i, str);
        cout << setw(SIZE) << str;
    }
    cout << endl;
}

void output_chr(int code)
{
    char str[SIZE];

    chr(code, str);
    cout << "\n\t" << str << endl;
}

void output_cde(char chr)
{
    cout << "\n\t0x" << hex << uppercase << (int)chr << endl;
}