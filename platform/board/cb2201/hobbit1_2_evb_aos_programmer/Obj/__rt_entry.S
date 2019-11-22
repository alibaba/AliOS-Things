                                                       
 .global __main                                         
 .weak __main                                           
 .global __e_rom                                        
 .weak __e_rom                                          
 .global __s_ram_data_1                                 
 .weak __s_ram_data_1                                   
 .global __e_ram_data_1                                 
 .weak __e_ram_data_1                                   
 .global __s_ram_bss_1                                  
 .weak __s_ram_bss_1                                    
 .global __e_ram_bss_1                                  
 .weak __e_ram_bss_1                                    
 .global __s_ram_data_2                                 
 .weak __s_ram_data_2                                   
 .global __e_ram_data_2                                 
 .weak __e_ram_data_2                                   
 .global __s_ram_bss_2                                  
 .weak __s_ram_bss_2                                    
 .global __e_ram_bss_2                                  
 .weak __e_ram_bss_2                                    
 .global __s_ram_data_3                                 
 .weak __s_ram_data_3                                   
 .global __e_ram_data_3                                 
 .weak __e_ram_data_3                                   
 .global __s_ram_bss_3                                  
 .weak __s_ram_bss_3                                    
 .global __e_ram_bss_3                                  
 .weak __e_ram_bss_3                                    
 .global __s_ram_data_4                                 
 .weak __s_ram_data_4                                   
 .global __e_ram_data_4                                 
 .weak __e_ram_data_4                                   
 .global __s_ram_bss_4                                  
 .weak __s_ram_bss_4                                    
 .global __e_ram_bss_4                                  
 .weak __e_ram_bss_4                                    
 .global __s_ram_data_5                                 
 .weak __s_ram_data_5                                   
 .global __e_ram_data_5                                 
 .weak __e_ram_data_5                                   
 .global __s_ram_bss_5                                  
 .weak __s_ram_bss_5                                    
 .global __e_ram_bss_5                                  
 .weak __e_ram_bss_5                                    
 .global __CDKInternalChipInitHandler                   
 .weak __CDKInternalChipInitHandler                     
                                                        
 .text                                                  
 .align 3                                               
 __bss_initialization:                                  
     subu	a2, a3		                               
     lsri	a2, 2		                               
     cmpnei	a2, 0                                      
     bf		2f                                         
     movi	a1, 0		                               
 1:                                                     
     stw		a1, (a3)	                               
     addi	a3, 4		                               
     subi	a2, 1		                               
     cmpnei	a2, 0                                      
     bt		1b			                               
 2:                                                     
     jmp r15                                            
                                                        
 __rom_decompression:                                   
     cmphs	a1, a2                                     
     bt		4f                                         
 3:                                                     
     ld.w	a3, (a0, 0)                                
     st.w	a3, (a1, 0)                                
     addi	a0, 4                                      
     addi	a1, 4                                      
     cmphs	a1, a2                                     
     bf		3b                                         
 4:                                                     
     jmp 	r15                                        
                                                        
 __main:                                                
     mov       r6,     r15                                  
     lrw         a3, __s_ram_bss_1                          
     lrw         a2, __e_ram_bss_1                          
     bsr         __bss_initialization                       
     lrw         a3, __s_ram_bss_2                          
     lrw         a2, __e_ram_bss_2                          
     bsr         __bss_initialization                       
     lrw         a3, __s_ram_bss_3                          
     lrw         a2, __e_ram_bss_3                          
     bsr         __bss_initialization                       
     lrw         a3, __s_ram_bss_4                          
     lrw         a2, __e_ram_bss_4                          
     bsr         __bss_initialization                       
     lrw         a3, __s_ram_bss_5                          
     lrw         a2, __e_ram_bss_5                          
     bsr         __bss_initialization                       
     lrw         a0, __e_rom                                
     lrw         a1, __s_ram_data_1                         
     lrw         a2, __e_ram_data_1                         
     bsr         __rom_decompression                        
     lrw         a1, __s_ram_data_2                         
     lrw         a2, __e_ram_data_2                         
     bsr         __rom_decompression                        
     lrw         a1, __s_ram_data_3                         
     lrw         a2, __e_ram_data_3                         
     bsr         __rom_decompression                        
     lrw         a1, __s_ram_data_4                         
     lrw         a2, __e_ram_data_4                         
     bsr         __rom_decompression                        
     lrw         a1, __s_ram_data_5                         
     lrw         a2, __e_ram_data_5                         
     bsr         __rom_decompression                        
 #ifdef __CSKYABIV2__                                       
     subi        sp, 4                                      
     stw         r6, (sp, 0)                                
     lrw         a0, __CDKInternalChipInitHandler           
     cmpnei        a0, 0                                    
     bf          1f                                         
     jsr         a0                                         
 1:                                                         
     lrw         a0, main                                   
     jsr          a0                                        
 #else                                                      
     subi        sp, 8                                      
     stw         r6, (sp, 0)                                
     lrw         a0, __CDKInternalChipInitHandler           
     cmpnei      a0, 0                                      
     bf          1f                                         
     jsri        __CDKInternalChipInitHandler               
 1:                                                         
     jsri         main                                      
 #endif                                                     
     ldw        r15, (sp, 0)                                
 #ifdef __CSKYABIV2__                                       
     addi        sp, 4                                      
 #else                                                      
     addi        sp, 8                                      
 #endif                                                     
     jmp        r15                                         
                                                        