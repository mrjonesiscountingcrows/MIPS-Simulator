/*
Team Members:   1. Tapan Tandon	
		2. Salvador A Cartagena
		3. Santosh Pandey
*/



#include <stdio.h>
#include "shell.h"


uint32_t instr=0;
uint32_t opcode=0;
int msb=0;
int MSB_rs=0;
int MSB_rt=0;

// Defining structure for storing the decoded instructions and registers
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

// A function for sign extension based on number of bits 
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

    //R-Type

    if (opcode==0)
    {


        R.rs= (instr>>21) & 0x1F;
        R.rt= (instr>>16) & 0x1F;
        R.rd= (instr>>11) & 0x1F;
        R.func= instr & 0x3F;
        R.shamt= (instr>>6) & 0x1F;
        printf("Function: %u \n",R.func);

    }

    //J- Type

    else if (opcode==2 || opcode==3)
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

    //I-Type

    else
    {
        if (opcode==5)  //I Type bne decode
        {

         I.immediate= instr & 0x0000FFFF;
         I.rs= (instr>>21) & 0x1F;
         I.rt= (instr>>16) & 0x1F;
        }

        else if (opcode==4)  //I Type beq decode
        {
         I.immediate= instr & 0x0000FFFF;
         I.rs= (instr>>21) & 0x1F;
         I.rt= (instr>>16) & 0x1F;
        }

        else
        {
         I.rs= (instr>>21) & 0x1F;
         printf("I.rs:%u\t",I.rs);
         I.rt = (instr>>16) & 0x1F;
         printf("I.rt:%u\n",I.rt);
	   // Read the value of rs register which holds the address to jump
         I.immediate = instr & 0x0000FFFF;
         }



    }
}

