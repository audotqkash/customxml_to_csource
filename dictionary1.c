#include <stdlib.h>
#include <stdio.h>
typedef struct class_st struct_class;
typedef struct function_st struct_function;
struct class_st
{
  short             classids;
  struct_function*  funcarray;
};
struct function_st
{
  short funids;
  char* brief;
  char* details;
};
struct_class classes[24] = 
{
  { 0, NULL},  { 1, NULL},  { 2, NULL},  { 3, NULL},  { 4, NULL},
  { 5, NULL},  { 6, NULL},  { 7, NULL},  { 8, NULL},
  { 9, NULL},  {10, NULL},  {11, NULL},  {12, NULL},
  {13, NULL},  {14, NULL},  {15, NULL},  {16, NULL},
  {17, NULL},  {18, NULL},  {19, NULL},  {20, NULL},
  {21, NULL},  {22, NULL},  {23, NULL}
};
struct_function def_functions1[] = 
{
  {
    0x0000, 
    "This is Class 1 Major 0x0000",
    "\
1. エンジンタイプ #1\n\
2. シャーシタイプ #1\n\
3. サスペンションタイプ #1\n\
"
  },
  {
    0xffff, 
    "This is errt1-ffff",
    "\
"
  },
};
struct_function def_functions10[] = 
{
  {
    0x0000, 
    "This is Class 10 Major 0x0000",
    "\
"
  },
  {
    0xffff, 
    "This is errtA-ffff",
    "\
"
  },
};
struct_function def_functions11[] = 
{
};
void carinfo_initialize()
{
  classes[1].funcarray = def_functions1;
  classes[10].funcarray = def_functions10;
  classes[11].funcarray = def_functions11;
}
size_t carinfo_getFunctionListsize(int classid)
{
  switch(classid)
  {
    case 0:
      return 0;
    case 1:
      return sizeof(def_functions1) / sizeof(struct_function);
    case 2:
      return 0;
    case 3:
      return 0;
    case 4:
      return 0;
    case 5:
      return 0;
    case 6:
      return 0;
    case 7:
      return 0;
    case 8:
      return 0;
    case 9:
      return 0;
    case 10:
      return sizeof(def_functions10) / sizeof(struct_function);
    case 11:
      return sizeof(def_functions11) / sizeof(struct_function);
    case 12:
      return 0;
    case 13:
      return 0;
    case 14:
      return 0;
    case 15:
      return 0;
    case 16:
      return 0;
    case 17:
      return 0;
    case 18:
      return 0;
    case 19:
      return 0;
    case 20:
      return 0;
    case 21:
      return 0;
    case 22:
      return 0;
    case 23:
      return 0;
  }
  return 0;
}
char* carinfo_getdetail(int classid, short funcid)
{
  size_t entrynum = carinfo_getFunctionListsize(classid);
  int i;
  if(entrynum == 0)
  {
    return NULL;
  }
  
  for(i = 0; i < entrynum; ++i)
  {
    if(classes[classid].funcarray[i].funids == funcid)
    {
      return classes[classid].funcarray[i].details;
    }
  }
  return NULL;
}
void carinfo_showall(int classid, FILE* outp)
{
  size_t entrynum = carinfo_getFunctionListsize(classid);
  int i;
  if(entrynum == 0)
  {
    fprintf(outp, " No Function Data\n");
  }
  
  for(i = 0; i < entrynum; ++i)
  {
    fprintf(outp, "0x%04x  %s\n", 
    (unsigned short)classes[classid].funcarray[i].funids,
                    classes[classid].funcarray[i].brief);
  }
}
