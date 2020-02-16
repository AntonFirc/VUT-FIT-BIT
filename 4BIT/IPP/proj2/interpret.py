from __future__ import print_function
import sys
import argparse
import xml.etree.ElementTree

def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)

class Variable: #variable information
    def __init__(self, var_name, var_value, var_frame):
        self.var_name = var_name
        self.var_value = var_value
        self.var_frame = var_frame

class Label:    #label info
    def __init__(self, name, inst_number):
        self.name = name
        self.inst_number = inst_number  #order

class Instruction:  #instruction info
    def __init__(self, order, opcode, args):
        self.order = order
        self.opcode = opcode
        self.arg = args #array of arguments

class Argument:
    def __init__(self, atype, text):
        self.atype = atype  #type  (variable, int,string,bool)
        self.text = text    #value or var name

def findvar( name, frame ): #finds and returns variable if exists
    for var_s in variables[frame]:
       if (var_s.var_name == name):
           return var_s
    eprint("Error: Variable does not exist")
    sys.exit(32)
    return

def findlabel( name ):  #finds and returns label order if exists
    for lbl in labels:
        if (lbl.name == name):
            return (int(lbl.inst_number) - 1)
    eprint("Error: Label", name, "not found")
    sys.exit(32)
    return

def get_frame( ):   #return memory frame index
    if (mem_frame == "glb"):
         return 0
    elif (mem_frame == "tmp"):
        return 1
    elif (mem_frame == "fnc"):
        return 2
    else:
        eprint("Error: Unknown memory frame")
        sys.exit(99)
    return


parser = argparse.ArgumentParser()
parser.add_argument("--source", help="source file", required=True)
args = parser.parse_args()
# SCOPE
# 0 => global
# 1 => temporary#change while cycle idx to match label instruction order
# 2 => function
variables = [[], [], []] # array of arays for variables in different contexts
labels = [] #array of labels
instructions = []   #array to store loaded instructions
stack = [] # stack for pushs and pops instructions
call_stack =[] #call stack
mem_frame = "glb" # begins with global memory frame

xml_file = xml.etree.ElementTree.parse(args.source).getroot()   #open input XML

for atype in xml_file.findall('instruction'): #loop trough all instructions in XML
    opcode = atype.get('opcode')   #get OPCODE
    order = atype.get('order')  #get ORDER
    arg = []    #array for arguments
    for child in atype: #for every argument in instruction
        arg.append(Argument(child.attrib.get('type'), child.text))  #append argument to array
    instructions.append(Instruction(order, opcode, arg))    #append instruction to array
    if (opcode == "LABEL"): #if instruction LABEL save info
        num = order
        name = arg[0].text
        lbl = Label(name, num)
        for lbl_s in labels:
           if (lbl_s.name == name):
               eprint("Error: Label", name, "already exists !")
               sys.exit(52)
        labels.append(lbl)