void execute()
{
      // R-Type add Instruction

    if (opcode==0 && R.func == 32)
    {
      MSB_rs = (NEXT_STATE.REGS[R.rs]>>31) & 1;
      printf("MSB for Rs is: %u \n", MSB_rs);

      MSB_rt = (NEXT_STATE.REGS[R.rt]>>31) & 1;
      printf("MSB for Rt is: %u \n", MSB_rt);

      if(MSB_rs == 1 || MSB_rt ==1)
      {
        CURRENT_STATE.FLAG_N =1;
        printf("One of the operands is negative \n");
      }

      NEXT_STATE.REGS[R.rd] = NEXT_STATE.REGS[R.rs] + NEXT_STATE.REGS[R.rt];

      msb = (NEXT_STATE.REGS[R.rd]>>31) & 1;
      printf("MSB is: %u \n", msb);
      if (msb==1)
      {
        CURRENT_STATE.FLAG_N =1;
        printf("The result is a negative number \n");
      }

      if(NEXT_STATE.REGS[R.rd] > 2147483647)
      {
        CURRENT_STATE.FLAG_V = 1;
        CURRENT_STATE.FLAG_C = 1;
      }

      printf("Addition of R-Type is: %u \n",NEXT_STATE.REGS[R.rd]);
      printf("The overflow flag value is: %d \n", CURRENT_STATE.FLAG_V);
      printf("The carry flag value is %d \n", CURRENT_STATE.FLAG_C);

      if(NEXT_STATE.REGS[R.rd] == 0)
      {
        CURRENT_STATE.FLAG_Z = 1;
        printf("The Zero flag value is: %d \n", CURRENT_STATE.FLAG_Z);
      }

    }

    //R-Type sub Instruction

    else if (opcode == 0 && R.func == 34)
    {
      MSB_rs = (NEXT_STATE.REGS[R.rs]>>31) & 1;
      printf("MSB for Rs is: %u \n", MSB_rs);

      MSB_rt = (NEXT_STATE.REGS[R.rt]>>31) & 1;
      printf("MSB for Rt is: %u \n", MSB_rt);

      if(MSB_rs == 1 || MSB_rt ==1)
      {
        CURRENT_STATE.FLAG_N =1;
        printf("One of the operands is negative \n");
      }


      NEXT_STATE.REGS[R.rd] = CURRENT_STATE.REGS[R.rs] - CURRENT_STATE.REGS[R.rt];

      msb = (NEXT_STATE.REGS[R.rd]>>31) & 1;
      printf("MSB is: %u \n", msb);
      if (msb==1)
      {
        CURRENT_STATE.FLAG_N =1;
        printf("The result is a negative number \n");
      }

      if(NEXT_STATE.REGS[R.rd] < -2147483648)
      {
        CURRENT_STATE.FLAG_V = 1;
        CURRENT_STATE.FLAG_C = 1;
      }

      printf("Subtraction of R-Type is: %u \n",NEXT_STATE.REGS[R.rd]);
      printf("The overflow flag value is: %d \n", CURRENT_STATE.FLAG_V);
      printf("The carry flag value is %d \n", CURRENT_STATE.FLAG_C);

      if(NEXT_STATE.REGS[R.rd] == 0)
      {
        CURRENT_STATE.FLAG_Z = 1;
        printf("The Zero flag value is: %d \n", CURRENT_STATE.FLAG_Z);
      }

    }

    //R-Type addu Instruction

    else if (opcode == 0 && R.func == 33)
    {
      NEXT_STATE.REGS[R.rd] = CURRENT_STATE.REGS[R.rs] + CURRENT_STATE.REGS[R.rt];
      if(NEXT_STATE.REGS[R.rd] > 4294967295)
      {
        CURRENT_STATE.FLAG_V = 1;
        CURRENT_STATE.FLAG_C = 1;
      }

      printf("Addition U of R-Type is: %u \n",NEXT_STATE.REGS[R.rd]);
      printf("The overflow flag value is: %d \n", CURRENT_STATE.FLAG_V);
      printf("The carry flag value is %d \n", CURRENT_STATE.FLAG_C);

      msb = (NEXT_STATE.REGS[R.rd]>>31) & 1;
      printf("MSB is: %u \n", msb);
      if (msb==1)
      {
        CURRENT_STATE.FLAG_N =1;
        printf("The result is a negative number and the negative flag value is: %d \n", CURRENT_STATE.FLAG_N);
      }

      if(NEXT_STATE.REGS[R.rd] == 0)
      {
        CURRENT_STATE.FLAG_Z = 1;
        printf("The Zero flag value is: %d \n", CURRENT_STATE.FLAG_Z);
      }

    }

    //R-Type subu Instruction

    else if (opcode == 0 && R.func == 35)
    {
      NEXT_STATE.REGS[R.rd] = CURRENT_STATE.REGS[R.rs] - CURRENT_STATE.REGS[R.rt];

      msb = (NEXT_STATE.REGS[R.rd]>>31) & 1;
      printf("MSB is: %u \n", msb);
      if (msb==1)
      {
        CURRENT_STATE.FLAG_N =1;
      }
      if (NEXT_STATE.REGS[R.rd] == 0)
      {
        CURRENT_STATE.FLAG_Z = 1;
      }

      printf("Subtraction U of R-Type is: %d \n",NEXT_STATE.REGS[R.rd]);
      printf("The negative flag value is: %d \n", CURRENT_STATE.FLAG_N);
      printf("The zero flag value is %d \n", CURRENT_STATE.FLAG_Z);
    }
    //R-Type slt Instruction

    else if (opcode == 0 && R.func == 42)
    {
        if(CURRENT_STATE.REGS[R.rs] < CURRENT_STATE.REGS[R.rt])
        {
          NEXT_STATE.REGS[R.rd] = 1;
        }
        else
        {
          NEXT_STATE.REGS[R.rd] = 0;
        }

        printf("Slt for R-Type is: %u \n", NEXT_STATE.REGS[R.rd]);
    }

    //R-Type sltu Instruction

    else if (opcode == 0 && R.func == 43)
    {
      MSB_rs = (NEXT_STATE.REGS[R.rs]>>31) & 1;
      printf("MSB for Rs is: %u \n", MSB_rs);

      MSB_rt = (NEXT_STATE.REGS[R.rt]>>31) & 1;
      printf("MSB for Rt is: %u \n", MSB_rt);

      if(MSB_rs == 1 || MSB_rt ==1)
      {
        CURRENT_STATE.FLAG_N =1;
      }

      if(CURRENT_STATE.REGS[R.rs] < CURRENT_STATE.REGS[R.rt])
      {
        NEXT_STATE.REGS[R.rd] = 1;
      }
      else
      {
        NEXT_STATE.REGS[R.rd] = 0;
      }

      printf("Slt U for R-Type is: %u \n", NEXT_STATE.REGS[R.rd]);
    }

    // J-Type Jump Instuction
	
	else if (opcode == 2)
	{
	NEXT_STATE.PC = J.address;
	}

	else if (opcode == 3)
	{
	NEXT_STATE.REGS[31] = NEXT_STATE.PC;
	NEXT_STATE.PC = J.address;
	}

    // I-Type addi Instruction

    else if (opcode == 8)
    {

        uint32_t temp= extension(I.immediate,16);

        NEXT_STATE.REGS[I.rt] = CURRENT_STATE.REGS[I.rs] + temp;

        msb = (NEXT_STATE.REGS[I.rt]>>31) & 1;
        printf("MSB is: %u \n", msb);
        if (msb==1)
        {
          CURRENT_STATE.FLAG_N =1;
          printf("The result is a negative number \n");
        }
        printf("Add Immediate Result is: %u \n",NEXT_STATE.REGS[I.rt]);

    }

    //I-Type addiu Instruction

    else if (opcode == 9)
    {

        NEXT_STATE.REGS[I.rt] = I.rs + I.immediate;
        if(NEXT_STATE.REGS[I.rt] > 4294967295)
        {
          CURRENT_STATE.FLAG_V = 1;
          CURRENT_STATE.FLAG_C = 1;
        }

        printf("Add Immediate U of I-Type is: %u \n",NEXT_STATE.REGS[I.rt]);
        printf("The overflow flag value is: %d \n", CURRENT_STATE.FLAG_V);
        printf("The carry flag value is %d \n", CURRENT_STATE.FLAG_C);

        if(NEXT_STATE.REGS[I.rt] == 0)
        {
          CURRENT_STATE.FLAG_Z = 1;
          printf("The Zero flag value is: %d \n", CURRENT_STATE.FLAG_Z);
        }

    }

    //I-Type slti Instruction

    else if (opcode == 10)
    {
      if(CURRENT_STATE.REGS[I.rs] < I.immediate)
      {
        NEXT_STATE.REGS[I.rt] = 1;
      }
      else
      {
        NEXT_STATE.REGS[I.rt] = 0;
      }

    printf("Slti for I-Type is: %u \n", CURRENT_STATE.REGS[I.rt]);
   }



   //I-Type ori Instruction

   else if (opcode == 13)
   {
     NEXT_STATE.REGS[I.rt] = CURRENT_STATE.REGS[I.rs] | I.immediate;

     printf("Ori for I-Type is: %x \n", NEXT_STATE.REGS[I.rt]);

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

      uint32_t temp = CURRENT_STATE.REGS[I.rs];
	    printf("Memory Address location: %x \t",temp);
	    uint32_t data = mem_read_32(temp);
	    printf("Loaded word: %u \n",data);

    }

      // I-type sw Instruction
   else if (opcode== 43)
    {
        // one operation for immediate value

      uint32_t temp = CURRENT_STATE.REGS[I.rs];
	    printf("Address: %x\t",temp);
	    mem_write_32(temp,CURRENT_STATE.REGS[I.rt]);
      printf("Value at %x is: %u \n",temp, CURRENT_STATE.REGS[I.rt]);
    }

    // I-type bne Instruction
   else if (opcode== 5)
    {
        // one operation for immediate value

        if (CURRENT_STATE.REGS[I.rs] != CURRENT_STATE.REGS[I.rt])
        {
         uint32_t temp= extension(I.immediate,16);
         temp = temp <<2;
         printf("Temp: %x\n",temp);
         NEXT_STATE.PC += temp;
         printf("Next state after branching is: %x\n",NEXT_STATE.PC);
          //Add it together
        }

    }


    // I-type beq Instruction
   else if (opcode== 4)
    {
        // one operation for immediate value
        if(CURRENT_STATE.REGS[I.rs] == CURRENT_STATE.REGS[I.rt])
        {
         uint32_t temp= extension(I.immediate,16);
         temp = temp <<2;
         printf("Temp: %x\n",temp);
         NEXT_STATE.PC += temp;
         printf("Next state after branching is: %x\n",NEXT_STATE.PC);
          //Add it together
        }

    }

    // I-type bgtz Instruction
   else if (opcode== 7)
    {
        // one operation for immediate value

        if (CURRENT_STATE.REGS[I.rt]>0)
        {
         uint32_t temp= extension(I.immediate,16);
         temp = temp <<2;
         printf("Temp: %x\n",temp);
         NEXT_STATE.PC += temp;
         printf("Next state after branching is: %x\n",NEXT_STATE.PC);
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
    if(instr == 0)
    {
      RUN_BIT = FALSE;
    }


}
