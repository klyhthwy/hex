#include <cstdio>
#include <cstdlib>
#include <string>
#include <fstream>
#include <iostream>
using namespace std;

typedef unsigned int address_t;

class arg_data
{
public:

    enum format_e
    {
        HEX,
        DECIMAL,
        OCTAL,
        BINARY
    };

    enum action_e
    {
        READ,
        WRITE,
        SET,
        APPEND
    };

    arg_data(void);
    ~arg_data(void);
    bool parse(int, const char **);
    const char * get_filename(void);
    format_e get_format(void);
    action_e get_action(void);
    address_t get_address(void);
    address_t get_display_size(void);
    address_t get_data_size(void);
    const char * get_data(void);

private:

    void parse_file_size(void);
    void parse_addr_size(int, const char **);
    void parse_data(int, const char **);

    string filename;
    format_e format;
    action_e action;
    address_t address;
    address_t display_size;
    address_t data_size;
    uint8_t *data;
};

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
void output_file(fstream &fin, const char *base, int field_width, address_t address, address_t size);
void output_head(int field_width);
void print_binary(unsigned char);

int main(int argc, const char **argv)
{
    arg_data args;
    fstream fio;
    string base("%02X ");
    int field_width = 2;

    if(!args.parse(argc, argv))
    {
        display_usage();
        return 1;
    }
    switch(args.get_action())
    {
        case arg_data::READ:
            fio.open(args.get_filename(), ios::in | ios::binary);
            if(fio.fail())
            {
                display_file_fail(args.get_filename());
                return 1;
            }
            switch(args.get_format())
            {
                case arg_data::HEX:
                    field_width = 2;
                    break;
                case arg_data::DECIMAL:
                    field_width = 3;
                    base = "%3u ";
                    break;
                case arg_data::OCTAL:
                    field_width = 4;
                    base = "%04o ";
                    break;
                case arg_data::BINARY:
                    field_width = 8;
                    base = "b";
                    break;
            }
            break;
        case arg_data::WRITE:
        case arg_data::SET:
            fio.open(args.get_filename(), ios::in | ios::out | ios::binary);
            if(fio.fail())
            {
                fio.open(args.get_filename(), ios::in | ios::out | ios::trunc | ios::binary);
                if(fio.fail())
                {
                    display_file_fail(args.get_filename());
                    return 1;
                }
            }
            fio.seekp(args.get_address(), ios::beg);
            fio.write(args.get_data(), args.get_data_size());
            fio.seekg(ios::beg);
            break;
        case arg_data::APPEND:
            fio.open(args.get_filename(), ios::in | ios::out | ios::app | ios::binary);
            if(fio.fail())
            {
                display_file_fail(args.get_filename());
                return 1;
            }
            fio.write(args.get_data(), args.get_data_size());
            fio.seekg(ios::beg);
            break;
    }

    output_file(fio, base.c_str(), field_width, 0, args.get_display_size());
    fio.close();

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
         << "                Usage: hex <input_file> [-<format_flag] [<start_address>] [<size>]\n"
         << "      -w        Write data to file. Currently only hex format is supported for data and addresses.\n"
         << "                Usage: hex <input_file> -w <start_address> <data_1> <data_2> ... <data_n>\n"
         << "      -a        Append data to file. Currently only hex format is supported for data and addresses.\n"
         << "                Usage: hex <input_file> -a <data_1> <data_2> ... <data_n>\n"
         << "      -s        Set data in file. Currently only hex format is supported for data and addresses.\n"
         << "                Usage: hex <input_file> -s <start_address> <data> <size>\n\n";
}

