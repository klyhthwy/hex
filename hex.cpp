#include <cstdio>
#include <fstream>
#include <iostream>
using namespace std;

void display_file_fail(const char *);
char display_txt(unsigned char);
void display_usage(void);
void output_file(fstream &, const char *);
void output_head();
void output_queue(char *);
void print_binary(unsigned char);
void write_data(fstream &fio, int, const char **);

int main(int argc, const char **argv)
{
    unsigned int addr;

    if(argc < 2)
    {
        display_usage();
        return 0;
    }

    fstream fio;

    if( argc > 2 )
    {
        if(argv[2][0] != '-')
        {
            display_usage();
            return 0;
        }
        switch(argv[2][1])
        {
            case 'a':
                fio.open(argv[1], ios::in | ios::out | ios::app | ios::binary);
                if(fio.fail())
                {
                    display_file_fail(argv[1]);
                    return 0;
                }
                write_data(fio, argc-3, &argv[3]);
                fio.seekg(ios::beg);
                output_file(fio, "      %02X ");
                fio.close();
                break;
            case 'b':
                fio.open(argv[1], ios::in | ios::binary);
                if(fio.fail())
                {
                    display_file_fail(argv[1]);
                    return 0;
                }
                output_file(fio, "b");
                fio.close();
                break;
            case 'd':
                fio.open(argv[1], ios::in | ios::binary);
                if(fio.fail())
                {
                    display_file_fail(argv[1]);
                    return 0;
                }
                output_file(fio, "%8u ");
                fio.close();
                break;
            case 'o':
                fio.open(argv[1], ios::in | ios::binary);
                if(fio.fail())
                {
                    display_file_fail(argv[1]);
                    return 0;
                }
                output_file(fio, "     %03o ");
                fio.close();
                break;
            case 'w':
                if(argc < 5)
                {
                    display_usage();
                    return 0;
                }
                fio.open(argv[1], ios::in | ios::out | ios::binary);
                if(fio.fail())
                {
                    fio.open(argv[1], ios::in | ios::out | ios::trunc | ios::binary);
                    if(fio.fail())
                    {
                        display_file_fail(argv[1]);
                        return 0;
                    }
                }
                sscanf(argv[3], "%x", &addr);
                fio.seekp(addr, ios::beg);
                write_data(fio, argc-4, &argv[4]);
                fio.seekg(ios::beg);
                output_file(fio, "      %02X ");
                fio.close();
                break;
            case 'h':
            case 'x':
            default:
                fio.open(argv[1], ios::in | ios::binary);
                if(fio.fail())
                {
                    display_file_fail(argv[1]);
                    return 0;
                }
                output_file(fio, "      %02X ");
                fio.close();
                break;
        }
    }
    else
    {
        fio.open(argv[1], ios::in | ios::binary);
        if(fio.fail())
        {
            display_file_fail(argv[1]);
            return 0;
        }
        output_file(fio, "      %02X ");
        fio.close();
    }

    return 0;
}

void display_file_fail(const char *filename)
{
    printf("Failed to open file \"%s\"\n", filename);
}

char display_txt(unsigned char chr)
{
    if(chr > 0x20 && chr < 0x7F)
    {
        return (char)chr;
    }
    else
    {
        return 0x20;
    }
}

/*
 *  Display tool usage.
 */
void display_usage(void)
{
    cout << "Command line hex editor.\n\n"
         << "Usage: hex <input_file> [-<flags>] [<start_address>] [<end_address>] [<data>]\n"
         << "    flags:\n"
         << "      -x, -h    Display data in hexadecimal format (default).\n"
         << "      -d        Display data in decimal format.\n"
         << "      -o        Display data in octal format.\n"
         << "      -b        Display data in binary format.\n"
         << "      -w        Write data to file. Currently only hex format is supported for data and addresses.\n"
         << "                Usage: hex <input_file> -w <start_address> <data_1> <data_2> ... <data_n>\n"
         << "      -a        Append data to file. Currently only hex format is supported for data and addresses.\n"
         << "                Usage: hex <input_file> -a <data_1> <data_2> ... <data_n>\n"
         << "      -r        Read data from file. Currently not supported.\n"
         << "                Usage: hex <input_file> -r[<flags>] <start_address> <end_address>\n\n";
}

void output_file(fstream &fin, const char *base)
{
    int i;
    char queue[0x11];

    if(!fin.is_open())
    {
        return;
    }

    output_head();
    queue[0] = queue[0x10] = '\0';
    for(i = 0; true; i++)
    {
        unsigned char buf;

        fin.read(reinterpret_cast<char *>(&buf), 1);
        if(fin.eof())
        {
            for(i %= 0x10; i < 0x10 && i != 0; i++)
            {
                queue[i] = ' ';
                printf("%*c ", 8, ' ');
            }
            output_queue(queue);
            output_head();
            return;
        }

        if(i % 0x10 == 0)
        {
            if(i > 0)
            {
                output_queue(queue);
            }
            printf("\n%08X:  ", (i / 0x10) * 0x10);
        }

        queue[i % 0x10] = display_txt(buf);
        if(base[0] != 'b')
        {
            printf(base, buf);
        }
        else
        {
            print_binary(buf);
        }
    }
}

void output_head()
{
    printf("\n\n     Hex:  ");
    for(int i = 0; i < 0x10; i++)
    {
        printf("%8X ", i);
    }
    printf("\n");
}

void output_queue(char queue[])
{
    printf("   ");
    for(int i = 0; i < 0x10; i++)
    {
        printf("%c", queue[i]);
    }
}

void print_binary(unsigned char buf)
{
    for(int i = 0; i < 8; i++)
    {
        if((1 << (7 - i)) & buf)
        {
            printf("1");
        }
        else
        {
            printf("0");
        }
    }
    printf(" ");
}

void write_data(fstream &fio, int size, const char **args)
{
    unsigned int val;
    unsigned char chr;

    for(int i = 0; i < size; i++)
    {
        sscanf(args[i], "%x", &val);
        chr = val & 0xFF;
        fio.write(reinterpret_cast<char *>(&chr), 1);
    }
}
