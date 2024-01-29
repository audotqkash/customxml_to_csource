#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "makedictionary.h"
#include "c_source_generator.h"
#include "common.h"
#include <libxml/tree.h>

static xmlChar* getNodeAttributes(xmlNodePtr, char*);

static void parseClass(xmlNodePtr);
static void parseMajor(xmlNodePtr);

static void print_headers();
static void print_structures();
static void print_initializer();
static bool classfunc_flags[CLASS_NUM] = {false};
static int print_sourceline(char*, char**);

void print_funcgetter();
void print_funcshowall();

static xmlChar* getNodeAttributes(xmlNodePtr nodep, char* searchStr)
{
    xmlAttrPtr attr;
    for(attr = nodep->properties; attr != NULL; attr = attr->next)
    {
        if(xmlStrcmp(attr->name, (xmlChar*)searchStr) == 0)
        {   
            return attr->children->content;
        }else;
    }

    return NULL;
}

void makeDictionary(xmlDocPtr documentp, FILE* filep)
{
    xmlXPathContextPtr context;
    xmlXPathObjectPtr classObjp;

    context = xmlXPathNewContext(documentp);
    classObjp = xmlXPathEvalExpression((xmlChar*)"//car/class", context);

    set_source_io(filep);

    print_headers();
    print_structures();

    if(xmlXPathNodeSetIsEmpty(classObjp->nodesetval))
    {
        goto END_errt_match;
    }

    for(int i = 0; i < classObjp->nodesetval->nodeNr; i++)
    {
        xmlNodePtr node = classObjp->nodesetval->nodeTab[i];

        parseClass(node); // Class nodeを渡す。
    }



END_errt_match:

    print_initializer();
    print_funcgetter();
    print_funcshowall();

    xmlXPathFreeContext(context);
    return;
}

/*! @fn parseClass(xmlNodePtr)
 *  @brief Classを探索
 *  @param[in] nodep classノードのポインタ
 */
static void parseClass(xmlNodePtr nodep)
{
    xmlNodePtr searchPtr = nodep->children;
    xmlChar*   classIdStr;
    int        classIdVal;

    classIdStr = getNodeAttributes(nodep, "id");
    if(classIdStr == NULL)
    {
        fprintf(stderr, "  [Error] class line:%d: attribute\"id\" is not specified.\n", nodep->line);
        return;
    }

    classIdVal = str2int((char*)classIdStr);
    if(classIdVal >= CLASS_NUM)
    {
        fprintf(stderr, "  [Error] class line:%d: attribute\"id\" is invalid value.\n", nodep->line);
        return;
    }


    classfunc_flags[classIdVal] = true;
    print_code("struct_function def_functions%d[] = ", classIdVal);
    print_bra_begin();/* start: def_functions[]*/

    while(searchPtr != NULL)
    {
        if(xmlStrcmp(searchPtr->name, (xmlChar*)"major") == 0)
        {
            xmlNodePtr majorPtr = searchPtr;
            parseMajor(majorPtr);
        }

        searchPtr = searchPtr->next;
    }
    print_bra_end_semicolon(); /* end : def_functions[]*/

}

/*! @fn print_sourceline(char*, char**)
 *  @param[in] str 文字列ポインタ
 *  @param[out] next 未処理文字列ポインタ
 *  @return 出力文字数
 */
static int print_sourceline(char* str, char** next)
{
    char* line = str;
    size_t line_length;
    char *end = str + strlen(str);
    char* target;

    target = strstr(line,"\n");
    if(target != NULL)
    {
        *target = '\0';
    }
        
    while(*line == ' ')
    {
        line += 1;
    }
    line_length = strlen(line);
    if((line_length > 0) && (target != line))
    {
        print_code_noindent_nonewline("%s", line);
        if(target != NULL)
        {
            print_code_noindent_nonewline("\\n");
        }else{
            print_code_noindent_nonewline("");
        }
    }

    if(target != NULL)
    {
        *target = '\n';
    }


    line += line_length + 1;

    if(line >= end)
    {
        *next = NULL;
    }else{
        *next = line;
    }

    return line_length;
}

/*! @fn parseMajor(xmlNodePtr)
 *  @brief Classを探索
 *  @param[in] nodep classノードのポインタ
 */
