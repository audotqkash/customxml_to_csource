#include <stdarg.h>
#include "c_source_generator.h"

static int bra_depth = 0;
static FILE* SOURCE_OUTPUT = NULL;

void set_source_io(FILE* filep)
{
    if(filep == NULL)
    {
        SOURCE_OUTPUT = stdout;
        return;
    }

    SOURCE_OUTPUT = filep;
}

void print_indent(int depth)
{
    int i;
    for(i = 0; i < depth; ++i)
    {
        fprintf(SOURCE_OUTPUT, "  ");
    }
}

void print_code(char *format, ...)
{
    va_list ap;
    va_start(ap, format);

    print_indent(bra_depth);
    vfprintf(SOURCE_OUTPUT, format, ap);
    fprintf(SOURCE_OUTPUT, "\n");
}
void print_code_noindent(char *format, ...)
{
    va_list ap;
    va_start(ap, format);

    vfprintf(SOURCE_OUTPUT, format, ap);
    fprintf(SOURCE_OUTPUT, "\n");
}

void print_code_noindent_nonewline(char *format, ...)
{
    va_list ap;
    va_start(ap, format);

    vfprintf(SOURCE_OUTPUT, format, ap);
}

void print_code_nonewline(char *format, ...)
{
    va_list ap;
    va_start(ap, format);

    print_indent(bra_depth);
    vfprintf(SOURCE_OUTPUT, format, ap);
}

void print_bra_begin()
{
    print_indent(bra_depth);
    bra_depth += 1;

    fprintf(SOURCE_OUTPUT, "{\n");
}

void print_bra_end()
{    
    bra_depth -= 1;
    print_indent(bra_depth);
    fprintf(SOURCE_OUTPUT, "}\n");
}

void print_bra_end_semicolon()
{    
    bra_depth -= 1;
    print_indent(bra_depth);
    fprintf(SOURCE_OUTPUT, "};\n");
}

void print_bra_end_comma()
{    
    bra_depth -= 1;
    print_indent(bra_depth);
    fprintf(SOURCE_OUTPUT, "},\n");
}

void print_indent_inc()
{
    bra_depth += 1;
}

void print_indent_dec()
{
    bra_depth -= 1;
}
