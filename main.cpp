#include <iostream>
#include <fstream>
#include <bitset>
#include <unordered_map>
#include <sstream>
#include <iomanip>
#include <algorithm>

using namespace std;

// struct for label

struct Label
{
    string LabelName;
    uint32_t pc;
};

// R type instructions
//  class R_Type
//  {
//      public:
//      bitset<5>
//  };

struct RType
{
	//Basically here we defined the bit distribution of 32 bit ,for R-type instruction 
    bitset<32> Instruction32;
    string InstructionHex;
    bitset<7> opcode;
    bitset<5> rd;
    bitset<3> func3;
    bitset<5> rs1;
    bitset<5> rs2;
    bitset<7> func7;
};

struct IType
{
	//Here also making the bit distribution
    bitset<32> Instruction32;
    string InstructionHex;
    bitset<7> opcode;
    bitset<5> rd;
    bitset<3> func3;
    bitset<5> rs1;
    bitset<12> imm;
};

struct SType
{
	//Bit distribution for S type instruction
    bitset<32> Instruction32;
    string InstructionHex;
    bitset<7> opcode;
    bitset<12> imm;
    bitset<3> func3;
    bitset<5> rs1;
    bitset<5> rs2;
};

struct SBType
{
	//Bit distrubution for SB type instruction
    bitset<32> Instruction32;
    string InstructionHex;
    bitset<7> opcode;
    bitset<12> imm;
    bitset<3> func3;
    bitset<5> rs1;
    bitset<5> rs2;
};

struct UType
{
	//Bit Distribution for U type instruction
    bitset<32> Instruction32;
    string InstructionHex;
    bitset<7> opcode;
    bitset<5> rd;
    bitset<20> imm;
};

struct UJType
{
    bitset<32> Instruction32;
    string InstructionHex;
    bitset<7> opcode;
    bitset<5> rd;
    bitset<20> imm;
};

void InstructionDetails_UJ(string line, int i, UJType &ujtype, unordered_map<string, int> labelMap, int currpc)
{
	//first search the x in the string line
    while (line[i] != 'x')
    {
        i++;
    }
    i++;
    string registerValue = "";
    while (line[i] != ' ' and line[i] != ',')
    {
        registerValue += line[i];
        i++;
    }
    int registerval_int = stoi(registerValue);//here convert the string to integer of the register value
    if (registerval_int < 0 || registerval_int > 31)
    {
		//Basically here we set the corner cases for register value it should not more than 31 and should not less than 0
        cerr << "The register does not exist" << endl;
        exit(EXIT_FAILURE);
    }
    ujtype.rd = registerval_int;//finally we got the distination register integer value
    while (line[i] != ',' &&line[i]!=' ' )
    {
        i++;
    }
    i++;
    while (line[i] == ' ')
        i++;
    string Label = "";
    while (i < line.size() and line[i] != ' ')
    {
        Label += line[i];
        i++;
    }
    auto it = labelMap.find(Label);//here searching the level is defined or not
    if (it != labelMap.end())
    {
        int labelPc = it->second;
        ujtype.imm = (labelPc - currpc) / 2;//if level defined the find the imm. value according program counter value
    }
    else
    {
		//if level not defined the error in the input code
        cerr << "The Label is not defined" << endl;
        exit(EXIT_FAILURE);
    }
    return;
}

void convert_UJ_32(UJType &ujtype)
{
	//In this function basically we convert UJ type instruction code in machine code 
    int i;
    for (i = 0; i < 7; i++)
    {
        ujtype.Instruction32[i] = ujtype.opcode[i];
    }
    for (; i < 12; i++)
    {
        ujtype.Instruction32[i] = ujtype.rd[i - 7];
    }
    for (; i < 20; i++)
    {
        ujtype.Instruction32[i] = ujtype.imm[i - 1];
    }
    ujtype.Instruction32[i] = ujtype.imm[10];
    i++;
    for (; i < 31; i++)
    {
        ujtype.Instruction32[i] = ujtype.imm[i - 21];
    }
    ujtype.Instruction32[i] = ujtype.imm[19];

    return;
}

void InstructionDetails_U(string line, int i, UType &rtype)
{
	//In this function we extract the detail of U type instruction
	//first we extract the destination register value
	//the extracting the immidiate value in decimal format
    while (line[i] != 'x')
    {
        i++;
    }
    i++;
    string registerValue = "";
    while (line[i] != ' ' and line[i] != ',')
    {
        registerValue += line[i];
        i++;
    }
    int registerval_int = stoi(registerValue);
    if (registerval_int < 0 || registerval_int > 31)
    {
        cerr << "The register does not exist" << endl;
        exit(EXIT_FAILURE);
    }
    rtype.rd = registerval_int;
    while (!isdigit(line[i]))
    {
        i++;
    }
    string immValue = "";
    while (i < line.size() and line[i] != ' ')
    {
        immValue += line[i];
        i++;
    }
    int immValueInDecimal = stoi(immValue, nullptr, 0);
    if (immValueInDecimal < -524288 || immValueInDecimal > 524287)
    {
        cerr << "The imm value is not under the limit" << endl;
        exit(EXIT_FAILURE);
    }
    // cout << immValue << "ss" << endl;
    rtype.imm = immValueInDecimal;

    return;
}

