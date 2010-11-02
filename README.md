About
=====
Once upon a time, an austrian guy named Ben made a brainfuck interpreter in C++. I thought I want to make one as well, so I did. But because I wanted direct access to stack I wrote the interpreter itself in x86 assembler(nothing really smart was used) and wrapped with C.

Demo
====
	 % cat s.bf
	 ,---------------------------[+++++++++++++++++++++++++++.----------------------------]
	 [-]++++++++++.
	 [-]++++++++++++++++++++++++++++++++++++++++++++++++...+..+.
	 % echo Z | ./bin/main s.bf
	 Interpreting started. (buf: 0x87f2008; size: 162)
	 ZYXWVUTSRQPONMLKJIHGFEDCBA@?>=<;:9876543210/.-,+*)('&%$#"!
	 000112
	 Interpreter returned: 0xa. Time: 90654 nanoseconds
