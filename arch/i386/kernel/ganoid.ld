ENTRY(start)

SECTIONS 
{
	.text 0x100000 :  
	{ 
	  code = .; _code = .; __code = .;__start_ = .;
	  boot.o(.text)
	  *(.text)
	  . = ALIGN(4096); 
	}

	.data : 
	{ 
	  data = .; _data = .; __data = .;
	  *(.data)
	  *(.rodata)
	  . = ALIGN(4096); 
	}

	.bss  : 
	{
	  bss = .; _bss = .; __bss = .;	_bss_start = .;
	  *(.bss)
	  _bss_end = .;
	  . = ALIGN(4096); 
	} 
end = .;_end = .;__end = .;__end_ = .;    
}