void convert_U_32(UType &rtype)
{
	//Here simply coverting the U type instruction distribution into the machine code.
    int i;
    for (i = 0; i < 7; i++)
    {
        rtype.Instruction32[i] = rtype.opcode[i];
    }
    for (; i < 12; i++)
    {
        rtype.Instruction32[i] = rtype.rd[i - 7];
    }
    for (; i < 32; i++)
    {
        rtype.Instruction32[i] = rtype.imm[i - 12];
    }
    return;
}

void InstructionDetails_SB(string line, int i, SBType &rtype, unordered_map<string, int> labelMap, int currPC)
{
	//In this function we extract the details for SB type instruction
	//accoding the insturion distribution firstly find the register rs1 and rs2 the check the level defined or not , if level defined then get the imm. field according to pc value
    while (line[i] != 'x')
    {
        i++;
    }
    i++;
    string registerValue = "";
    while (line[i] != ' ' && line[i] != ',')
    {
        registerValue += line[i];
        i++;
    }
    int registerval_int = stoi(registerValue);
    if (registerval_int < 0 || registerval_int > 31)
    {
        cerr << "The register does not exist" << endl;
        exit(EXIT_FAILURE);
    }
    rtype.rs1 = registerval_int;  //firstly got the rs1 value

    while (line[i] != 'x')
    {
        i++;
    }
    i++;
    registerValue = "";
    while (line[i] != ' ' && line[i] != ',')
    {
        registerValue += line[i];
        i++;
    }
    registerval_int = stoi(registerValue);
    if (registerval_int < 0 || registerval_int > 31)
    {
        cerr << "The register does not exist" << endl;
        exit(EXIT_FAILURE);
    }
    rtype.rs2 = registerval_int;   //got the rs2 value
    
    while (line[i] != ',' &&line[i]!=' ' )
    {
        i++;
    }
    i++;

    while (i < line.size() && (line[i] == ' ' || line[i] == '\t'))
    {
        i++;
    }

    string Label = "";
	//checking the level is defined or not
    while (i < line.size() && line[i] != ' ' && line[i] != '#' && line[i] != '\t' && line[i] != ',')
    {
        Label += line[i];
        i++;
    }
    // cout<<Label<<"deba"<<endl;
    auto it = labelMap.find(Label);
    if (it != labelMap.end())
    {
        int labelPc = it->second;
        rtype.imm = (labelPc - currPC) / 2;
    }
    else
    {
        cerr << "The Label is not defined" << endl;
        exit(EXIT_FAILURE);
    }
    // cout << rtype.imm << "se" << endl;

    return;
}

void convert_SB_32(SBType &rtype)
{
	//According to the SB type distribution we convert the SB type instruction into machine code in binary format ,
    //further we convert it into hexadecimal
    int i;
    for (i = 0; i < 7; i++)
    {
        rtype.Instruction32[i] = rtype.opcode[i];
    }
    rtype.Instruction32[i] = rtype.imm[10];
    i++;
    // cout << i << "di" << endl;

    for (; i < 12; i++)
    {

        // cout << i << "ded" << endl;
        // cout << rtype.imm[i - 8] << "deb" << endl;
        rtype.Instruction32[i] = rtype.imm[i - 8];
    }
    for (; i < 15; i++)
    {
        rtype.Instruction32[i] = rtype.func3[i - 12];
    }
    for (; i < 20; i++)
    {
        rtype.Instruction32[i] = rtype.rs1[i - 15];
    }
    for (; i < 25; i++)
    {
        rtype.Instruction32[i] = rtype.rs2[i - 20];
    }
    for (; i < 31; i++)
    {
        // cout << i << "de" << endl;
        rtype.Instruction32[i] = rtype.imm[i - 21];
    }
    // cout << i << "de" << endl;

    rtype.Instruction32[i] = rtype.imm[11];
    return;
}

string bin_hex_conversion(bitset<32> bin)
{
	//This funciotn take the 32 bit binary and convert it into hexadecimal and return it in for of string
    string hex_fin = "0x";
    for (int i = 31; i >= 0;)
    {
        int val = 0;
        for (int k = 8; k >= 1; k /= 2)
        {
            val += bin[i] * k;
            i--;
        }
        if (val < 10)
        {
            hex_fin += val + '0';
        }
        else
        {
            hex_fin += val - 10 + 'A';
        }
    }
    return hex_fin;
}

// helper function for R type

void InstructionDetails_R(string line, int i, RType &rtype)
{
	//Here extraction the detail of R type instruction
	//firstly find the rd value the going to find the rs1 and rs2 and further going to convert it into machine code

    while (line[i] != 'x')
    {
        i++;
    }
    i++;
    string registerValue = "";
    while (line[i] != ' ' and line[i] != ',')
    {
        registerValue += line[i];
        i++;
    }
    int registerval_int = stoi(registerValue);
    if (registerval_int < 0 || registerval_int > 31)
    {
        cerr << "The register does not exist" << endl;
        exit(EXIT_FAILURE);
    }
    rtype.rd = registerval_int;
    while (line[i] != 'x')
    {
        i++;
    }
    i++;
    registerValue = "";
    while (line[i] != ' ' and line[i] != ',')
    {
        registerValue += line[i];
        i++;
    }
    registerval_int = stoi(registerValue);
    if (registerval_int < 0 || registerval_int > 31)
    {
        cerr << "The register does not exist" << endl;
        exit(EXIT_FAILURE);
    }
    rtype.rs1 = registerval_int;
    while (line[i] != 'x')
    {
        i++;
    }
    i++;
    registerValue = "";
    while (i < line.size() and line[i] != ' ')
    {
        registerValue += line[i];
        i++;
    }
    registerval_int = stoi(registerValue);
    if (registerval_int < 0 || registerval_int > 31)
    {
        cerr << "The register does not exist" << endl;
        exit(EXIT_FAILURE);
    }
    rtype.rs2 = registerval_int;
    return;
}

