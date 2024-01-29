#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "makedictionary.h"
#include "c_source_generator.h"
#include "common.h"
#include <libxml/tree.h>

static xmlChar* getNodeAttributes(xmlNodePtr, char*);

struct dataattr
{
    unsigned char offset;
    unsigned char length;
    unsigned long mask;
    unsigned long value;
};

static void parseClass(xmlNodePtr);
static void parseMajor(xmlNodePtr);
static void parseValues(xmlNodePtr);
static void parseNote(xmlNodePtr, struct dataattr*);

static bool classfunc_flags[CLASS_NUM] = {false};

static int print_sourceline(char*, char**);

#define ARGNAME_CLASSID    "classid"
#define ARGNAME_FUNCTIONID "funcid"



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

void makeAnalyzer(xmlDocPtr documentp, FILE* filep)
{
    xmlXPathContextPtr context;
    xmlXPathObjectPtr classObjp;

    set_source_io(filep);

    context = xmlXPathNewContext(documentp);
    classObjp = xmlXPathEvalExpression((xmlChar*)"//car/class", context);

    print_code("void professional_analyse(short %s, short %s, unsigned char data[8])", 
                    ARGNAME_CLASSID,
                    ARGNAME_FUNCTIONID);
    print_bra_begin();

    print_code("switch(%s)", ARGNAME_CLASSID);
    print_bra_begin();
    for(int i = 0; i < classObjp->nodesetval->nodeNr; i++)
    {
        xmlNodePtr node = classObjp->nodesetval->nodeTab[i];

        parseClass(node); // Class nodeを渡す。
        /*
        fprintf(filep, "%s ", node->name);
        char* class_name = getNodeAttributes(node, "name");
        fprintf(stderr, " %s\n", class_name);
        */
    }
    print_bra_end();
    print_bra_end();

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
    print_code("case %2d:", classIdVal);
    print_indent_inc();

    print_code("switch(%s)", ARGNAME_FUNCTIONID);
    print_bra_begin();
    while(searchPtr != NULL)
    {
        if(xmlStrcmp(searchPtr->name, (xmlChar*)"major") == 0)
        {
            xmlNodePtr majorPtr = searchPtr;
            parseMajor(majorPtr);
        }

        searchPtr = searchPtr->next;
    }
    print_code("break;");
    print_bra_end();
    print_indent_dec();

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
    print_code("case 0x%04x:", majorIdValue);
    print_indent_inc();

    while(searchPtr != NULL)
    {
        if(xmlStrcmp(searchPtr->name, (xmlChar*)"values") == 0)
        {
            xmlNodePtr ValuesPtr = searchPtr;
            parseValues(ValuesPtr);
        }

        searchPtr = searchPtr->next;
    }
    print_code("break;");
    print_indent_dec();

}

static void parseValues(xmlNodePtr nodep)
{
    xmlNodePtr searchPtr = nodep->children;
    struct dataattr condition;
    
    while(searchPtr != NULL)
    {
        if(xmlStrcmp(searchPtr->name, (xmlChar*)"value") == 0)
        {
            xmlNodePtr ValuePtr = searchPtr;
            xmlChar *attr_offsetStr;
            xmlChar *attr_lengthStr;
            xmlChar *attr_maskStr;
            attr_offsetStr = getNodeAttributes(ValuePtr, "offset");
            attr_lengthStr = getNodeAttributes(ValuePtr, "length");
            attr_maskStr = getNodeAttributes(ValuePtr, "mask");
            condition.offset = str2int((char*)attr_offsetStr);
            condition.length = str2int((char*)attr_lengthStr);
            condition.mask   = str2int((char*)attr_maskStr);
            print_bra_begin();
            parseNote(searchPtr->children, &condition);
            print_bra_end();
        }
        
        searchPtr = searchPtr->next;
    }
}


static void parseNote(xmlNodePtr searchPtr, struct dataattr *condition)
{

    while(searchPtr != NULL)
    {
        if(xmlStrcmp(searchPtr->name, (xmlChar*)"note") == 0)
        {
            xmlChar *attr_valueStr;
            attr_valueStr = getNodeAttributes(searchPtr, "value");
            if(attr_valueStr != NULL)
            {
                char* basetypename[9] = 
                {
                    "error", 
                    "unsigned char", "unsigned short", "unsigned int", "unsigned int",
                    "unsigned long", "unsigned long", "unsigned long", "unsigned long" 
                };
                unsigned long basemask[9] = 
                {
                    0x0000000000000000, 
                    0x00000000000000FF, 0x000000000000FFFF, 0x0000000000FFFFFF, 0x00000000FFFFFFFF,
                    0x000000FFFFFFFFFF,  0x0000FFFFFFFFFFFF,  0x00FFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF
                };

                condition->value = str2int((char*)attr_valueStr);
                if(condition->length > 8)
                {
                    condition->length = 8; 
                }
                print_code("if( (((*(%s*)&data[%d]) & 0x%X) & 0x%08x) == 0x%x )", 
                            basetypename[condition->length], 
                            condition->offset, basemask[condition->length], 
                            condition->mask, condition->value);
                print_bra_begin();
                print_code("printf(\"             %%08x %%08x  : %s \\n\", \n\
                        *(unsigned int*)&data[0], \n\
                        *(unsigned int*)&data[4]); \n", 
                                                searchPtr->children->content);
                print_bra_end();
            }
        }
        searchPtr = searchPtr->next;
    }
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