void output_file(fstream &fin, const char *base, int field_width, address_t address, address_t size)
{
    char_queue queue;
    address_t end = address + size;

    if(!fin.is_open())
    {
        return;
    }

    address &= ~static_cast<address_t>(0x0F);
    fin.seekg(address, ios::beg);

    output_head(field_width);
    for(; true; address++)
    {
        unsigned char buf;

        if(address % 0x10 == 0)
        {
            if(address > 0)
            {
                queue.output();
            }
            printf("\n%08X:  ", (address / 0x10) * 0x10);
        }

        fin.read(reinterpret_cast<char *>(&buf), 1);
        if(fin.eof() || address >= end)
        {
            queue.push(' ');
            if(fin.eof())
            {
                printf("(EOF) ");
            }
            else
            {
                printf(" (. . .) ");
            }
            for(address = (address % 0x10) + 1; address < 0x10 && address != 0; address++)
            {
                queue.push(' ');
                printf("%*c ", 8, ' ');
            }
            queue.output();
            output_head(field_width);
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

void output_head(int field_width)
{
    printf("\n\n     Hex:  ");
    for(int i = 0; i < 0x10; i++)
    {
        printf("%*X ", field_width, i);
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




arg_data::arg_data(void) :
        filename(), format(HEX), action(READ), address(0), display_size(0), data_size(0), data(NULL)
{
}

arg_data::~arg_data(void)
{
    if(data != NULL)
    {
        delete data;
    }
}

bool arg_data::parse(int argc, const char **argv)
{
    unsigned int temp;

    if(argc < 2)
    {
        return false;
    }

    filename = argv[1];
    parse_file_size();
    if(argc > 2)
    {
        if(argv[2][0] == '-')
        {
            switch(argv[2][1])
            {
                case 'h':
                case 'x':
                default:
                    format = HEX;
                    if(argc > 3)
                    {
                        parse_addr_size(argc - 3, &argv[3]);
                    }
                    break;
                case 'd':
                    format = DECIMAL;
                    if(argc > 3)
                    {
                        parse_addr_size(argc - 3, &argv[3]);
                    }
                    break;
                case 'o':
                    format = OCTAL;
                    if(argc > 3)
                    {
                        parse_addr_size(argc - 3, &argv[3]);
                    }
                    break;
                case 'b':
                    format = BINARY;
                    if(argc > 3)
                    {
                        parse_addr_size(argc - 3, &argv[3]);
                    }
                    break;
                case 'a':
                    if(argc > 3)
                    {
                        action = APPEND;
                        parse_data(argc - 3, &argv[3]);
                    }
                    else
                    {
                        return false;
                    }
                    break;
                case 's':
                    if(argc == 6)
                    {
                        action = SET;
                        sscanf(argv[3], "%x", &temp);
                        address = temp;
                        sscanf(argv[4], "%x", &temp);
                        sscanf(argv[5], "%x", &data_size);
                        data = new uint8_t[data_size];
                        memset(data, static_cast<int>(temp), static_cast<size_t>(data_size));
                    }
                    else
                    {
                        return false;
                    }
                    break;
                case 'w':
                    if(argc > 4)
                    {
                        action = WRITE;
                        sscanf(argv[3], "%x", &temp);
                        address = temp;
                        parse_data(argc - 4, &argv[4]);
                    }
                    else
                    {
                        return false;
                    }
                    break;
            }
        }
        else
        {
            parse_addr_size(argc - 2, &argv[2]);
        }
    }

    return true;
}

const char * arg_data::get_filename(void)
{
    return filename.c_str();
}

arg_data::format_e arg_data::get_format(void)
{
    return format;
}

arg_data::action_e arg_data::get_action(void)
{
    return action;
}

address_t arg_data::get_address(void)
{
    return address;
}

address_t arg_data::get_display_size(void)
{
    return display_size;
}

address_t arg_data::get_data_size(void)
{
    return data_size;
}

const char * arg_data::get_data(void)
{
    return reinterpret_cast<const char *>(data);
}

void arg_data::parse_file_size(void)
{
    fstream fio(filename.c_str(), ios::in);
    if(fio.fail())
    {
        display_size = 0;
    }
    else
    {
        fio.close();
        fio.open(filename.c_str(), ios::app);
        display_size = static_cast<address_t>(fio.tellp());
        fio.close();
    }
}

void arg_data::parse_addr_size(int argc, const char **argv)
{
    unsigned int temp;

    sscanf(argv[0], "%x", &temp);
    address = temp;
    if(argc > 1)
    {
        sscanf(argv[1], "%x", &temp);
//        display_size = temp;
    }
}

void arg_data::parse_data(int size, const char **raw_data)
{
    unsigned int temp;

    if(size > 0)
    {
        data_size = static_cast<address_t>(size);
//        display_size += static_cast<address_t>(size);
        data = new uint8_t[size];
        for(int i=0; i<size; i++)
        {
            sscanf(raw_data[i], "%x", &temp);
            data[i] = temp & 0xFF;
        }
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