void convert_R_32(RType &rtype)
{
	//here conveting the all register value in binary and put in the 32 bit format (in binary order)
    int i;
    for (i = 0; i < 7; i++)
    {
        rtype.Instruction32[i] = rtype.opcode[i];
    }
    for (; i < 12; i++)
    {
        rtype.Instruction32[i] = rtype.rd[i - 7];
    }
    for (; i < 15; i++)
    {
        rtype.Instruction32[i] = rtype.func3[i - 12];
    }
    for (; i < 20; i++)
    {
        rtype.Instruction32[i] = rtype.rs1[i - 15];
    }
    for (; i < 25; i++)
    {
        rtype.Instruction32[i] = rtype.rs2[i - 20];
    }
    for (; i < 32; i++)
    {
        rtype.Instruction32[i] = rtype.func7[i - 25];
    }
    return;
}

// helper function for I type

void InstructionDetails_I_type1(string line, int i, IType &itype)
{
    while (line[i] != 'x')
    {
        i++;
    }
    i++;
    string registerValue = "";
    while (line[i] != ' ' and line[i] != ',')
    {
        registerValue += line[i];
        i++;
    }
    // cout << registerValue << endl;
    int registerval_int = stoi(registerValue);
    // cout << registerval_int << "sus" << endl;
    if (registerval_int < 0 || registerval_int > 31)
    {
        cerr << "The register does not exist" << endl;
        exit(EXIT_FAILURE);
    }
    itype.rd = registerval_int;
    while (line[i] != 'x')
    {
        i++;
    }
    i++;
    registerValue = "";
    while (line[i] != ' ' and line[i] != ',')
    {
        registerValue += line[i];
        i++;
    }
    // cout << registerValue << endl;

    registerval_int = stoi(registerValue);
    if (registerval_int < 0 || registerval_int > 31)
    {
        cerr << "The register does not exist" << endl;
        exit(EXIT_FAILURE);
    }
    itype.rs1 = registerval_int;
    while (!isdigit(line[i]) && line[i] != '-')
    {
        i++;
    }
    string immValue = "";
    if (line[i] == '-')
    {
        immValue += line[i];
        i++;
    }

    while (i < line.size() && line[i] != ' ')
    {
        immValue += line[i];
        i++;
    }

    int immValueInDecimal = stoi(immValue);
    if (immValueInDecimal < -2048 || immValueInDecimal > 2047)
    {
        cerr << "Error: Immediate value out of range for a 12-bit signed integer." << endl;
        exit(EXIT_FAILURE);
    }
    // cout << immValueInDecimal << "sus" << endl;
    itype.imm = immValueInDecimal;

    return;
}
void InstructionDetails_I_type2(string line, int i, IType &itype)
{
    while (line[i] != 'x')
    {
        i++;
    }
    i++;
    string registerValue = "";
    while (line[i] != ' ' and line[i] != ',')
    {
        registerValue += line[i];
        i++;
    }
    int registerval_int = stoi(registerValue);
    if (registerval_int < 0 || registerval_int > 31)
    {
        cerr << "The register does not exist" << endl;
        exit(EXIT_FAILURE);
    }
    // cout<<registerval_int<<"sus"<<endl;
    itype.rd = registerval_int;

    while (!isdigit(line[i]))
    {
        i++;
    }
    string immValue = "";
    while (line[i] != ' ' and line[i] != '(')
    {
        immValue += line[i];
        i++;
    }
    int immValueInDecimal = stoi(immValue);
    if (immValueInDecimal < -2048 || immValueInDecimal > 2047)
    {
        cerr << "Error: Immediate value out of range for a 12-bit signed integer." << endl;
        exit(EXIT_FAILURE);
    }
    // cout<<immValue<<"sus"<<endl;

    itype.imm = immValueInDecimal;

    while (line[i] != 'x')
    {
        i++;
    }
    i++;
    registerValue = "";
    while (line[i] != ' ' and line[i] != ',')
    {
        registerValue += line[i];
        i++;
    }
    registerval_int = stoi(registerValue);
    // cout<<registerval_int<<"sus"<<endl;
    if (registerval_int < 0 || registerval_int > 31)
    {
        cerr << "The register does not exist" << endl;
        exit(EXIT_FAILURE);
    }
    itype.rs1 = registerval_int;
    return;
}

void convert_I_32(IType &itype)
{
	//here conveting the all register value in binary along with imm and put in the 32 bit format (in binary order)
    int i;
    for (i = 0; i < 7; i++)
    {
        itype.Instruction32[i] = itype.opcode[i];
    }
    for (; i < 12; i++)
    {
        itype.Instruction32[i] = itype.rd[i - 7];
    }
    for (; i < 15; i++)
    {
        itype.Instruction32[i] = itype.func3[i - 12];
    }
    for (; i < 20; i++)
    {
        itype.Instruction32[i] = itype.rs1[i - 15];
    }
    for (; i < 32; i++)
    {
        itype.Instruction32[i] = itype.imm[i - 20];
    }
    // cout<<itype.Instruction32<<"ss"<<endl;
    return;
}

