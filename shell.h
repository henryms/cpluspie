#ifndef  SHELL_H
#define SHELL_H

class shell_c
{
	public:
	shell_c ();
	void run (stringstream& ss );
	void sh(stringstream& ss);
	bool show_pdflatex_error(string makefile_path, editor_c& edit, std::stringstream& ss);
	bool show_compiler_error(string makefile_path, editor_c& edit, std::stringstream& ss);
	bool envstr(string& s);
};

#endif