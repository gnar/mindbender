#ifdef AGENT_EXT

void CLCompiler::agentConstructorExpr()
{
	addLineOp();

	std::string agent_id = "<unnamed agent>";

	// I. Parse optional agent id
	if (l.tok == TOK_STRING)
	{
		agent_id = l.str;
		lex();
	}

	// II. call aibo.__create_agent("id")
	// a) Get function clvalue
	fp->addInstruction(new CLIInstruction(OP_PUSHROOT));
	pushStringConstant("aibo");
	fp->addInstruction(new CLIInstruction(OP_TABGET));
	pushStringConstant("__create_agent");
	fp->addInstruction(new CLIInstruction(OP_TABGET)); // on stack: function "__create_agent"

	// b) Execute it: __create_agent("id")
	fp->addInstruction(new CLIInstruction(OP_PUSHROOT)); // on stack: fn, self (=root)
	fp->addInstruction(new CLIInstruction(OP_PUSHS, agent_id)); // on stack: fn, self, "agent_id"
	fp->addInstruction(new CLIInstruction(OP_PUSHI, 1)); // on stack: fn, self, "id", argc (=1)
	fp->addInstruction(new CLIInstruction(OP_MCALL)); // on stack: function result (the agent class)

	// III. Handle table
	expect(CLToken('['));
	while (l.tok != CLToken(']'))
	{
		switch (l.tok)
		{
			// This all is copied from tableConstructorExpr() in clcompiler.cpp ///////////////
			case TOK_IDENTIFIER: // id = <expr> syntax
			{
				fp->addInstruction(new CLIInstruction(OP_DUP, 0));
				pushStringConstant(l.str); 
				lex(); // TOK_IDENTIFIER
				expect(CLToken('='));
				expressionExpr();
				fp->addInstruction(new CLIInstruction(OP_TABSET));
				fp->addInstruction(new CLIInstruction(OP_POP, 1));
				break;
			}

			case '[': // [<expr>] = <expr> syntax
			{
				lex(); // '['
				fp->addInstruction(new CLIInstruction(OP_DUP, 0));
				expressionExpr(); // key expr
				expect(CLToken(']'));
				expect(CLToken('='));
				expressionExpr(); // value expr
				fp->addInstruction(new CLIInstruction(OP_TABSET));
				fp->addInstruction(new CLIInstruction(OP_POP, 1));
				break;
			}

			// syntax sugar for member functions, also states
			case TOK_STATE:
			case TOK_FUNCTION:
			{
				bool is_state = (l.tok == TOK_STATE); lex(); // accept TOK_FUNCTION/TOK_STATE
				
				if (l.tok != TOK_IDENTIFIER) {
					if (is_state) error("parse", "need identifer after 'state' in agent constructor");
					else          error("parse", "need identifer after 'function' in agent constructor");
				}
				std::string func_id = l.str; std::string &state_id = func_id;
				lex(); // accept TOK_IDENTIFIER

				// compile function (argument list + body)
				CLIFunction *ifn = compileFunction(false);
				CLValue func = ifn->generateFunction();
				delete ifn;

				// add function object to parent function's constant list
				int f_id = fp->addConstant(func);

				// add to table, to make state accessible like a member-function
				fp->addInstruction(new CLIInstruction(OP_DUP, 0)); // push agent-class 
				fp->addInstruction(new CLIInstruction(OP_PUSHS, func_id)); // push function id/name
				fp->addInstruction(new CLIInstruction(OP_PUSHCONST, f_id)); // push function onto stack
				fp->addInstruction(new CLIInstruction(OP_TABSET));
				fp->addInstruction(new CLIInstruction(OP_POP, 1)); // discard tabset result

				if (is_state)
				{
					// for states, register "id"/fn - combo to agent-class
					// by calling aibo.__add_agent_state(agent-class, "state-id", state-fn)
					
					// a) Get function clvalue
					fp->addInstruction(new CLIInstruction(OP_PUSHROOT));
					pushStringConstant("aibo");
					fp->addInstruction(new CLIInstruction(OP_TABGET));
					pushStringConstant("__add_agent_state");
					fp->addInstruction(new CLIInstruction(OP_TABGET)); // on stack: function "__create_agent"
					
					// b) Execute it: __add_agent_state(agent-class, "state-id", state-fn)
					fp->addInstruction(new CLIInstruction(OP_PUSHROOT));       // self
					fp->addInstruction(new CLIInstruction(OP_DUP, 2));         // args[0] = agent-class
					fp->addInstruction(new CLIInstruction(OP_PUSHS, state_id));// args[1] = "state-id"
					fp->addInstruction(new CLIInstruction(OP_PUSHCONST, f_id));// args[2] = state-fn
					fp->addInstruction(new CLIInstruction(OP_PUSHI, 3));       // argc
					fp->addInstruction(new CLIInstruction(OP_MCALL));  
					fp->addInstruction(new CLIInstruction(OP_POP, 1));         // discard result
				}
				break;
			}
			
			default: error("parse", "Expected key identifier or function keyword or state keyword in agent constructor");
		}

		if (l.tok == CLToken(',')) lex(); // optional comma
	}
	expect(CLToken(']'));
}