void InstructionDetails_S(string line, int i, SType &rtype)
{
    while (line[i] != 'x')
    {
        i++;
    }
    i++;
    string registerValue = "";
    while (line[i] != ' ' and line[i] != ',')
    {
        registerValue += line[i];
        i++;
    }
    // cout << registerValue << endl;
    int registerval_int = stoi(registerValue);
    if (registerval_int < 0 || registerval_int > 31)
    {
        cerr << "The register does not exist" << endl;
        exit(EXIT_FAILURE);
    }
    rtype.rs2 = registerval_int;
    while (!isdigit(line[i]))
    {
        i++;
    }
    string immValue = "";
    while (line[i] != ' ' and line[i] != '(')
    {
        immValue += line[i];
        i++;
    }
    // cout << immValue << endl;
    int immValueInDecimal = stoi(immValue);
    rtype.imm = immValueInDecimal;
    while (line[i] != 'x')
    {
        i++;
    }
    i++;
    registerValue = "";
    while (i < line.size() and line[i] != ')')
    {
        registerValue += line[i];
        i++;
    }
    // cout << registerValue << endl;
    registerval_int = stoi(registerValue);
    if (registerval_int < 0 || registerval_int > 31)
    {
        cerr << "The register does not exist" << endl;
        exit(EXIT_FAILURE);
    }
    rtype.rs1 = registerval_int;
    return;
}

void convert_S_32(SType &rtype)
{
	//here conveting the all register value in binary along with the imm and put in the 32 bit format (in binary order)
    int i;
    for (i = 0; i < 7; i++)
    {
        rtype.Instruction32[i] = rtype.opcode[i];
    }
    for (; i < 12; i++)
    {
        rtype.Instruction32[i] = rtype.imm[i - 7];
    }
    for (; i < 15; i++)
    {
        rtype.Instruction32[i] = rtype.func3[i - 12];
    }
    for (; i < 20; i++)
    {
        rtype.Instruction32[i] = rtype.rs1[i - 15];
    }
    for (; i < 25; i++)
    {
        rtype.Instruction32[i] = rtype.rs2[i - 20];
    }
    for (; i < 32; i++)
    {
        rtype.Instruction32[i] = rtype.imm[i - 20];
    }
    // cout<<rtype.Instruction32<<endl;
    return;
}

// Class for helper functions
class MachineCodeHelper
{
public:
    unordered_map<string, int> LabelMap_gotopc; //unorder map for storing the labels with the program counter
    string Pro_counter_hex; //for storing the pc in hex format
    bitset<32> pro_counter = 0; //bitset for the pc