idx = 0 #index to instructions array
while idx < len(instructions): #loop trough all loaded instructions
    instruction = instructions[idx].opcode  #get opcode
    if (instruction == "WRITE" or instruction == "DPRINT"):
        p_type = instructions[idx].arg[0].atype
        if (p_type == "var"):   #if var print value
            name = instructions[idx].arg[0].text[3:]
            var_p = findvar(name, get_frame())
            if (instruction == "WRITE"):
                print (var_p.var_value)
            else:
                eprint(var_p.var_value)
        else:       # else print string
            if (instruction == "WRITE"):
                print (instructions[idx].arg[0].text)
            else:
                eprint(instructions[idx].arg[0].text)
    if (instruction == "DEFVAR"):
        name = instructions[idx].arg[0].text[3:]
        frame = instructions[idx].arg[0].text[:2] # memory frame
        var = Variable(name, None, frame) # create new var
        duplicity = False
        for var_s in variables[0]:  # check if already exists
            if (var_s.var_name == name):
                    duplicity = True

        if (duplicity == False):
            if ((mem_frame == "tmp" and frame != "TF") or ((mem_frame == "glb" or mem_frame == "fnc") and frame == "TF")):  #chceck memory frme
                eprint("Error: Incorrect memory frame")
                sys.exit(55)
            variables[get_frame()].append(var)    # store to variables list
        else:
            eprint("Error: Redeclaration of variable !")
            sys.exit(32)

    if (instruction == "MOVE"):
        name = instructions[idx].arg[0].text[3:]    #variable name
        var_m = findvar(name, get_frame())
        m_type = instructions[idx].arg[1].atype
        if (m_type == "var"):   #move variable value
            v_name = instructions[idx].arg[0].text[3:]
            var_m1 = findvar(name, get_frame())
            var_m.var_value = var_m1.var_value
        else:   #move constat
            var_m.var_value = instructions[idx].arg[1].text

    if (instruction == "PUSHS"):
        p_type = instructions[idx].arg[0].atype
        if (p_type == "var"):   #push var value`
            name = instructions[idx].arg[0].text[3:]
            var_p = findvar(name, get_frame())
            stack.append(var_p.var_value)
        else:   #push constant
            stack.append(instructions[idx].arg[0].text)

    if (instruction == "POPS"):
        name = instructions[idx].arg[0].text[3:] #variable to store popped value
        var_p = findvar(name, get_frame())
        var_p.var_value = stack.pop()

    if (instruction == "CREATEFRAME"):
        mem_frame = "tmp"
        del variables[1][:] #delete all previous temporary variables
    if (instruction == "PUSHFRAME"):
        if (mem_frame != "tmp"):
            eprint("Error: No memory frame to push")
            sys.exit(55)
        mem_frame = "fnc"
    if (instruction == "POPFRAME"):
        if (mem_frame != "fnc"):
            eprint("Error: Cannot remove MAIN memory frame")
            sys.exit(55)
        mem_frame = "glb"
        del variable[2][:] #delete all used function variables

    #integer arithmetic instructions
    if (instruction == "ADD" or instruction == "SUB" or
        instruction == "MUL" or instruction == "IDIV" or
        instruction == "LT" or instruction == "GT" or
        instruction == "EQ" or instruction == "AND" or
        instruction == "OR"):
        name = instructions[idx].arg[0].text[3:]
        var_a = findvar(name, get_frame())
        symb1 = instructions[idx].arg[1].atype #type of symb1
        symb2 = instructions[idx].arg[2].atype #type of symb2

        if (symb1 == "var"):    #symb1 = variable
            var_s1 = findvar(instructions[idx].arg[1].text[3:], get_frame())
            val1 = int(var_s1.var_value)
        elif (symb1 == "int"):  #symb1 = integer
            val1 = int(instructions[idx].arg[1].text)
        else:
            eprint("Error: integer expected")
            sys.exit(53)

        if (symb2 == "var"):    #symb2 = variable
            var_s2 = findvar(instructions[idx].arg[2].text[3:], get_frame())
            val2 = int(var_s2.var_value)
        elif (symb2 == "int"):  #symb2 = integer
            val2 = int(instructions[idx].arg[2].text)
        else:
            eprint("Error: integer expected")
            sys.exit(53)

        if (instruction == "ADD"):
            var_a.var_value = val1 + val2
        elif (instruction == "SUB"):
            var_a.var_value = val1 - val2
        elif (instruction == "MUL"):
            var_a.var_value = val1 * val2
        elif (instruction == "IDIV"):
            if (val2 == 0):
                eprint("Error: division by zero")
                sys.exit(57)
            var_a.var_value = val1 / val2
        elif (instruction == "LT"):
            var_a.var_value = (val1 < val2)
        elif (instruction == "GT"):
            var_a.var_value = (val1 > val2)
        elif (instruction == "EQ"):
            var_a.var_value = (val1 == val2)
        elif (instruction == "AND"):
            var_a.var_value = (val1 & val2)
        elif (instruction == "OR"):
            var_a.var_value = (val1 | val2)

    if (instruction == "NOT"):
        name = instructions[idx].arg[0].text[3:]
        var_a = findvar(name, get_frame())
        symb1 = instructions[idx].arg[1].atype

        if (symb1 == "var"):    #symb1 = variable
            var_s1 = findvar(instructions[idx].arg[1].text[3:], get_frame())
            val1 = int(var_s1.var_value)
        elif (symb1 == "int"):  #symb1 = integer
            val1 = int(instructions[idx].arg[1].text)
        else:
            eprint("Error: integer expected")
            sys.exit(53)

        var_a.var_value = ~val1

    if (instruction == "STRI2INT" or instruction == "GETCHAR"
        or instruction == "SETCHAR"):
        name = instructions[idx].arg[0].text[3:]
        var_a = findvar(name, get_frame())
        symb1 = instructions[idx].arg[1].atype
        symb2 = instructions[idx].arg[2].atype

        if (symb1 == "var"):
            var_s1 = findvar(instructions[idx].arg[1].text[3:], get_frame())
            val1 = var_s1.var_value
        elif (symb1 == "string"):
            val1 = instructions[idx].arg[1].text
        else:
            eprint("Error: string expected")
            sys.exit(53)

        if (symb2 == "var"):
            var_s2 = findvar(instructions[idx].arg[2].text[3:], get_frame())
            val2 = int(var_s2.var_value)
        elif (symb2 == "int"):
            val2 = int(instructions[idx].arg[2].text)
        else:
            eprint("Error: integer expected")
            sys.exit(53)

        if ((val2 < 0) or ((val2 - 1) > len(val1))):    #chceck if index in string
            eprint("Error STRI2INT: index out of string bounds")
            sys.exit(58)
        if (instruction == "STRI2INT"):
            var_a.var_value = ord(val1[val2])   #return ordinal value of character
        elif (instruction == "GETCHAR"):
            var_a.var_value = val1[val2]
        else:
            var_a.var_value = val1[:val2] + var_a.var_value + val1[val2+1:]

    if (instruction == "INT2CHAR"):
        name = instructions[idx].arg[0].text[3:]
        var_a = findvar(name, get_frame())
        symb1 = instructions[idx].arg[1].atype

        if (symb1 == "var"):
            var_s1 = findvar(instructions[idx].arg[1].text[3:], get_frame())
            val1 = int(var_s1.var_value)
        elif (symb1 == "int"):
            val1 = int(instructions[idx].arg[1].text)
        else:
            eprint("Error: string expected")
            sys.exit(53)

        if ((val1 < 0) or (val1 > 255)):    #check if in range for chr() function
            eprint("Error  INT2CHAR: out of limit")
            sys.exit(58)
        var_a.var_value = chr(val1) #return character

    if (instruction == "TYPE"):
        name = atype.find('arg1').text[3:]
        var_t = findvar(name, get_frame())
        symb1 = atype.find('arg2').get('type')

        if (symb1 == "var"):
            var_s1 = findvar(atype.find('arg2').text[3:], get_frame())
            if (var_s1.var_value.isdigit()):
                var_t.var_value = "int"
            elif (var_s1.var_value == "True" or var_s1.var_value == "False"):
                var_t.var_value = "bool"
            elif (var_s1.var_value == None):
                var_t.var_value = ""
            else:
                var_t.var_value = "string"
        else:
            var_t.var_value = "constant"

    if (instruction == "READ"):
        name = instructions[idx].arg[0].text[3:]
        var_r = findvar(name, get_frame())
        var_type = instructions[idx].arg[1].text

        if (var_type == "int"): #read integer
            val_in = input()
            if  (val_in.isdigit()):
                var_r.var_value = int(val_in)
            else:   #if not integer enter implicit value
                var_r.var_value = 0
        elif (var_type == "string"):    #read string
            var_r.var_value = input()
        elif (var_type == "bool"):  #read bool
            bool_val = input()
            if (bool_val == "true"):
                var_r.var_value = True
            else:   #implicit value
                var_r.var_value = False
        else:
            eprint("Error READ: unknown type")
            sys.exit(32)

    if (instruction == "BREAK"):
        instr = instructions[idx].order
        print("**DEBUG INFO**\nInstructions executed:", instr, "\nCurrent memory frame:", mem_frame) #brief debug info

    if (instruction == "JUMP"):
        lbl_name = instructions[idx].arg[0].text
        idx = int(findlabel(lbl_name))  #change while cycle idx to match label instruction order

    if (instruction == "JUMPIFEQ" or instruction == "JUMPIFNEQ"):
        name = instructions[idx].arg[0].text
        lbl_j = int(findlabel(name))
        symb1 = instructions[idx].arg[1].atype
        symb2 = instructions[idx].arg[2].atype

        if (symb1 == "var"):
            var_s1 = findvar(instructions[idx].arg[1].text[3:], get_frame())
            val1 = int(var_s1.var_value)
        elif (symb1 == "int"):
            val1 = int(instructions[idx].arg[1].text)
        else:
            eprint("Error: integer expected")
            sys.exit(53)

        if (symb2 == "var"):
            var_s2 = findvar(instructions[idx].arg[2].text[3:], get_frame())
            val2 = int(var_s2.var_value)
        elif (symb2 == "int"):
            val2 = int(instructions[idx].arg[2].text)
        else:
            eprint("Error: integer expected")
            sys.exit(53)

        #test conditions according to instruction and execute jump
        if (instruction == "JUMPIFEQ"):
            if (val1 == val2):
                idx = lbl_j #change while cycle idx to match label instruction order
        else:
            if (val1 != val2):
                idx = lbl_j #change while cycle idx to match label instruction order

    if (instruction == "CALL"):
        lbl_name = instructions[idx].arg[0].text
        lbl_c = int(findlabel(lbl_name))    #get instruction ordr for jump
        c_idx = idx #get incremented instruction pointer
        call_stack.append(c_idx)    #store instruction pointer for return
        idx = lbl_c #set order for label

    if (instruction == "RETURN"):
        idx = int(call_stack.pop()) #return instruction pointer after last CALL

    if (instruction == "CONCAT"):
        name = instructions[idx].arg[0].text[3:]
        var_a = findvar(name, get_frame())
        symb1 = instructions[idx].arg[1].atype #type of symb1
        symb2 = instructions[idx].arg[2].atype #type of symb2

        if (symb1 == "var"):    #symb1 = variable
            var_s1 = findvar(instructions[idx].arg[1].text[3:], get_frame())
            val1 = var_s1.var_value
        elif (symb1 == "string"):  #symb1 = string
            val1 = instructions[idx].arg[1].text
        else:
            eprint("Error: integer expected")
            sys.exit(53)

        if (symb2 == "var"):    #symb2 = variable
            var_s2 = findvar(instructions[idx].arg[2].text[3:], get_frame())
            val2 = var_s2.var_value
        elif (symb2 == "string"):  #symb2 = string
            val2 = instructions[idx].arg[2].text
        else:
            eprint("Error: integer expected")
            sys.exit(53)

        var_a.var_value = val1 + val2;

    if (instruction == "STRLEN"):
        name = instructions[idx].arg[0].text[3:]
        var_a = findvar(name, get_frame())
        symb1 = instructions[idx].arg[1].atype #type of symb1

        if (symb1 == "var"):    #symb1 = variable
            var_s1 = findvar(instructions[idx].arg[1].text[3:], get_frame())
            val1 = var_s1.var_value
        elif (symb1 == "string"):  #symb1 = string
            val1 = instructions[idx].arg[1].text
        else:
            eprint("Error: integer expected")
            sys.exit(53)

        var_a.var_value = len(val1)

    idx += 1    #increment index to instructions array
