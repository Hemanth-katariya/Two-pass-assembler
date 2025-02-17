/*****************************************************************************
TITLE: Emulator
AUTHOR: HEMANTH KUMAR KATARIYA
ROLLNO: 2301CS22
Declaration of Authorship
This cpp file, emu.cpp, is part of the miniproject of CS2102 at the 
department of Computer Science and Engg, IIT Patna. 
*****************************************************************************/

#include <bits/stdc++.h>
using namespace std;

// Global variables representing the emulator's state
int pc;                  // Program Counter
int a = 0, b = 0;        // Registers 'a' and 'b'
int sp = 0;              // Stack Pointer
int cnt = 0;             // Instruction counter
vector<int> instructions; // Stores the list of machine code instructions
int memory[20000005];    // Memory array for storage
vector<string> output;   // Stores output trace logs
string fileName = "";    // File name for output trace file

// Function to convert a decimal number to an 8-character hexadecimal string
string dec2hex(int number) {
    unsigned int num = number;  // Treats the number as unsigned for positive values
    string ans = "";            // Resulting hexadecimal string

    for (int i = 0; i < 8; i++) {  // Create an 8-character hex representation
        int remainder = num % 16;
        // If remainder is 0-9, add '0' to '9'; if 10-15, add 'A' to 'F'
        ((remainder <= 9) ? ans += char(remainder + '0') : ans += char(remainder - 10 + 'A'));
        num /= 16;
    }
    reverse(ans.begin(), ans.end());  // Reverse to get correct hex format
    return ans;
}

// Function to convert a hexadecimal string to a decimal integer
int hex2Dec(string s) {
    int ans = 0, pow = 1;             // Initialize answer and power for 16^0, 16^1, etc.
    for (int i = s.size() - 1; i >= 0; i--) { // Process each character from right to left
        if (s[i] >= '0' && s[i] <= '9') {
            ans += pow * (s[i] - '0');         // Convert '0'-'9' to integer
        } else {
            ans += pow * ((tolower(s[i]) - 'a') + 10); // Convert 'A'-'F' to 10-15
        }
        pow *= 16;                             // Increase power of 16
    }
    return ans;
}

// Function to generate the output trace file
void produceFile() {
    ofstream out(fileName + ".txt"); // Create a file with the base name
    for (auto line : output) {
        out << line;                 // Write each output line to the file
    }
    out.close();
    cout << "Output file generated" << endl;
}

