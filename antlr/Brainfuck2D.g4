grammar Brainfuck2D;

program: instruction* EOF;

instruction:
	'>'		# moveRight
	| '<'	# moveLeft
	| '+'	# increment
	| '-'	# decrement
	| '.'	# output
	| ','	# input
	| '['	# loopStart
	| ']'	# loopEnd
	| '/'	# moveUp
	| '\\'	# moveDown;

COMMENT: .~[><+\-.,/[\]\\]* -> skip;
WS: [ \t\r\n]+ -> skip;