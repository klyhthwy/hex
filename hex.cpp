#include <cstdio>
#include <fstream>
#include <iostream>
using namespace std;

class char_queue
{
public:

    char_queue(void);
    ~char_queue(void);

    void push(char);
    void output(void);

private:

    static const int SIZE;

    char * init_queue(void);
    void flush(void);
    int pix;
    char *queue;
};

void display_file_fail(const char *);
char display_txt(unsigned char);
void display_usage(void);
void output_file(fstream &, const char *, unsigned int);
void output_head();
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

    if(argc > 2)
    {
        if(argc > 3)
        {

        }
        else
        {

        }
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
                write_data(fio, argc - 3, &argv[3]);
                fio.seekg(ios::beg);
                output_file(fio, "      %02X ", 0);
                fio.close();
                break;
            case 'b':
                fio.open(argv[1], ios::in | ios::binary);
                if(fio.fail())
                {
                    display_file_fail(argv[1]);
                    return 0;
                }
                output_file(fio, "b", 0);
                fio.close();
                break;
            case 'd':
                fio.open(argv[1], ios::in | ios::binary);
                if(fio.fail())
                {
                    display_file_fail(argv[1]);
                    return 0;
                }
                output_file(fio, "%8u ", 0);
                fio.close();
                break;
            case 'o':
                fio.open(argv[1], ios::in | ios::binary);
                if(fio.fail())
                {
                    display_file_fail(argv[1]);
                    return 0;
                }
                output_file(fio, "     %03o ", 0);
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
                write_data(fio, argc - 4, &argv[4]);
                fio.seekg(ios::beg);
                output_file(fio, "      %02X ", 0);
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
                output_file(fio, "      %02X ", 0);
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
        output_file(fio, "      %02X ", 0);
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
        return static_cast<char>(chr);
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
         << "Usage: hex <input_file> [-<flags>] [<start_address>] [<size>] [<data>]\n"
         << "    flags:\n"
         << "      -x, -h    Display data in hexadecimal format (default).\n"
         << "      -d        Display data in decimal format.\n"
         << "      -o        Display data in octal format.\n"
         << "      -b        Display data in binary format.\n"
         << "                Usage: hex <input_file> [-<format_flag] [<start_address>]\n"
         << "      -w        Write data to file. Currently only hex format is supported for data and addresses.\n"
         << "                Usage: hex <input_file> -w <start_address> <data_1> <data_2> ... <data_n>\n"
         << "      -a        Append data to file. Currently only hex format is supported for data and addresses.\n"
         << "                Usage: hex <input_file> -a <data_1> <data_2> ... <data_n>\n"
         << "      -s        Set data in file. Currently only hex format is supported for data and addresses.\n"
         << "                Usage: hex <input_file> -s <start_address> <size> <data>\n\n";
}

void output_file(fstream &fin, const char *base, unsigned int address)
{
    const int MAX_SIZE = 0x200;
    unsigned int i;
    char_queue queue;

    if(!fin.is_open())
    {
        return;
    }

    address &= ~static_cast<unsigned int>(0x0F);
    fin.seekg(address, ios::beg);

    output_head();
    for(i = 0; true; i++)
    {
        unsigned char buf;

        if(i % 0x10 == 0)
        {
            if(i > 0)
            {
                queue.output();
            }
            printf("\n%08X:  ", (i / 0x10) * 0x10);
        }

        fin.read(reinterpret_cast<char *>(&buf), 1);
        if(fin.eof() || i >= MAX_SIZE)
        {
            queue.push(' ');
            if(fin.eof())
            {
                printf("   (EOF) ");
            }
            else
            {
                printf(" (. . .) ");
            }
            for(i = (i % 0x10) + 1; i < 0x10 && i != 0; i++)
            {
                queue.push(' ');
                printf("%*c ", 8, ' ');
            }
            queue.output();
            output_head();
            return;
        }

        queue.push(display_txt(buf));
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




char_queue::char_queue(void) :
        pix(0), queue(init_queue())
{
}

char_queue::~char_queue(void)
{
    delete queue;
}

void char_queue::push(char push_char)
{
    if(pix >= SIZE)
    {
        return;
    }
    queue[pix++] = push_char;
}

void char_queue::output(void)
{
    printf("   %s", queue);
    flush();
}

const int char_queue::SIZE = 0x11;

char * char_queue::init_queue(void)
{
    char *new_queue = new char[SIZE];
    memset(new_queue, 0, SIZE);
    return new_queue;
}

void char_queue::flush(void)
{
    pix = 0;
    memset(queue, 0, SIZE);
}
