#ifdef MINDBENDER_EXT

static std::string getDefaultArgumentName(int s)
{
	char result[128];
	sprintf(result, "__argname%i", s);
	return std::string(result);
}

void CLCompiler::eventStatement()
{
	// 'event' <identifier> ':' <expr> ['(' <identifier> ')'] [ ',' ... ]    '{' <statements> '}'
	//
	// Ex.: event scumm give: ego, ANY(receiver), ... { <blah> }
	//
	// -> syntactic sugar for:
	//
	// adv.AddEvent("scumm", "give", ego, ANY, function(__dummy0, receiver) { <blah> });
	//
	//
	// pushroot
	// pushstring "adv"
	// tabget
	// pushstring "AddEvent"
	// tabget				// on stack: function to call (1)
	// pushroot				// on stack: self value of function to call (2)
	// pushstring "scumm"			// on stack: first argument (3a)
	// pushstring "give"			// on stack: second argument (3b)
	// "push <ego>"				// on stack: result of expression 'ego' // (4)
	// "push <ANY>"				// on stack: result of expression 'ANY' // (4)
	// pushconst #				// on stack: event function // (5)
	// pushi 5				// on stack: number of arguments (3 + 2) // (6)
	// mcall				// calls function at adv.AddEvent
	// pop 1				// discard result of AddEvent function call

	addLineOp();

	expect(TOK_EVENT);
	fp->addInstruction(new CLIInstruction(OP_PUSHROOT));
	pushStringConstant("adv");
	fp->addInstruction(new CLIInstruction(OP_TABGET));
	pushStringConstant("AddEvent");
	fp->addInstruction(new CLIInstruction(OP_TABGET)); // (1)
	fp->addInstruction(new CLIInstruction(OP_PUSHROOT)); // (2)

	// get event class&name string
	if (l.tok != TOK_IDENTIFIER) error("parser", "Event statement: expected event class");
	std::string event_class = l.str; lex();

	if (l.tok != TOK_IDENTIFIER) error("parser", "Event statement: expected event event");
	std::string event_name = l.str; lex();

	pushStringConstant(event_class); // (3a)
	pushStringConstant(event_name); // (3b)

	expect(CLToken(':'));

	// new event function
	CLIFunction *event_fn = new CLIFunction(context);
	event_fn->beginBlock();
	
	// parse parameter list
	int argc = 0;
	bool first_arg = true;
	while (l.tok != CLToken('{'))
	{
		if (!first_arg) expect(CLToken(','));

		expressionExpr(); // (4)
		std::string arg_name = getDefaultArgumentName(argc);
		
		if (l.tok == ':')
		{
			lex();
			if (l.tok != TOK_IDENTIFIER) error("parser", "Event statement: expected identifier for event argument: arg:<identifier>");
			arg_name = l.str; lex();
		}

		event_fn->addParameter(arg_name);
		++argc;

		first_arg = false;
	}

	{	// compile event function into event_fn
		CLIFunction *old_fp = fp;
		fp = event_fn;

		// add debug info
		addFileOp(lexer.getFile());

		expect(CLToken('{'));
		while (l.tok != '}')
		{
			statement();
		} 
		fp->endBlock();
		expect(CLToken('}'));

		if (fp->needReturnGuard())
		{
			fp->addInstruction(new CLIInstruction(OP_PUSH0));
			fp->addInstruction(new CLIInstruction(OP_RET));
		}
		fp = old_fp;
	}	// done

	CLValue func = event_fn->generateFunction();
	delete event_fn;
	int f_id = fp->addConstant(func);
	fp->addInstruction(new CLIInstruction(OP_PUSHCONST, f_id));
	
	fp->addInstruction(new CLIInstruction(OP_PUSHI, argc+3)); // (6)
	fp->addInstruction(new CLIInstruction(OP_MCALL)); 
	addFileOp(lexer.getFile());
	fp->addInstruction(new CLIInstruction(OP_POP, 1)); 
}

#endif

