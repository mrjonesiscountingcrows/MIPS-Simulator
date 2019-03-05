#include <stdio.h>
#include "shell.h"


uint32_t instr=0;
uint32_t opcode=0;

typedef struct Rtype
{
    uint32_t rs;
    uint32_t rt;
    uint32_t rd;
    uint32_t func;
    uint32_t shamt;
};

typedef struct Itype
{
      uint32_t rs;
      uint32_t rt;
      uint32_t immediate;
      uint32_t Iaddress;  
};
typedef struct Jtype
{
      uint32_t address;
};

struct Rtype R;
struct Itype I;
struct Jtype J;
uint32_t a=4;


uint32_t extension(uint32_t temp,int length)
{
        // shift to get MSB
    int x = (temp>>(length-1)); 
    if (x==0)
        // Then do use and with 0000FFFF        
        {return (temp & 0x0000FFFF);   }    
    else
        // or it with FFFF0000
        {return (temp | 0xFFFF0000);   }

}

uint32_t fetch()
{
instr= mem_read_32(CURRENT_STATE.PC);
printf("\n\nNew Instruction: %x\n",instr);
NEXT_STATE.PC=CURRENT_STATE.PC+a ;

    return instr;
}

void decode(uint32_t instr)
{
    opcode = instr>>26;
    printf("Opcode is: %u \n", opcode);
    if (opcode==0)   // R type
    {


        R.rs= (instr>>21) & 0x1F;
        R.rt= (instr>>16) & 0x1F;
        R.rd= (instr>>11) & 0x1F;
        R.func= instr & 0x3F;
        R.shamt= (instr>>6) & 0x1F;
        printf("Function: %u \n",R.func);

        R.rs=CURRENT_STATE.REGS[R.rs];
        printf("Rs: %u \n", R.rs);
        R.rt=CURRENT_STATE.REGS[R.rt];
        printf("Rt: %u \n", R.rt);
        R.rd=CURRENT_STATE.REGS[R.rd];
        printf("Rd: %u \n", R.rd);


    }
    else if (opcode==2 || opcode==3)    // J type
    {
        printf("New Instruction is: %x \n",instr);
        J.address= instr & 0x3FFFFFF;
          // get the old pc
        uint32_t old=  CURRENT_STATE.PC;
        old = old & 0xF0000000;
        printf("old: %x\n",old);
        // get the address and shift by 2 bits
        uint32_t address1= (J.address<<2); 
        printf("address: %x\n",address1);        
        // add both
        J.address= old | address1;
    }
    else    // I type
    {
        if (opcode == 5)
        {
         
         I.immediate= instr & 0x0000FFFF;
         I.rs= (instr>>21) & 0x1F;
        I.rt= (instr>>16) & 0x1F;
        }
         

        else
        {
         uint32_t rs= (instr>>21) & 0x1F;
         
         I.rt = (instr>>16) & 0x1F;
         printf("I.rt:%u\t",I.rt);
	 // Read the value of rs register which holds the address to jump
         I.rs=CURRENT_STATE.REGS[rs];
	 printf("I.rs:%x\n",I.rs);
         I.immediate = instr & 0xFFFF;
         }



    }
}

void execute()
{
    // read the opcode
    // switch case for each instr
    // get a way to read registers as address


    // R-Type add Instruction

    if (opcode==0 && R.func == 32)
    {

        R.rd = R.rs + R.rt;
        printf("add for R-Type is: %u \n", R.rd);
    }

    //R-Type sub Instruction

    else if (opcode == 0 && R.func == 34)
    {
        R.rd = R.rs - R.rt;
        printf("Sub for R-Type is: %u \n", R.rd);
    }

    //R-Type addu Instruction

    else if (opcode == 0 && R.func == 33)
    {
        R.rd = R.rs + R.rt;
        printf("addu for R-Type is: %u \n", R.rd);

    }

    //R-Type subu Instruction

    else if (opcode == 0 && R.func == 35)
    {
        R.rd = R.rs - R.rt;
        printf("Subu for R-Type is: %u \n", R.rd);
    }

    //R-Type slt Instruction

    else if (opcode == 0 && R.func == 42)
    {
        if(R.rs < R.rt)
        {
          R.rd = 1;
        }
        else
        {
          R.rd = 0;
        }

        printf("Slt for R-Type is: %u \n", R.rd);
    }

    //R-Type sltu Instruction

    else if (opcode == 0 && R.func == 43)
    {
        if(R.rs < R.rt)
        {
          R.rd = 1;
        }
        else
        {
          R.rd = 0;
        }

        printf("Sltu for R-Type is: %u \n", R.rd);
    }

    // J-Type Jump Instuction

    else if (opcode ==2 || opcode==3)
    {
        NEXT_STATE.PC=J.address;
      
    }

    // I-Type addi Instruction

    else if (opcode == 8)
    {

        NEXT_STATE.REGS[I.rt] = I.rs + I.immediate;
        printf("Result %u \n",NEXT_STATE.REGS[I.rt]);

    }

    //I-Type addiu Instruction

    else if (opcode == 9)
    {

        NEXT_STATE.REGS[I.rt] = I.rs + I.immediate;
        printf("Result %u \n",NEXT_STATE.REGS[I.rt]);

    }

    //I-Type slti Instruction

    else if (opcode == 10)
    {
      if(I.rs < I.immediate)
      {
        I.rt = 1;
      }
      else
      {
        I.rt = 0;
      }

    printf("Slti for I-Type is: %u \n", I.rt);
   }

   //I-Type ori Instruction

   else if (opcode == 13)
   {
     I.rt = I.rs | I.immediate;

     printf("Ori for I-Type is: %u \n", I.rt);

   }

   //I-Type lui Instruction

   else if (opcode == 15)
   {
     NEXT_STATE.REGS[I.rt] = I.immediate << 16 & 0xFFFF0000;
     printf("Lui for I-Type is: %x \n",NEXT_STATE.REGS[I.rt]);

   }

    // I-type lw Instruction
   else if (opcode== 35)
    {
        // one operation for immediate value

        uint32_t temp = I.rs;
	//temp = extension(temp,16);
	printf("Address: %x\t",temp);
	uint32_t data = mem_read_32(temp);
	printf("Loaded: %x\n",data);    
    }

    // I-type bne Instruction
   else if (opcode== 5)
    {
        // one operation for immediate value

        if(I.rs != I.rt)
        {
         uint32_t temp= extension(I.immediate,16);
         temp = temp <<2;
         printf("Temp: %x\n",temp);
         //NEXT_STATE.PC += temp;  
         printf("Next state: %x\n",NEXT_STATE.PC+temp);
          //Add it together
        }
  
    }









}
void process_instruction()
{
    /* execute one instruction here. You should use CURRENT_STATE and modify
     * values in NEXT_STATE. You can call mem_read_32() and mem_write_32() to
     * access memory. */
    uint32_t instr = fetch();
    decode(instr);
    execute();


}
