Zachary Colby
Michael Wahlberg
Compiler project (Module 4)
COP 3402
Summer C 2015

Execution instructions. 

Open terminal and go to directory containing the source files and input file.

Execute the following command to compile the drive:
	gcc driver.c -o drive
	or
	gcc driver.c -lm -o drive

The driver handles the compilation of the Scanner, Parser, and VM through its execution.
Execute the driver through the following command:
	./drive

The driver allows the user to see the output of each component to the terminal using three flags:
	-a: Parser
	-l: Scanner
	-v: VM

There is no required order for the flags, and any or all of them can be used at once.