    //the following function checks for label in an instruction 
    bool checkLabel(const string &instruction)
    {
        for (char ch : instruction)
        {
            if (ch == ':')
            {
                return true;
            }
        }
        return false;
    }
    //the following function increments the program counter which is used in all the parts of code and data segments
    void incrementPro_counter(bitset<32> &pc)
    {
        for (int i = 2; i < 32; i++)
        {
            if (pc[i] == 0)
            {
                pc[i] = 1;
                break;
            }
            else
            {
                pc[i] = 0;
            }
        }
        return;
    }
    //the following function converts the pc from bin to hex which is later printed in the mc file
    void Binary_Hex_Pro_counter()
    {

        if (pro_counter == 0)
        {
            Pro_counter_hex = "0x0";
            return;
        }
        Pro_counter_hex = "0x";
        bool flag = false;
        for (int i = 31; i >= 0;)
        {
            int val = 0;
            for (int k = 8; k >= 1; k /= 2)
            {
                val += pro_counter[i] * k;
                i--;
            }
            if (val != 0)
            {
                flag = true;
            }
            if (flag)
            {
                if (val < 10)
                {
                    Pro_counter_hex += val + '0';
                }
                else
                {
                    Pro_counter_hex += val - 10 + 'A';
                }
            }
        }
        return;
    }
    //the following function is for storing the labels along with their respective program counter in the unordered map
    void storeLabel(string instruction)
    {
        int i = 0;
        string labelname = "";
        while (instruction[i] != ' ' && instruction[i] != ':')
        {
            labelname += instruction[i];
            i++;
        }

        Label newLabel;
        newLabel.pc = this->pro_counter.to_ulong();
        newLabel.LabelName = labelname;
        // cout << labelname << "deb112" << endl;
        // cout << newLabel.pc << "deb112" << endl;
        LabelMap_gotopc[labelname] = newLabel.pc;
        return;
    }
    // the following is the function which converts the instruction to machine code hex
    string Instruction_Hex(string line)
    {
        int i = 0;
        string real_instruct = "";
        while (line[i] == ' ')
        {
            i++;
        }
        while (line[i] != ' ')
        {
            real_instruct += line[i];
            i++;
        }

        if (real_instruct == "add")
        {
            RType add;
            add.func3 = 0;
            add.func7 = 0;
            add.opcode = 51;
            InstructionDetails_R(line, i, add);
            convert_R_32(add);
            add.InstructionHex = bin_hex_conversion(add.Instruction32);
            return add.InstructionHex;
        }
        else if (real_instruct == "and")
        {
            RType And;
            And.func3 = 7;
            And.func7 = 0;
            And.opcode = 51;
            InstructionDetails_R(line, i, And);
            convert_R_32(And);
            And.InstructionHex = bin_hex_conversion(And.Instruction32);
            return And.InstructionHex;
        }
        else if (real_instruct == "or")
        {
            RType OR;
            OR.func3 = 6;
            OR.func7 = 0;
            OR.opcode = 51;
            InstructionDetails_R(line, i, OR);
            convert_R_32(OR);
            OR.InstructionHex = bin_hex_conversion(OR.Instruction32);
            return OR.InstructionHex;
        }
        else if (real_instruct == "sll")
        {
            RType sll;
            sll.func3 = 1;
            sll.func7 = 0;
            sll.opcode = 51;
            InstructionDetails_R(line, i, sll);
            convert_R_32(sll);
            sll.InstructionHex = bin_hex_conversion(sll.Instruction32);
            return sll.InstructionHex;
        }
        else if (real_instruct == "slt")
        {
            RType slt;
            slt.func3 = 2;
            slt.func7 = 0;
            slt.opcode = 51;
            InstructionDetails_R(line, i, slt);
            convert_R_32(slt);
            slt.InstructionHex = bin_hex_conversion(slt.Instruction32);
            return slt.InstructionHex;
        }
        else if (real_instruct == "sra")
        {
            RType sra;
            sra.func3 = 5;
            sra.func7 = 32;
            sra.opcode = 51;
            InstructionDetails_R(line, i, sra);
            convert_R_32(sra);
            sra.InstructionHex = bin_hex_conversion(sra.Instruction32);
            return sra.InstructionHex;
        }
        else if (real_instruct == "srl")
        {
            RType srl;
            srl.func3 = 5;
            srl.func7 = 0;
            srl.opcode = 51;
            InstructionDetails_R(line, i, srl);
            convert_R_32(srl);
            srl.InstructionHex = bin_hex_conversion(srl.Instruction32);
            return srl.InstructionHex;
        }
        else if (real_instruct == "sub")
        {
            RType sub;
            sub.func3 = 0;
            sub.func7 = 32;
            sub.opcode = 51;
            InstructionDetails_R(line, i, sub);
            convert_R_32(sub);
            sub.InstructionHex = bin_hex_conversion(sub.Instruction32);
            return sub.InstructionHex;
        }
        else if (real_instruct == "xor")
        {
            RType Xor;
            Xor.func3 = 4;
            Xor.func7 = 0;
            Xor.opcode = 51;
            InstructionDetails_R(line, i, Xor);
            convert_R_32(Xor);
            Xor.InstructionHex = bin_hex_conversion(Xor.Instruction32);
            return Xor.InstructionHex;
        }
        else if (real_instruct == "mul")
        {
            RType mul;
            mul.func3 = 0;
            mul.func7 = 1;
            mul.opcode = 51;
            InstructionDetails_R(line, i, mul);
            convert_R_32(mul);
            mul.InstructionHex = bin_hex_conversion(mul.Instruction32);
            return mul.InstructionHex;
        }
        else if (real_instruct == "div")
        {
            RType Div;
            Div.func3 = 0;
            Div.func7 = 1;
            Div.opcode = 51;
            InstructionDetails_R(line, i, Div);
            convert_R_32(Div);
            Div.InstructionHex = bin_hex_conversion(Div.Instruction32);
            return Div.InstructionHex;
        }
        else if (real_instruct == "rem")
        {
            RType rem;
            rem.func3 = 6;
            rem.func7 = 1;
            rem.opcode = 51;
            InstructionDetails_R(line, i, rem);
            convert_R_32(rem);
            rem.InstructionHex = bin_hex_conversion(rem.Instruction32);
            return rem.InstructionHex;
        }
        else if (real_instruct == "addi")
        {
            IType addi;
            addi.func3 = 0;
            addi.opcode = 19;
            InstructionDetails_I_type1(line, i, addi);
            convert_I_32(addi);
            addi.InstructionHex = bin_hex_conversion(addi.Instruction32);
            return addi.InstructionHex;
        }
        else if (real_instruct == "andi")
        {
            IType andi;
            andi.func3 = 7;
            andi.opcode = 19;
            InstructionDetails_I_type1(line, i, andi);
            convert_I_32(andi);
            andi.InstructionHex = bin_hex_conversion(andi.Instruction32);
            return andi.InstructionHex;
        }
        else if (real_instruct == "ori")
        {
            IType ori;
            ori.func3 = 6;
            ori.opcode = 19;
            InstructionDetails_I_type1(line, i, ori);
            convert_I_32(ori);
            ori.InstructionHex = bin_hex_conversion(ori.Instruction32);
            return ori.InstructionHex;
        }
        else if (real_instruct == "lb")
        {
            IType lb;
            lb.func3 = 0;
            lb.opcode = 3;
            InstructionDetails_I_type2(line, i, lb);
            convert_I_32(lb);
            lb.InstructionHex = bin_hex_conversion(lb.Instruction32);
            return lb.InstructionHex;
        }

        else if (real_instruct == "ld")
        {
            // cout << "h" << endl;
            IType ld;
            ld.func3 = 3;
            ld.opcode = 3;
            InstructionDetails_I_type2(line, i, ld);
            convert_I_32(ld);
            ld.InstructionHex = bin_hex_conversion(ld.Instruction32);
            return ld.InstructionHex;
        }
        else if (real_instruct == "lh")
        {
            IType lh;
            lh.func3 = 1;
            lh.opcode = 3;
            InstructionDetails_I_type2(line, i, lh);
            convert_I_32(lh);
            lh.InstructionHex = bin_hex_conversion(lh.Instruction32);
            return lh.InstructionHex;
        }
        else if (real_instruct == "lw")
        {
            IType lw;
            lw.func3 = 2;
            lw.opcode = 3;
            InstructionDetails_I_type2(line, i, lw);
            convert_I_32(lw);
            lw.InstructionHex = bin_hex_conversion(lw.Instruction32);
            return lw.InstructionHex;
        }
        else if (real_instruct == "jalr")
        {
            IType jalr;
            jalr.func3 = 0;
            jalr.opcode = 103;
            InstructionDetails_I_type1(line, i, jalr);
            convert_I_32(jalr);
            jalr.InstructionHex = bin_hex_conversion(jalr.Instruction32);
            return jalr.InstructionHex;
        }
        else if (real_instruct == "sb")
        {
            SType jalr;
            jalr.func3 = 0;
            jalr.opcode = 35;
            InstructionDetails_S(line, i, jalr);
            convert_S_32(jalr);
            jalr.InstructionHex = bin_hex_conversion(jalr.Instruction32);
            return jalr.InstructionHex;
        }
        else if (real_instruct == "sw")
        {
            SType jalr;
            jalr.func3 = 2;
            jalr.opcode = 35;
            InstructionDetails_S(line, i, jalr);
            convert_S_32(jalr);
            jalr.InstructionHex = bin_hex_conversion(jalr.Instruction32);
            // cout<<jalr.InstructionHex<<"he"<<endl;
            return jalr.InstructionHex;
        }
        else if (real_instruct == "sd")
        {
            SType jalr;
            jalr.func3 = 3;
            jalr.opcode = 35;
            InstructionDetails_S(line, i, jalr);
            convert_S_32(jalr);
            jalr.InstructionHex = bin_hex_conversion(jalr.Instruction32);
            return jalr.InstructionHex;
        }
        else if (real_instruct == "sh")
        {
            SType jalr;
            jalr.func3 = 1;
            jalr.opcode = 35;
            InstructionDetails_S(line, i, jalr);
            convert_S_32(jalr);
            jalr.InstructionHex = bin_hex_conversion(jalr.Instruction32);
            return jalr.InstructionHex;
        }
        else if (real_instruct == "beq")
        {
            // cout << "beq here" << endl;
            SBType beq;
            beq.func3 = 0;
            beq.opcode = 99;
            InstructionDetails_SB(line, i, beq, LabelMap_gotopc, this->pro_counter.to_ulong());
            convert_SB_32(beq);
            beq.InstructionHex = bin_hex_conversion(beq.Instruction32);
            return beq.InstructionHex;
        }
        else if (real_instruct == "bne")
        {
            SBType bne;
            bne.func3 = 1;
            bne.opcode = 99;
            InstructionDetails_SB(line, i, bne, LabelMap_gotopc, this->pro_counter.to_ulong());
            convert_SB_32(bne);
            bne.InstructionHex = bin_hex_conversion(bne.Instruction32);
            return bne.InstructionHex;
        }
        else if (real_instruct == "bge")
        {
            SBType bge;
            bge.func3 = 5;
            bge.opcode = 99;
            InstructionDetails_SB(line, i, bge, LabelMap_gotopc, this->pro_counter.to_ulong());
            convert_SB_32(bge);
            bge.InstructionHex = bin_hex_conversion(bge.Instruction32);
            return bge.InstructionHex;
        }
        else if (real_instruct == "blt")
        {
            SBType blt;
            blt.func3 = 4;
            blt.opcode = 99;
            InstructionDetails_SB(line, i, blt, LabelMap_gotopc, this->pro_counter.to_ulong());
            convert_SB_32(blt);
            blt.InstructionHex = bin_hex_conversion(blt.Instruction32);
            return blt.InstructionHex;
        }
        else if (real_instruct == "auipc")
        {
            UType auipc;
            auipc.opcode = 23;
            InstructionDetails_U(line, i, auipc);
            convert_U_32(auipc);
            auipc.InstructionHex = bin_hex_conversion(auipc.Instruction32);
            return auipc.InstructionHex;
        }
        else if (real_instruct == "lui")
        {
            UType lui;
            lui.opcode = 55;
            InstructionDetails_U(line, i, lui);
            convert_U_32(lui);
            lui.InstructionHex = bin_hex_conversion(lui.Instruction32);
            return lui.InstructionHex;
        }

        else if (real_instruct == "jal")
        {
            UJType jal;
            jal.opcode = 111;
            InstructionDetails_UJ(line, i, jal, LabelMap_gotopc, this->pro_counter.to_ulong());
            convert_UJ_32(jal);
            jal.InstructionHex = bin_hex_conversion(jal.Instruction32);
            return jal.InstructionHex;
        }
        else
            return "error";
    }
};