static void parseMajor(xmlNodePtr nodep)
{
    xmlNodePtr searchPtr = nodep->children;
    xmlChar* majorIdStr;
    unsigned int majorIdValue;

    majorIdStr = getNodeAttributes(nodep, "id");
    if(majorIdStr == NULL)
    {
        fprintf(stderr, "  [Error] major line:%d: attribute\"id\" is not specified.\n", nodep->line);
        return;
    }
    

    majorIdValue = str2int((char*)majorIdStr);
    print_bra_begin();
    print_code("0x%04x, ", majorIdValue);
    xmlChar* briefStr   = NULL;
    xmlChar* dummyCharPtr   = NULL;
    xmlChar* detailsStr = NULL;
    char* detailsptr;
    size_t result;
    while(searchPtr != NULL)
    {
        if((briefStr == NULL) && xmlStrcmp(searchPtr->name, (xmlChar*)"brief") == 0)
        {
            briefStr = searchPtr->children->content;
        }
        if((detailsStr == NULL) && xmlStrcmp(searchPtr->name, (xmlChar*)"details") == 0)
        {
            detailsStr = searchPtr->children->content;
        }

        searchPtr = searchPtr->next;
    }

    print_code_nonewline("\"");
    result = print_sourceline((char*)briefStr, (char**)&dummyCharPtr);
    print_code_noindent("\",");

    print_code("\"\\");
    detailsptr = (char*)detailsStr;
    while(detailsptr != NULL)
    {
        result = print_sourceline(detailsptr, &detailsptr);
        if(result > 0)
        {
            print_code_noindent("\\");
        }
    }
    print_code_noindent("\"");
    print_bra_end_comma();

}

void print_headers()
{
    print_code("#include <stdlib.h>");
    print_code("#include <stdio.h>");
}

void print_structures()
{
    #define CLASS_DEFINE_STRING "{%2d, NULL}"
    #define CLASS_DEFINE_STRING_TERM(term) CLASS_DEFINE_STRING term
    int i = 0;
    print_code("typedef struct class_st struct_class;");
    print_code("typedef struct function_st struct_function;");
    print_code("struct class_st");
    print_code("{");
    print_indent_inc();
    print_code("short             classids;");
    print_code("struct_function*  funcarray;");
    print_indent_dec();
    print_code("};");
    print_code("struct function_st");
    print_code("{");
    print_indent_inc();
    print_code("short funids;");
    print_code("char* brief;");
    print_code("char* details;");
    print_indent_dec();
    print_code("};");

    print_code("struct_class classes[%d] = ", CLASS_NUM);
    print_bra_begin();
    for(i = 0; i < CLASS_NUM; ++i)
    {   
        if(i == (CLASS_NUM - 1))
        {
            print_code(CLASS_DEFINE_STRING, i);
        }
        else if((i > 0) && ((i % 4) == 0))
        {
            print_code(CLASS_DEFINE_STRING_TERM(","), i);
        }else
        {
            print_code_nonewline(CLASS_DEFINE_STRING_TERM(","), i);
        }
    }
    print_bra_end_semicolon();
}

void print_initializer()
{
    int i;
    print_code("void carinfo_initialize()");
    print_bra_begin();
    for(i = 0; i < CLASS_NUM; ++i)
    {
        if(classfunc_flags[i] == true)
        {
            print_code("classes[%d].funcarray = def_functions%d;", i, i);
        }
    }
    print_bra_end();
}

void print_funcgetter()
{
    int i;
    print_code("size_t carinfo_getFunctionListsize(int classid)");
    print_bra_begin();
    print_code("switch(classid)");
    print_bra_begin();
    for(i = 0; i < CLASS_NUM; ++i)
    {
        print_code("case %d:", i);
        print_indent_inc();
        if(classfunc_flags[i] == true)
        {
            print_code("return sizeof(def_functions%d) / sizeof(struct_function);", i);
        }else{
            print_code("return 0;"); 
        }
        print_indent_dec();
    }
    print_bra_end();
    print_code("return 0;"); 
    print_bra_end();


    print_code("char* carinfo_getdetail(int classid, short funcid)");
    print_bra_begin();
    print_code("size_t entrynum = carinfo_getFunctionListsize(classid);");
    print_code("int i;");
    print_code("if(entrynum == 0)");
    print_bra_begin();
    print_code("return NULL;");
    print_bra_end();
    print_code("");
    print_code("for(i = 0; i < entrynum; ++i)");
    print_bra_begin();
    print_code("if(classes[classid].funcarray[i].funids == funcid)");
    print_bra_begin();
    print_code("return classes[classid].funcarray[i].details;");
    print_bra_end();
    print_bra_end();
    print_code("return NULL;");
    print_bra_end();
}

void print_funcshowall()
{
    print_code("void carinfo_showall(int classid, FILE* outp)");
    print_bra_begin();
    print_code("size_t entrynum = carinfo_getFunctionListsize(classid);");
    print_code("int i;");
    print_code("if(entrynum == 0)");
    print_bra_begin();
    print_code("fprintf(outp, \" No Function Data\\n\");");
    print_bra_end();
    print_code("");
    print_code("for(i = 0; i < entrynum; ++i)");
    print_bra_begin();
    print_code("fprintf(outp, \"0x%%04x  %%s\\n\", ");
    print_code("(unsigned short)classes[classid].funcarray[i].funids,");
    print_code("                classes[classid].funcarray[i].brief);");
    print_bra_end();
    print_bra_end();
}