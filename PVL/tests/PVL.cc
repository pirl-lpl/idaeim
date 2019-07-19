/*	PVL

Copyright (C) 2002 - 2013  Bradford Castalia

This file is part of the idaeim libraries.

The idaeim libraries are free software; you can redistribute them and/or
modify them under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

The idaeim libraries are distributed in the hope that they will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser
General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.

CVS ID: PVL.cc,v 1.16 2013/02/24 00:50:49 castalia Exp
*******************************************************************************/

#include	"PVL.hh"
using namespace idaeim;
using namespace PVL;

#include	<string>
#include	<cctype>
#include	<iostream>
#include	<fstream>
#include	<cstdlib>
using namespace std;



string
	Filename;
bool
	Quiet				= false,
	Strict_Parse		= false,
	Strict_Write		= false,
	Show_Warnings		= false,
	Verbatim_Strings	= false,
	Crosshatch_Comments	= true,
	Case_Sensitive		= false,
	String_Continuation = true;
vector<string>
	Find_Paths;

void get_PVL (istream&);
bool not_argument (string&);
void usage (const char*);

#define EXIT_SUCCESS 0




/**	PVL

	Examination of PVL source files.

	Use:

		PVL [[<option> [...]] [<filename>]] ...

	Description:

	A source of PVL statements is parsed. If no filename is specified
	stdin is used. The single character '-' indicates stdin, which may
	only be specified once. Each source is parsed in the order it is
	listed. The PVL found becomes the current parameter set. The
	options are applied in the order listed. If no options are
	specified the default is to list all parameters. All options
	encountered before a filename - options remain in effect until
	changed - apply to the parameter set from that file.

	Options:

	-[Not_]Strict[_In | _Out]

		Strict PVL syntax is (not) enforced for input (parsing) and/or
		output (listing). Default: Not_Strict for both In and Out.

	-[No_]Warnings

		Warnings will (not) be listed. Default: No_Warnings.

	-[No_]Crosshatch_comments

		Lines starting with a '#' character are (not) comments.
		Default: Crosshatch_comments (Not if Strict)

	-[No_]Verbatim_strings

		Do (not) ignore formatting escape characters in quoted strings.
		Default: No_Verbatim_strings

	-[Not_]Quiet

		Do (not) supress filename listing. Default: Not_Quiet.

	-[No_]Find [<pathname>]

		Do (not) find parameters at the pathname. Multiple -find
		pathname options may be specified and each pathname specified
		will be accumulated; all search for all accumulated pathnames
		will be applied to the PVL from the following files. A no_find
		option clears all accumulated pathnames. Default: No_Find.

	-[Not_]Case_Sensitive

		Find parameter names is (not) case-sensitive. Default:
		Not_Case_Sensitive

	-Help

		List the usage description. If this option is specified
		anywhere on the command line all other options are ignored.


	Author:

	Bradford Castalia, idaeim studio
*/
int
main
	(
	int		count,
	char	**arguments
	)
{
bool
	positive;
int
	cin_argument = 0;
string::size_type
	index;

for (int argument = 1;
	 argument < count;
	 argument++)
	if (*arguments[argument] == '-' &&
		toupper (*(arguments[argument] + 1)) == 'H')
		usage (*arguments);

for (int argument = 1;
	 argument < count;
	 argument++)
	{
	string
		Argument = arguments[argument];
	if (Argument.empty ())
		continue;
	if (Argument[0] == '-')
		{
		Argument.erase (0, 1);
		if (Argument.empty ())
			{
			if (cin_argument)
				{
				cerr << *arguments
					 << ": Only one stdin source is allowed." << endl
					 << "  Arguments " << cin_argument << " and "
					 << argument << " both specify stdin source;" << endl
					 << "  Argument " << argument << " ignored." << endl;
				continue;
				}
			Filename.clear ();
			cin_argument = argument;
			get_PVL (cin);
			continue;
			}
		positive = ! not_argument (Argument);
		switch (toupper (Argument[0]))
			{
			case 'F':
				if (positive && ++argument < count)
					{
					Find_Paths.push_back (string (arguments[argument]));
					}
				else
					{
					Find_Paths.clear ();
					}
				break;
			case 'W':	//	Warnings
				Show_Warnings = positive && true;
				break;
			case 'V':	//	Verbatim strings
				Verbatim_Strings = positive && true;
				break;
			case 'C':	//	Crosshatch comments
				index = Argument.find ('_');
				if (index < Argument.length () &&
					toupper (Argument[++index]) == 'S')
					{
					Case_Sensitive = positive && true;
					}
				else
					{
					Crosshatch_Comments = positive && true;
					}
				break;
			case 'Q':	//	Quiet
				Quiet = positive && true;
				break;
			case 'S':
				index = Argument.find ('_');
				if (index < Argument.length ())
					{
					if (toupper (Argument[++index]) == 'I')
						{
						//	Strict_In
						Strict_Parse = positive && true;
						break;
						}
					else if (toupper (Argument[index]) == 'O')
						{
						//	Strict_Out
						Strict_Write = positive && true;
						break;
						}
					else if (toupper (Argument[index]) == 'C')
						{
						//	String_Continuation
						String_Continuation = positive && true;
						break;
						}
					}
				else
					{
					//	Strict
					Strict_Parse = positive && true;
					Strict_Write = positive && true;
					break;
					}
				//	Fall trhough to usage.
			default:
				cerr << *arguments
					 << ": Unknown option \"" << arguments[argument]
					 << '"' << endl;
				usage (*arguments);
			}
		}
	else
		{
		Filename = Argument;
		ifstream
			input_file (Filename.c_str ());
		if (! input_file)
			cerr << "Unable to open the input file: " << Filename << endl;
		else
			get_PVL (input_file);
		}
	}
if (! cin_argument && Filename.empty ())
	get_PVL (cin);

exit (EXIT_SUCCESS);
}


