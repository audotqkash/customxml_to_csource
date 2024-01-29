#include <stdio.h>

void set_source_io(FILE*);
void print_code(char *format, ...);
void print_code_nonewline(char *format, ...);
void print_code_noindent(char *format, ...);
void print_code_noindent_nonewline(char *format, ...);
void print_bra_begin();
void print_bra_end();
void print_bra_end_comma();
void print_bra_end_semicolon();
void print_indent_inc();
void print_indent_dec();
