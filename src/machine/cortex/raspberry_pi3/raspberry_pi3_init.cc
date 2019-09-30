//// EPOS Raspberry Pi3 (Cortex-A53) Initialization
//
//#include <system/config.h>
//#include <machine.h>
//#include <architecture/cpu.h>
//#include __MODEL_H
//
//__BEGIN_SYS
//
//#define GPIO_BASE       (IO_BASE + 0x200000)
//#define GPIO            ((struct GPIOREG*) GPIO_BASE)
//#define PBASE           0x3F000000
//
//#define GPFSEL1         (volatile unsigned int*)(PBASE+0x00200004)
//#define GPSET0          (volatile unsigned int*)(PBASE+0x0020001C)
//#define GPCLR0          (volatile unsigned int*)(PBASE+0x00200028)
//#define GPPUD           (volatile unsigned int*)(PBASE+0x00200094)
//#define GPPUDCLK0       (volatile unsigned int*)(PBASE+0x00200098)
//
//#define AUX_ENABLES     (volatile unsigned int*)(PBASE+0x00215004)
//#define AUX_MU_IO_REG   (volatile unsigned int*)(PBASE+0x00215040)
//#define AUX_MU_IER_REG  (volatile unsigned int*)(PBASE+0x00215044)
//#define AUX_MU_IIR_REG  (volatile unsigned int*)(PBASE+0x00215048)
//#define AUX_MU_LCR_REG  (volatile unsigned int*)(PBASE+0x0021504C)
//#define AUX_MU_MCR_REG  (volatile unsigned int*)(PBASE+0x00215050)
//#define AUX_MU_LSR_REG  (volatile unsigned int*)(PBASE+0x00215054)
//#define AUX_MU_MSR_REG  (volatile unsigned int*)(PBASE+0x00215058)
//#define AUX_MU_SCRATCH  (volatile unsigned int*)(PBASE+0x0021505C)
//#define AUX_MU_CNTL_REG (volatile unsigned int*)(PBASE+0x00215060)
//#define AUX_MU_STAT_REG (volatile unsigned int*)(PBASE+0x00215064)
//#define AUX_MU_BAUD_REG (volatile unsigned int*)(PBASE+0x00215068)
//
//// DSB causes completion of all cache maintenance operations appearing in program
//// order before the DSB instruction.
//void dsb()
//{
//    ASM("dsb");
//}
//
//
//// void uart_send( unsigned int c )
//// {
////     do{asm volatile ("nop"); } while(!((*AUX_MU_LSR_REG)&0x20));
////     *AUX_MU_IO_REG=c;
//// }
//
//// void print(char* ch)
//// {
////     while (*ch!=0)
////     {
////         uart_send((unsigned int)*ch);
////         ch++;
////     }
////     uart_send(0x0D);
////     uart_send(0x0A);
//// }
//
//
//
//// An ISB instruction causes the effect of all branch predictor maintenance
//// operations before the ISB instruction to be visible to all instructions
//// after the ISB instruction.
//void isb()
//{
//    ASM("isb");
//}
//
//void invalidate_caches()
//{

//}
//
//void Raspberry_Pi3::pre_init()
//{
//    // Relocated the vector table
//    // ASM("MCR p15, 0, %0, c12, c0, 0" : : "p"(Traits<Machine>::VECTOR_TABLE) :);
//    ASM ("MOV R0, #0 \t\n MSR SPSR, R0 \t\n CPSIE aif");
//    // MMU init
//    invalidate_caches();
//    clear_branch_prediction_array();
//    invalidate_tlb();
//    enable_dside_prefetch();
//    set_domain_access();
//    dsb();
//    isb();
//
//    // Initialize PageTable.
//
//    // Create a basic L1 page table in RAM, with 1MB sections containing a flat
//    // (VA=PA) mapping, all pages Full Access, Strongly Ordered.
//
//    // It would be faster to create this in a read-only section in an assembly file.
//
//    if(CPU::id() == 0)
//        page_tables_setup();
//
//    // Activate the MMU
//    enable_mmu();
//    dsb();
//    isb();
//
//    // MMU now enable - Virtual address system now active
//
//    // Branch Prediction init
//    branch_prediction_enable();
//
//    // SMP initialization
//    if(CPU::id() == 0) {
//        //primary core
//
//        // gic enable is now on Machine::pre_init()
//
//        // FIXME:: is the following part of memory map or is there really a flat segment register?
//        // this is only a flat segment register that allows mapping the start point for the secondary cores
//        static const unsigned int FLAG_SET_REG = 0x40000000;
//        // set flag register to the address secondary CPUS are meant to start executing
//// /*
//        ASM("str %0, [%1, #0x9c]" : : "p"(Traits<Machine>::VECTOR_TABLE), "p"(FLAG_SET_REG) :);
//        ASM("str %0, [%1, #0xac]" : : "p"(Traits<Machine>::VECTOR_TABLE), "p"(FLAG_SET_REG) :);
//        ASM("str %0, [%1, #0xbc]" : : "p"(Traits<Machine>::VECTOR_TABLE), "p"(FLAG_SET_REG) :);
////*/
//        dsb();
//        // secondary cores reset is now on Machine::pre_init()
//        ASM ("SEV");
//
//        clear_bss();
//    } else {
//        //clear_interrupt();
//    }
//}
//
//void Raspberry_Pi3::init()
//{
//}
//
//__END_SYS