void
get_PVL (istream& input_file)
{
if (! Quiet)
	{
	cout << "===>>> ";
	if (Filename.empty ())
		cout << "(stdin)" << endl;
	else
		cout << Filename << endl;

	cout << boolalpha
		<< "--->>> " << "     Strict Parsing: " << Strict_Parse << endl
		<< "--->>> " << "     Strict Writing: " << Strict_Write << endl
		<< "--->>> " << "   Verbatim Strings: " << Verbatim_Strings << endl
		<< "--->>> " << "Crosshatch Comments: " << Crosshatch_Comments << endl
		<< "--->>> " << "String Continuation: " << String_Continuation << endl;
	if (! Find_Paths.empty ())
		cout
		<< "--->>> " << "     Case Sensitive: " << Case_Sensitive << endl;
	cout << endl;
	}
Parser
	parser (input_file);
parser.strict (Strict_Parse);
parser.verbatim_strings (Verbatim_Strings);
parser.commented_lines (Crosshatch_Comments);
parser.string_continuation (String_Continuation);
try
	{
	Parameter
		*parameters = parser.get_parameters ();
	if (Show_Warnings)
		{
		Parser::Warnings_List
			*warnings_list = parser.warnings ();
		if (! warnings_list->empty ())
			{
			for (Parser::Warnings_List::const_iterator
					warning = warnings_list->begin ();
				 warning != warnings_list->end ();
				 ++warning)
				cout << (*warning)->message () << endl << endl;
			}
		}
	Lister
		lister (cout);
	lister.strict (Strict_Write);
	if (Find_Paths.empty ())
		lister << *parameters << endl;
	else
		{
		for (vector<string>::iterator
				pathname =  Find_Paths.begin ();
				pathname != Find_Paths.end ();
				++pathname)
			{
			for (Aggregate::depth_iterator
					element =  ((Aggregate&)(*parameters)).begin_depth ();
					element != ((Aggregate&)(*parameters)).end ();
					++element)
				{
				if (element->at_pathname (*pathname, Case_Sensitive))
					{
					element->comment ("");	//	Drop the comments.
					cout << element->pathname_to (parameters);
					lister << *element;
					}
				}
			}
		}
	delete parameters;
	}
catch (const Exception& except)
	{
	cout << "PVL Exception:" << endl
		 << except.message () << endl;
	exit (-1);
	}
catch (out_of_range except)
	{
	cout << "out_of_range - " << endl
		 << except.what () << endl;
	}
}


