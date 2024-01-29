#include <stdio.h>
#include <string.h>
#include "common.h"
#include "dictionary.h"
#include "makedictionary.h"
#include "makeanaryzer.h"

#define TEST_ON

const char* xmlfile = "/mnt/c/Users/Takashi/Documents/VSCode/C/parseXml/sample.xml";

extern void carinfo_initialize();
extern void carinfo_showall(int, FILE*);
extern char* carinfo_getdetail(int, short);

void carinfo_showClassList(int classid)
{
#ifdef TEST_ON
    size_t e_num = (size_t)carinfo_getFunctionListsize(classid);
    char* detailStr;

    fprintf(stderr, "[ class %2d ] %ld entries\n", classid, e_num);
    if(e_num == 0)
    {
        goto END_getclass;
    }

    carinfo_showall(classid, stderr);
    detailStr = carinfo_getdetail(classid, 0x0000);
    while(detailStr != NULL)
    {
        fprintf(stderr, "        ");
        print_line(detailStr, stderr, &detailStr);
        fprintf(stderr, "\n");
    }
END_getclass:
    fprintf(stderr, "---------------------------\n");
#endif
    return;
}


int main(int argn, char** argv)
{    
    /* xml */
    xmlDocPtr xmlDoc;

    fprintf(stderr, "File open : %s\n", xmlfile);

    xmlDoc = xmlReadFile(xmlfile, NULL, 0);
    if(xmlDoc == NULL)
    {
        fprintf(stderr, " [Error] Failed to parse xml.\n");
    }
    else
    {
#if 0
        xmlDocDump(stdout, xmlDoc);
#endif
    }

    /* 初期解析用辞書 */
    FILE* out1p = fopen("dictionary1.c", "w");
    makeDictionary(xmlDoc, out1p);
    fclose(out1p);

    /* 詳細解析用辞書 */
    FILE* out2p = fopen("dictionary2.c", "w");
    makeAnalyzer(xmlDoc, out2p);
    fclose(out2p);
    
    xmlFreeDoc(xmlDoc);
    xmlCleanupParser();

#ifdef TEST_ON
    carinfo_initialize();
    carinfo_showClassList(1);
    carinfo_showClassList(2);
    carinfo_showClassList(3);
    carinfo_showClassList(10);
    carinfo_showClassList(11);
    carinfo_showClassList(12);

    unsigned long data = 0x00000002;
    professional_analyse(1, 0x0000, (unsigned char*)&data);
#endif

    return 0;
}
