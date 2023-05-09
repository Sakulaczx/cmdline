_CMDLINE_START = __com_list_start ;
_CMDLINE_END = __com_list_end ;

SECTIONS
{
  .cmd_list : ALIGN(4)
  {
     __com_list_start = .;        
     *(.cmd_list)        
      __com_list_end = .;   
  } >FLASH
}