0  LDI 1     	//load the value 1 into A-Register.
1  SUB 15    	//Subtract A-Register's contents in memory location 15.
2  JZ  8	//Jump to address 8 if the previous result was a zero.
3  ADD 15	//Add back contents of RAM at mem. loc. 15 to A-Register.
4  OUT		//Output A-Register contents on SSD.
5  ADD 14	//Add contents of mem. loc. 14 to A-Register.
6  JMP 1	//Jump to mem. loc. 1.
7  ADD 15	//Add back contents of RAM at mem. loc. 15 to A-Register.
8  OUT		//Output A-Register contents on SSD.
9  HLT		//Halt the computer.
.
.
.
14 1		//Value to increment count by.
15 57		//Value to stop at.
