void professional_analyse(short classid, short funcid, unsigned char data[8])
{
  switch(classid)
  {
    case  1:
      switch(funcid)
      {
        case 0x0000:
          {
            if( (((*(unsigned char*)&data[0]) & 0xFF) & 0x000000ff) == 0x1 )
            {
              printf("             %08x %08x  : C1M0 ------01 -------- \n", 
                        *(unsigned int*)&data[0], 
                        *(unsigned int*)&data[4]); 

            }
            if( (((*(unsigned char*)&data[0]) & 0xFF) & 0x000000ff) == 0x2 )
            {
              printf("             %08x %08x  : C1M0 ------02 -------- \n", 
                        *(unsigned int*)&data[0], 
                        *(unsigned int*)&data[4]); 

            }
          }
          {
            if( (((*(unsigned int*)&data[4]) & 0xFFFFFFFF) & 0xffffffff) == 0x1 )
            {
              printf("             %08x %08x  : C1M0 -------- 000000001 \n", 
                        *(unsigned int*)&data[0], 
                        *(unsigned int*)&data[4]); 

            }
            if( (((*(unsigned int*)&data[4]) & 0xFFFFFFFF) & 0xffffffff) == 0x2 )
            {
              printf("             %08x %08x  : C1M0 -------- 000000002 \n", 
                        *(unsigned int*)&data[0], 
                        *(unsigned int*)&data[4]); 

            }
          }
          break;
        case 0xffff:
          break;
        break;
      }
    case 10:
      switch(funcid)
      {
        case 0x0000:
          break;
        case 0xffff:
          {
            if( (((*(unsigned char*)&data[6]) & 0xFF) & 0x0000000f) == 0x1 )
            {
              printf("             %08x %08x  : function 0000-1 \n", 
                        *(unsigned int*)&data[0], 
                        *(unsigned int*)&data[4]); 

            }
            if( (((*(unsigned char*)&data[6]) & 0xFF) & 0x0000000f) == 0x2 )
            {
              printf("             %08x %08x  : function 0000-2 \n", 
                        *(unsigned int*)&data[0], 
                        *(unsigned int*)&data[4]); 

            }
          }
          {
            if( (((*(unsigned char*)&data[7]) & 0xFF) & 0x000000f0) == 0x6 )
            {
              printf("             %08x %08x  : ここにエラーがあります。問題を解決するには～を実行してください。 \n", 
                        *(unsigned int*)&data[0], 
                        *(unsigned int*)&data[4]); 

            }
            if( (((*(unsigned char*)&data[7]) & 0xFF) & 0x000000f0) == 0x90 )
            {
              printf("             %08x %08x  : ここにエラーがあります。 \n", 
                        *(unsigned int*)&data[0], 
                        *(unsigned int*)&data[4]); 

            }
          }
          break;
        break;
      }
    case 11:
      switch(funcid)
      {
        break;
      }
  }
}
