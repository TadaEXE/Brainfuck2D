grammar Brainfuck2D;

program: (instruction | loop)* EOF;

loop:'[' (instruction | loop)* ']'; 

instruction:
	'>'		# moveRight
	| '<'	# moveLeft
	| '+'	# increment
	| '-'	# decrement
	| '.'	# output
	| ','	# input
	| '/'	# moveUp
	| '\\'	# moveDown;

COMMENT: '#' ~[\r\n]* -> skip;
WS: [ \t\r\n]+ -> skip;