string preprocessLine(const std::string &line)
{
    std::string lineWithoutSpaces = line;
    // lineWithoutSpaces.erase(remove_if(lineWithoutSpaces.begin(), lineWithoutSpaces.end(), ::isspace), lineWithoutSpaces.end());

    size_t commentPos = lineWithoutSpaces.find('#');
    if (commentPos != std::string::npos)
    {
        lineWithoutSpaces = lineWithoutSpaces.substr(0, commentPos);
    }

    return lineWithoutSpaces;
}
// Function that processes the instructions
void Assembler(const string &asmfileName, const string &mcfileName)
{
    ifstream asmFile(asmfileName); //input stream file
    ofstream mcFile(mcfileName); //output file stream

    //if we cant open the files sending the error!
    if (!asmFile.is_open() || !mcFile.is_open())
    {
        cerr << "Error: Cannot open files." << endl;
        exit(EXIT_FAILURE);
    }

    MachineCodeHelper helper; // Create an instance of the helper class

    uint32_t codeAddress = 0x00000000;
    uint32_t dataAddress = 0x10000000;

    string line;
    bool dataflag = false;
    //processing the lables with their program counter and storing them in the unordered_map
    while (getline(asmFile, line))
    {

        string processedLine = preprocessLine(line);
        // cout<<processedLine<<endl;
        if (processedLine.find(".data") != string::npos)
        {
            dataflag = true;
            continue;
        }
        if (processedLine.find(".text") != string::npos)
        {
            dataflag = false;
            continue;
        }
        else if (line.find(".word") != string::npos)
        {

            continue;
        }
        else if (line.find(".half") != string::npos)
        {

            continue;
        }
        else if (line.find(".asciiz") != string::npos)
        {

            continue;
        }
        if (processedLine.empty())
        {
            continue;
        }
        if (all_of(processedLine.begin(), processedLine.end(), ::isspace))
        {
            continue;
        }
        //considering the lables only after the .text segment
        if (helper.checkLabel(processedLine) && !dataflag)
        {
            // cout<<line<<endl;
            helper.storeLabel(line);
            size_t labelEnd = processedLine.find(':');
            string lineWithoutSpaces_1 = processedLine;
            lineWithoutSpaces_1.erase(remove_if(lineWithoutSpaces_1.begin(), lineWithoutSpaces_1.end(), ::isspace), lineWithoutSpaces_1.end());

            if (labelEnd != string::npos && labelEnd + 1 < lineWithoutSpaces_1.size())
            {
                processedLine = processedLine.substr(labelEnd + 1);
                processedLine = processedLine.substr(processedLine.find_first_not_of(" \t"));
            }
            else
            {
                // it is a pure label
                continue;
            }
        }

        helper.Binary_Hex_Pro_counter();

        helper.incrementPro_counter(helper.pro_counter);
    }
    asmFile.close();
    helper.pro_counter = 0;

    asmFile.open(asmfileName);
    dataflag = false;
    //processing the instructions 
    while (getline(asmFile, line))
    {
        string processedLine = preprocessLine(line);
        // cout << processedLine << endl;
        if (processedLine.find(".data") != string::npos)
        {
            dataflag = true;
            continue;
        }
        if (processedLine.find(".text") != string::npos)
        {
            dataflag = false;
            continue;
        }
        else if (line.find(".word") != string::npos)
        {

            continue;
        }
        else if (line.find(".half") != string::npos)
        {

            continue;
        }
        else if (line.find(".asciiz") != string::npos)
        {

            continue;
        }
        if (processedLine.empty())
        {
            continue;
        }
        if (all_of(processedLine.begin(), processedLine.end(), ::isspace))
        {
            continue;
        }
        if (helper.checkLabel(processedLine) && dataflag)
        {
            cout << line << endl;
            continue;
        }
        if (helper.checkLabel(processedLine) && !dataflag)
        {

            size_t labelEnd = processedLine.find(':');
            string lineWithoutSpaces_1 = processedLine;
            lineWithoutSpaces_1.erase(remove_if(lineWithoutSpaces_1.begin(), lineWithoutSpaces_1.end(), ::isspace), lineWithoutSpaces_1.end());

            if (labelEnd != string::npos && labelEnd + 1 < lineWithoutSpaces_1.size())
            {
                processedLine = processedLine.substr(labelEnd + 1);
                processedLine = processedLine.substr(processedLine.find_first_not_of(" \t"));
            }
            else
            {
                // it is a pure label
                continue;
            }
        }
        //editing the machine code file with the instructions machine code
        helper.Binary_Hex_Pro_counter();
        // cout << helper.pro_counter.to_ulong() << "thisde" << endl;
        mcFile << helper.Pro_counter_hex << " " << helper.Instruction_Hex(processedLine) << endl;

        helper.incrementPro_counter(helper.pro_counter);
    }
    // helper.incrementPro_counter(helper.pro_counter);
    //our terminating call sign is 0x00000000
    helper.Binary_Hex_Pro_counter();
    mcFile << helper.Pro_counter_hex << " 0x00000000" << endl;
    asmFile.close();
    helper.pro_counter = dataAddress;

    asmFile.open(asmfileName);
    dataflag = false;
    //now we are processing the data segment which start at 0x10000000
    while (getline(asmFile, line))
    {
        string processedLine = preprocessLine(line);
        if (processedLine.find(".data") != string::npos)
        {
            dataflag = true;
            continue;
        }
        if (processedLine.find(".text") != string::npos)
        {
            dataflag = false;
            continue;
        }
        //now we are considering the .word segment with the increment in program counter +4
        if (processedLine.find(".word") != string::npos)
        {
            istringstream iss(processedLine);
            string directive, value, label;
            iss >> directive;
            //if the line has a lable we are ignoring it and moving forward
            bool hasLabel = (processedLine.find(':') != string::npos) && (processedLine.find('.') != string::npos);
            if (hasLabel)
            {
                iss >> label;
            }
            while (iss >> value)
            {
                uint32_t data = stoi(value, nullptr, 0);
                helper.Binary_Hex_Pro_counter();
                mcFile << hex << setw(8) << setfill('0') << helper.Pro_counter_hex << " 0x" << hex << setw(8) << setfill('0') << data << endl;
                uint32_t counter_value = helper.pro_counter.to_ulong();
                counter_value += 4;
                helper.pro_counter = counter_value;
            }
            continue;
        }
        //now we are considering the .word segment with the increment in program counter +8
        if (processedLine.find(".dword") != string::npos)
        {
            istringstream iss(processedLine);
            string directive, value, label;
            iss >> directive;
            //if the line has a lable we are ignoring it and moving forward
            bool hasLabel = (processedLine.find(':') != string::npos) && (processedLine.find('.') != string::npos);
            if (hasLabel)
            {
                iss >> label;
            }
            while (iss >> value)
            {
                uint32_t data = stoi(value, nullptr, 0);
                helper.Binary_Hex_Pro_counter();
                mcFile << hex << setw(8) << setfill('0') << helper.Pro_counter_hex << " 0x" << hex << setw(8) << setfill('0') << data << endl;
                uint32_t counter_value = helper.pro_counter.to_ulong();
                counter_value += 8;
                helper.pro_counter = counter_value;
            }
            continue;
        }
        //now we are considering the .word segment with the increment in program counter +1
        if (processedLine.find(".byte") != string::npos)
        {
            istringstream iss(processedLine);
            string directive, value, label;
            iss >> directive;
            //if the line has a lable we are ignoring it and moving forward

            bool hasLabel = (processedLine.find(':') != string::npos) && (processedLine.find('.') != string::npos);
            if (hasLabel)
            {
                iss >> label;
            }
            while (iss >> value)
            {
                uint32_t data = stoi(value, nullptr, 0);
                helper.Binary_Hex_Pro_counter();
                mcFile << hex << setw(8) << setfill('0') << helper.Pro_counter_hex << " 0x" << hex << setw(8) << setfill('0') << data << endl;
                uint32_t counter_value = helper.pro_counter.to_ulong();
                counter_value += 1;
                helper.pro_counter = counter_value;
            }
            continue;
        }
        //now we are considering the .word segment with the increment in program counter +2
        if (processedLine.find(".half") != string::npos)
        {
            istringstream iss(processedLine);
            string directive, value, label;
            iss >> directive;
            //if the line has a lable we are ignoring it and moving forward

            bool hasLabel = (processedLine.find(':') != string::npos) && (processedLine.find('.') != string::npos);
            if (hasLabel)
            {
                iss >> label;
            }
            while (iss >> value)
            {
                // cout<<value<<endl;
                helper.Binary_Hex_Pro_counter();
                uint16_t data = stoi(value, nullptr, 0);
                mcFile << hex << setw(8) << setfill('0') << helper.Pro_counter_hex << " 0x" << hex << setw(4) << setfill('0') << data << endl;
                uint32_t counter_value = helper.pro_counter.to_ulong();
                counter_value += 2;
                helper.pro_counter = counter_value;
            }
            continue;
        }
        //now we are considering the .word segment with the increment in program counter +1 for each character
        if (processedLine.find(".asciiz") != string::npos)
        {
            size_t start = processedLine.find("\"");
            size_t end = processedLine.rfind("\"");

            string dataString = processedLine.substr(start + 1, end - start - 1);

            istringstream iss(processedLine);
            string directive, label;
            iss >> directive;
            //if the line has a lable we are ignoring it and moving forward
            bool hasLabel = (processedLine.find(':') != string::npos);

            if (hasLabel)
            {
                iss >> label;
            }

            for (char c : dataString)
            {
                helper.Binary_Hex_Pro_counter();
                uint8_t data = static_cast<uint8_t>(c);
                mcFile << hex << setw(8) << setfill('0') << helper.Pro_counter_hex << " 0x" << hex << setw(2) << setfill('0') << static_cast<int>(data) << endl;
                uint32_t counter_value = helper.pro_counter.to_ulong();
                counter_value += 1;
                helper.pro_counter = counter_value;
            }

            // Add null terminator
            helper.Binary_Hex_Pro_counter();
            mcFile << hex << setw(8) << setfill('0') << helper.Pro_counter_hex << " 0x00" << endl;
            uint32_t counter_value = helper.pro_counter.to_ulong();
            counter_value += 1;
            helper.pro_counter = counter_value;

            continue;
        }
    }

    asmFile.close();
    mcFile.close();
}

// Main function
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cerr << "Usage: " << argv[0] << " input.asm output.mc" << endl;
        return EXIT_FAILURE;
    }
    //getting the file names from the arguments in the bash command
    string asmfileName = argv[1]; //asm file name
    string mcfileName = argv[2]; //machine code output file 

    Assembler(asmfileName, mcfileName);

    cout << "Assembly to machine code conversion complete." << endl;

    return EXIT_SUCCESS;
}