void CLCompiler::transitionStatement()
{
	// transition(state_id)
	// 
	//  I. call aibo.__agent_transition("state_id")
	// II. do yield(null)

	lex(); // accept TOK_TRANSITION
	expect(CLToken('('));
	if (l.tok != TOK_IDENTIFIER) error("parse", "need state identifer after 'transition' keyword!");
	std::string state_id = l.str; lex();
	expect(CLToken(')'));
	
	//  I. call __agent_transition("state_id")
	fp->addInstruction(new CLIInstruction(OP_PUSHROOT));
	pushStringConstant("aibo");
	fp->addInstruction(new CLIInstruction(OP_TABGET));
	pushStringConstant("__agent_transition");
	fp->addInstruction(new CLIInstruction(OP_TABGET));   // on stack: fn
	fp->addInstruction(new CLIInstruction(OP_PUSHROOT)); // self
	pushStringConstant(state_id);                        // args[0] = "state_id"
	fp->addInstruction(new CLIInstruction(OP_PUSHI, 1)); // argc=1
	fp->addInstruction(new CLIInstruction(OP_MCALL));
	fp->addInstruction(new CLIInstruction(OP_POP, 1)); // dispose of fn result
	
	// II. do yield
	fp->addInstruction(new CLIInstruction(OP_PUSH0));
	fp->addInstruction(new CLIInstruction(OP_YIELD));
}

void CLCompiler::terminateStatement()
{
	// terminate()
	// 
	//  I. call aibo.__agent_terminate(..)
	// II. do yield(null)

	//  I. call __agent_terminate()
	//       or __agent_terminate(<expr>)
	fp->addInstruction(new CLIInstruction(OP_PUSHROOT));
	pushStringConstant("aibo");
	fp->addInstruction(new CLIInstruction(OP_TABGET));
	pushStringConstant("__agent_terminate");
	fp->addInstruction(new CLIInstruction(OP_TABGET));   // on stack: fn
	fp->addInstruction(new CLIInstruction(OP_PUSHROOT)); // self

	lex(); // accept TOK_TRANSITION
	expect(CLToken('('));
	if (l.tok != CLToken(')')) {
		expressionExpr();
		fp->addInstruction(new CLIInstruction(OP_PUSHI, 1)); // argc=1
	} else {
		fp->addInstruction(new CLIInstruction(OP_PUSHI, 0)); // argc=0
	}
	expect(CLToken(')'));
	
	fp->addInstruction(new CLIInstruction(OP_MCALL));
	fp->addInstruction(new CLIInstruction(OP_POP, 1)); // dispose of fn result
	
	// II. do yield
	fp->addInstruction(new CLIInstruction(OP_PUSH0));
	fp->addInstruction(new CLIInstruction(OP_YIELD));
}

void CLCompiler::delegateStatement()
{
	/*
	 * Syntax:
	 *   delegate(<agent-expr>)
	 * -or-
	 *   delegate(<agent-expr>; <list-expr>)
	 *
	 *   Examples:
	 *     delegate(Goalie)
	 *     delegate(GoToPose; [x=5 y=20 angle=45])
	 */
	
	lex(); // accept TOK_DELEGATE
	expect(CLToken('('));

	bool has_list = false;

	fp->addInstruction(new CLIInstruction(OP_PUSHROOT));
	pushStringConstant("aibo");
	fp->addInstruction(new CLIInstruction(OP_TABGET));
	pushStringConstant("__agent_delegate");
	fp->addInstruction(new CLIInstruction(OP_TABGET));   // on stack: fn
	fp->addInstruction(new CLIInstruction(OP_PUSHROOT)); // self
	expressionExpr(); // push <agent-expr> onto stack
	if (l.tok == CLToken(';') || l.tok == CLToken(','))
	{
		lex(); // accept ';' or ','
		expressionExpr(); // push <list-expr> onto stack
		has_list = true;
	} 
	fp->addInstruction(new CLIInstruction(OP_PUSHI, has_list ? 2 : 1)); // argc
	fp->addInstruction(new CLIInstruction(OP_MCALL));
	fp->addInstruction(new CLIInstruction(OP_POP, 1)); // dispose of fn result
	
	expect(CLToken(')'));
}

void CLCompiler::parseDelegateSpecifier()
{
	// state xyz(...) : agent_specifier
	// {
	//   ...
	// }
	//if (inside_state_definition)
	//{
	//	expect(CLToken(':'));
	//	if (l.tok != TOK_IDENTIFIER) error("parse", "Need delegate identifier after state definition")
	//	std::string &delegate_id = l.str; lex();
	//}
}

#endif

