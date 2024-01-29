#include "common.h"
#include <string.h>
#include <stdlib.h>

/*! @fn print_line(char*, char**)
 *  @param[in] str   文字列ポインタ
 *  @param[in] outp  出力先ファイルポインタ
 *  @param[out] next 未処理文字列ポインタ
 *  @return 出力文字数
 */
int print_line(char* str, FILE* outp, char** next)
{
    char* line = str;
    size_t line_length;
    char *end = str + strlen(str);
    char* target;

    size_t tmplen = strlen(str);
    char *tmpstr;

    target = strstr(line,"\n");
    if(target != NULL)
    {
        tmplen = target - line;
    }
    
    tmpstr = calloc(tmplen + 1, sizeof(char));
    if(tmpstr == NULL)
    {
        fprintf(stderr, " print_line : lack of memory\n");
        *next = NULL;
        return 0;
    }
    strncpy(tmpstr, line, tmplen);

    while(*line == ' ')
    {
        line += 1;
    }

    line_length = strlen(&tmpstr[(int)(line-str)]);

    if((line_length > 0) && (target != line))
    {
        fprintf(outp, "%s", &tmpstr[(int)(line-str)]);
        if(target != NULL)
        {
//            fprintf(outp, "\\n");
        }else{
//            fprintf(outp, "");
        }
    }

    free(tmpstr);


    line += line_length + 1;

    if(line >= end)
    {
        *next = NULL;
    }else{
        *next = line;
    }

    return line_length;
}

/*! @fn str2int(const char*)
 *  @brief 文字列を数値変換する
 *  @details 10進数と16進数に対応する。
 *  ####は10進数、0x####は16進数として扱う。
 */
unsigned long str2int(const char* str)
{
    unsigned long value = 0;
    int i;
    int isHead = 1;

    if(strstr(str, "0x") == NULL)
    {
        /* 10進数 */
        for(i = 0; i < strlen(str); ++i)
        {
            if(str[i] == ' ')
            {
                if(isHead == 1)
                {
                    continue;
                }else{
                    break;
                }
            }

            isHead = 0;
            if(str[i] >= '0' && str[i] <= '9')
            {
                value *= 10;
                value += (int)str[i] - (int)('0');
            }else{
                fprintf(stderr, " str2int Error (char = %c)\n", str[i]);
                break;
            }
        }
    }else{
        /* 16進数 */
        unsigned long startpos = (unsigned long)strstr(str, "0x") - (unsigned long)str;
        for(i = (int)startpos + 2; i < strlen(str); ++i)
        {
            if(str[i] == ' ')
            {
                    break;
            }

            if(str[i] >= '0' && str[i] <= '9')
            {
                value *= 16;
                value += (int)str[i] - (int)('0');
            }
            else if(str[i] >= 'a' && str[i] <= 'f')
            {
                value *= 16;
                value += (int)str[i] - (int)('a') + 10;
            }
            else if(str[i] >= 'A' && str[i] <= 'F')
            {
                value *= 16;
                value += (int)str[i] - (int)('A') + 10;
            }
            else
            {
                fprintf(stderr, " str2int Error (char = %c)\n", str[i]);
                break;
            }
        }
    }

//    fprintf(stderr, "str2int : value = %x\n", value);
    return value;
}