bool
not_argument
	(
	string&		argument
	)
{
if (argument.empty ())
	{
	return false;
	}

bool
	is_not = false;
string
	test_argument;
string::size_type
	index = 0;
while (index < argument.length ())
	test_argument += toupper (argument[index++]);


if (test_argument.length () > 4 &&
   (test_argument.find ("NOT_") == 0 ||
	test_argument.find ("NOT-") == 0))
	{
	argument.erase (0, 4);
	is_not = true;
	}
else
if (test_argument.length () > 3 &&
   (test_argument.find ("NO_") == 0 ||
	test_argument.find ("NO-") == 0))
	{
	argument.erase (0, 3);
	is_not = true;
	}
else
if (test_argument.length () > 2 &&
   (test_argument.find ("N_") == 0 ||
	test_argument.find ("N-") == 0))
	{
	argument.erase (0, 2);
	is_not = true;
	}

return is_not;
}


void
usage
	(
	const char	*program
	)
{
cout << "Usage: " << program << " [[<option> [...]] [<filename>]] ..." << endl
	 << "  options -" << endl
	 << "    -[Not_]Strict[_In | _Out]" << endl
	 << "      Strict PVL syntax is (not) enforced for input (parsing)" << endl
	 << "        and/or output (writing)." << endl
	 << "      Default: Not_Strict for both In and Out." << endl
	 << "    -[No_]Warnings" << endl
	 << "      Warnings will (not) be listed." << endl
	 << "      Default: No_Warnings." << endl
	 << "    -[No_]Crosshatch_comments" << endl
	 << "      Lines starting with a '#' character are (not) comments." << endl
	 << "      Default: Crosshatch_comments (Not if Strict)" << endl
	 << "    -[No_]Verbatim_strings" << endl
	 << "      Do (not) ignore formatting escape characters in quoted strings." << endl
	 << "      Default: No_Verbatim_strings" << endl
	 << "    -[No_]String_Continuation" << endl
	 << "      Do (not) recognize the end of line string continution delimiter." << endl
	 << "      Default: String_Continuation" << endl
	 << "    -[No_]Find [<pathname>]" << endl
	 << "      Do (not) find parameters at the pathname." << endl
	 << "        Multiple -find pathname options will be accumulated." << endl
	 << "        A no_find option clears all accumulated pathnames." << endl
	 << "      Default: No_Find." << endl
	 << "    -[Not_]Case_Sensitive" << endl
	 << "      Find parameter names is (not) case-sensitive." << endl
	 << "      Default: Not_Case_Sensitive" << endl
	 << "    -[Not_]Quiet"  << endl
	 << "      Do (not) supress filename and modes listing." << endl
	 << "      Default: Not_Quiet." << endl
	 << "    -Help" << endl
	 << "      List the usage description. If this option is specified" << endl
	 << "        anywhere on the command line all other options are ignored." << endl
	 << "  filename -" << endl
	 << "    A file to be parsed for PVL syntax." << endl
	 << "      If no filenames are specified, stdin is parsed." << endl
	 << "      The single character '-' indicates stdin, which may only be" << endl
	 << "        specified once." << endl
	 << "      Files are parsed as they are encountered on the command line." << endl
	 << "      Any and previous options apply to the current file; options" << endl
	 << "        remain in effect until changed." << endl;
exit (2);
}