// Function to execute instructions and trace their execution
void traceFunc(int howMuch) { 
    pc = 0;                     // Reset Program Counter and other registers
    cnt = 0;
    a = b = sp = 0;

    while (pc < instructions.size()) {  // Loop through each instruction
        cnt++;
        if (pc >= instructions.size() || cnt > (1 << 24)) { // If infinite loop or memory error
            cout << "Segmentation Fault\n";
            output.push_back("Segmentation Fault\n");
            exit(0);               // Terminate the program on error
        }

        // Extract the opcode by converting the current instruction to hexadecimal
        string mCode = dec2hex(instructions[pc]);
        int opcode = hex2Dec(mCode.substr(6)); // Get the opcode from last two hex digits

        if (opcode == 18) {        // HALT opcode
            cout << "HALT found" << endl;
            output.push_back("HALT found\n");
            break;
        }

        int offset = 0;            // Initialize offset for each instruction
        int k = 8;
        while (k < 31) {           // Extract the offset value from instruction bits
            if (instructions[pc] & (1 << k)) {
                int temp = pow(2, k - 8);
                offset += temp;
            }
            k++;
        }
        if (instructions[pc] & (1 << 31)) {  // Check if offset is negative
            int temp = pow(2, 23);
            offset -= temp;
        }

        // Execute instruction based on the opcode
        switch (opcode) {
            case 0:  // LOAD immediate
                b = a;
                a = offset;
                break;

            case 1:  // ADD immediate
                a = a + offset;
                break;

            case 2:  // LOAD from memory
                b = a;
                a = memory[sp + offset];
                break;

            case 3:  // STORE to memory
                memory[sp + offset] = a;
                a = b;
                break;

            case 4:  // LOAD from address
                a = memory[a + offset];
                break;

            case 5:  // STORE to address
                memory[a + offset] = b;
                break;

            case 6:  // ADD registers
                a = b + a;
                break;

            case 7:  // SUBTRACT registers
                a = b - a;
                break;

            case 8:  // SHIFT left
                a = b << a;
                break;

            case 9:  // SHIFT right
                a = b >> a;
                break;

            case 10: // UPDATE stack pointer
                sp = sp + offset;
                break;

            case 11: // LOAD SP into A
                sp = a;
                a = b;
                break;

            case 12: // SAVE A into SP
                b = a;
                a = sp;
                break;

            case 13: // JUMP to offset
                b = a;
                a = pc;
                pc += offset;
                break;

            case 14: // JUMP back
                pc = a;
                a = b;
                break;

            case 15: // JUMP if zero
                if (!a) pc += offset;
                break;

            case 16: // JUMP if negative
                if (a < 0) pc += offset;
                break;

            case 17: // Unconditional JUMP
                pc += offset;
                break;
        }

        // Log the current state of registers and PC
        cout << "PC= " << dec2hex(pc) << " SP= " << dec2hex(sp) << " A= " << dec2hex(a) << " B= " << dec2hex(b) << "\n";
        string out = "PC= " + dec2hex(pc) + " SP= " + dec2hex(sp) + " A= " + dec2hex(a) + " B= " + dec2hex(b) + "\n";
        output.push_back(out);
        pc++; // Move to next instruction
    }

    // Final log entry with total instructions executed
    cout << cnt << " instructions were executed" << endl;
    string out = to_string(cnt) + " instructions were executed";
    output.push_back(out);
    produceFile();
}

// Function to display memory before execution
void memBefore() {
    cout << "Memory dump before execution\n";
    for (int i = 0; i <= instructions.size() - 1; i += 4) {
        cout << dec2hex(i) << " ";
        for (int j = i; j < min(i + 4, (int)instructions.size()); ++j) {
            cout << dec2hex(instructions[j]) << " ";
        }
        cout << "\n";
    }
}

// Function to display memory after execution
void memAfter() {
    cout << "Memory dump after execution\n";
    for (int i = 0; i <= instructions.size() - 1; i += 4) {
        cout << dec2hex(i) << " ";
        for (int j = i; j < min((int)instructions.size(), i + 4); ++j) {
            cout << dec2hex(memory[j]) << " ";
        }
        cout << endl;
    }
}

int main() {
    string inputFile;
    cout << "Enter file name with extension \n";
    cin >> inputFile;

    ifstream in;
    int read;
    int counter = 0;
    fileName = inputFile;

    // Remove file extension from fileName for output file
    for (int i = 0; i < 2; i++) {
        fileName.pop_back();
    }

    // Open and read the binary machine code file
    in.open(inputFile, ios::in | ios::binary);
    while (in.read((char*)&read, sizeof(int))) {
        memory[counter] = read;
        counter++;
        instructions.push_back(read);
    }
    in.close();

    // Command options for the emulator
     string s ="";
    cout<<"Enter the command you want to execute \n";                                   //all the commands implemented
    cout<<"To run the whole program : -trace \n";
    cout<<"To view memory dump before execution : -before \n";
    cout<<"To view memory dump after execution : -after\n";
    while (true)
    {
        cout<<"\nEnter the command or 'exit' to exit \n";
        cin>>s;
        if (s == "-trace")                    //run whole program
        {
            traceFunc(0);
        }
        else if(s == "-before"){            //show memory before
            memBefore();
        }
        else if(s == "-after"){             //show memory after
            memAfter();
        }
        else if(s == "exit"){
            exit(0);                        //exit if command is zero
        }
        else
        {
            "Invalid Command \n";           //if no match then invalid command
        }
    }
}