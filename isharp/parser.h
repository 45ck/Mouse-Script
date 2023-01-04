#pragma once
#include "stdafx.h"
#include <iostream>
#include "variableHelper.h"
using namespace std;

class PF { // Parser Functions are used to parse lexor code into runtime code.
public:
	static string splitColinGet(string token, int index) {
		if (token.find(":") == string::npos) return "NULL";
		vector<string> splitColin;
		splitColin.resize(20);
		splitColin = helperclasses::split(token, ':');
		if (index == 1) {
			string return_ = helperclasses::arrayPassArgs(splitColin, 0, ':');
			return return_;
		}
		return splitColin[index];
	}

	static string split(string token, char delimer, int index) {
		if (token.find(delimer) == string::npos) return "NULL";
		vector<string> splitColin;
		splitColin.resize(20);
		splitColin = helperclasses::split(token, delimer);
		return splitColin[index];
	}

	static string removeQuotes(string toRemove) {
		toRemove.erase(0, 1);
		toRemove.erase(toRemove.size() - 1);
		return toRemove;
	}

	static void printVector(vector<string> vs) {
		for each (string a in vs)
		{
			cout << a << endl;
		}
	}
};


class var_manager {
private:

	bool systemVars(string name, string content) {
		try {
			size_t t;
			if (content[0] == '"') content = PF::removeQuotes(content);
			if (name == "$using") {
				if (content == "Console") {
					usingConsole = true;

				} 
				else if (content == "Console_Extra") {
					usingConsole = true;
				}
				else if (content == "Random") {
					usingRandom = true;
				}
			}
			else if (usingConsole) {
				if (name == "$Console_Hidden") {
				if (content == "true") {
					ShowWindow(GetConsoleWindow(), SW_HIDE);
				}
				else if (content == "false") {
					ShowWindow(GetConsoleWindow(), SW_SHOW);
				}
				else {
					helperclasses::error("The system variable \"$Console_Hidden\" does not recognise the value of \"" + content + "\". This variable is a bool (\"true\" or \"false\").");
				}
				return true;
			}
				else if (name == "$Console_Font_Name") {
				font_name = content;
				return true;
				}
				else if (name == "$Console_Font_Size") {
				font_size = content;
				return true;
			}
				else if (name == "$Console_Cursor_Pos_X") {
				COORD cord;
				int temp_number = atoi(content.c_str());
				cord.X = temp_number;
				SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cord);

				return true;
			}
				else if (name == "$Console_Cursor_Pos_Y") {
				COORD cord;
				int temp_number = atoi(content.c_str());
				cord.Y = temp_number;
				SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cord);

				return true;
			}
				/*else if (name == "$Console_Title") {
					SetConsoleTitle(content.c_str()));
					title_name = content;
				}*/
				else if (name == "$Console_Width") {
					console_functions::setConsoleWidth(atoi(content.c_str()));
				}
				else if (name == "$Console_Height") {
					console_functions::setConsoleHeight(atoi(content.c_str()));
				}
				else {
					return false;
				}
			}
			else {
				return false;
			}
		}
		catch (exception e) {
			helperclasses::error("A fatal error has occured when trying to set a system variable (name = " + name + ", content = " + content + ").");
			return false;
		}
	}


	string getSystemVars(string name) {
		if (usingConsole) {
			if (name == "$Console_Hidden") {
				if (IsWindowVisible(GetConsoleWindow()) != FALSE) {
					return "false";
				}
				else {
					return "true";
				}
			}
			else if (name == "$Console_Font_Name") {
				return font_name;
			}
			else if (name == "$Console_Font_Size") {
				return font_size;
			}
			else if (name == "$Console_Cursor_Pos_X") {
				HANDLE h = GetConsoleWindow();
				COORD coord = helperclasses::GetConsoleCursorPosition(h);
				return to_string(coord.X);
			}
			else if (name == "$Console_Cursor_Pos_Y") {
				HANDLE h = GetConsoleWindow();
				COORD coord = helperclasses::GetConsoleCursorPosition(h);
				return to_string(coord.Y);
			}
			else if (name == "$Console_Title") {
				return title_name;

				//ERRORS.exe
				/*
				TCHAR szOldTitle[MAX_PATH];
				DWORD t = GetConsoleTitle(szOldTitle, MAX_PATH);
				char * procID = "";
				sprintf(procID, "%d", t);
				string toReturn = procID;
				delete procID;
				return toReturn;
				*/
			}
			else if (name == "$Console_Width") {
				return to_string(console_functions::getConsoleWidth());
			}
			else if (name == "$Console_Height") {
				return to_string(console_functions::getConsoleHeight());
			}
			else {
				return "";
			}
		}
		else {
			return "";
		}
	}

public:
	bool usingConsole = false;
	bool usingConsoleExtra = false;
	bool usingRandom = false;


	string title_name = "Untilted Mouse Scirpt";
	string font_name = "Lucidia Console";
	string font_size = "14";


	map<string, string> variables;
	void addVar(string name, string content) {
		if (systemVars(name, content)) return; // if it is a system variable than return.

		if (variables.find(name) != variables.end()) {
			variables[name] = content;
		}
		else {
			variables.insert(make_pair(name, content));
		}
	}

	string getVar(string name) {
		if (getSystemVars(name) != "") return getSystemVars(name);

		if (variables.find(name) == variables.end()) helperclasses::error( "The variable \"" + name + "\" does not exist in the current context."); // means theres none
		else {
			return variables[name]; // oh so there is a variable in the maps! lets get the content of the variable! so than we can use it to bake a cupcake.
		}
	}
};



class parser
{

public:
	parser();
	~parser();


	void parse(vector<string> tokens) {
		// CODE BEFORE STARTING THE PARSER...
		var_manager varManager;

		srand(time(NULL)); // RANDOM FROM CURRENT TIME..

		SetConsoleTitle(varManager.title_name.c_str());

		// END CODE

		string LineNameToGoto = "Main"; // sets the defualt goto as the Main 
		string LastGoto = "Main";
		vector<string> goto_arguments;

		bool lookingForIf = false; // are we looking for a if statement?
		bool ifStatementBool = false; // the actuall value of the statment
		bool ifIsEndOfIf = false;
		bool findingEndIf = false;
		bool findingElse = false;
	top: // used for GOTO and functions...
		
		bool fromThere = false;

		for (int i = 0; i < (int)(tokens.size()); i++) {
			if (!fromThere) {
				if (tokens[i] == "LABEL:"+LineNameToGoto) {
					fromThere = true;
				} else if (lookingForIf && (tokens[i] + " " + tokens[i + 1] + " EQUALS " + tokens[i + 3] == LineNameToGoto)) {

					if (ifIsEndOfIf) {
						findingEndIf = true;
					} 
					else {
						if (ifStatementBool) {
							fromThere = true;
						}
						else {
							findingElse = true;
						}
					}
				}
				else if (findingEndIf && (tokens[i] == "ENDIF")) {
					lookingForIf = false;
					ifStatementBool = false;
					ifIsEndOfIf = false;
					findingEndIf = false;
					fromThere = true;
				}
				else if (findingElse && (tokens[i] == "ELSE")) {
					fromThere = true;
					findingElse = false;
				}
				continue;
			}
			
			if (tokens[i] == "NULL") helperclasses::error("An unidentifyed error has occoured..");
				
			if (tokens[i] == "IF") {
				if (tokens[i] + " " + PF::splitColinGet(tokens[i + 1], 0) + " EQUALS " + PF::splitColinGet(tokens[i + 3], 0) + " " + tokens[i + 4] + " " + PF::splitColinGet(tokens[i + 5], 0) + " " + PF::splitColinGet(tokens[i + 6], 0) == "IF VAR EQUALS VAR GOTO GODIR GOFUNC" ||
					tokens[i] + " " + PF::splitColinGet(tokens[i + 1], 0) + " EQUALS " + PF::splitColinGet(tokens[i + 3], 0) + " " + tokens[i + 4] + " " + PF::splitColinGet(tokens[i + 5], 0) + " " + PF::splitColinGet(tokens[i + 6], 0) == "IF VAR EQUALS STRING GOTO GODIR GOFUNC" ||
					tokens[i] + " " + PF::splitColinGet(tokens[i + 1], 0) + " EQUALS " + PF::splitColinGet(tokens[i + 3], 0) + " " + tokens[i + 4] + " " + PF::splitColinGet(tokens[i + 5], 0) + " " + PF::splitColinGet(tokens[i + 6], 0) == "IF STRING EQUALS VAR GOTO GODIR GOFUNC" ||
					tokens[i] + " " + PF::splitColinGet(tokens[i + 1], 0) + " EQUALS " + PF::splitColinGet(tokens[i + 3], 0) + " " + tokens[i + 4] + " " + PF::splitColinGet(tokens[i + 5], 0) + " " + PF::splitColinGet(tokens[i + 6], 0) == "IF STRING EQUALS STRING GOTO GODIR GOFUNC" ||
					tokens[i] + " " + PF::splitColinGet(tokens[i + 1], 0) + " EQUALS " + PF::splitColinGet(tokens[i + 3], 0) + " " + tokens[i + 4] + " " + PF::splitColinGet(tokens[i + 5], 0) + " " + PF::splitColinGet(tokens[i + 6], 0) == "IF INT EQUALS VAR GOTO GODIR GOFUNC" ||
					tokens[i] + " " + PF::splitColinGet(tokens[i + 1], 0) + " EQUALS " + PF::splitColinGet(tokens[i + 3], 0) + " " + tokens[i + 4] + " " + PF::splitColinGet(tokens[i + 5], 0) + " " + PF::splitColinGet(tokens[i + 6], 0) == "IF VAR EQUALS INT GOTO GODIR GOFUNC" ||
					tokens[i] + " " + PF::splitColinGet(tokens[i + 1], 0) + " EQUALS " + PF::splitColinGet(tokens[i + 3], 0) + " " + tokens[i + 4] + " " + PF::splitColinGet(tokens[i + 5], 0) + " " + PF::splitColinGet(tokens[i + 6], 0) == "IF INT EQUALS INT GOTO GODIR GOFUNC") {
					// finds the If statement here... If it's else still finds the if statement than finds the next else, 
					// when it's done ether way it goes to the endif.

					string one = "";
					if (PF::splitColinGet(tokens[i + 1], 0) == "VAR") {
						if (varManager.getVar(PF::splitColinGet(tokens[i + 1], 1))[0] == '"') {
							one = PF::removeQuotes(varManager.getVar(PF::splitColinGet(tokens[i + 1], 1)));
						}
						else {
							one = varManager.getVar(PF::splitColinGet(tokens[i + 1], 1));
						}
					}
					else if (PF::splitColinGet(tokens[i + 1], 0) == "STRING") {
						one = PF::removeQuotes(PF::splitColinGet(tokens[i + 1], 1));
					}
					else if (PF::splitColinGet(tokens[i + 1], 0) == "INT") {
						one = PF::splitColinGet(tokens[i + 1], 1);
					}
					else {
						helperclasses::error("The value type \"" + PF::splitColinGet(tokens[i + 1], 0) + "\" is not suitable for an if statement.");
					}


					string two = "";
					if (PF::splitColinGet(tokens[i + 3], 0) == "VAR") {
						if (varManager.getVar(PF::splitColinGet(tokens[i + 3], 1))[0] == '"') {
							two = PF::removeQuotes(varManager.getVar(PF::splitColinGet(tokens[i + 3], 1)));
						}
						else {
							two = varManager.getVar(PF::splitColinGet(tokens[i + 3], 1));
						}
					}
					else if (PF::splitColinGet(tokens[i + 3], 0) == "STRING") {
						two = PF::removeQuotes(PF::splitColinGet(tokens[i + 3], 1));
					}
					else if (PF::splitColinGet(tokens[i + 3], 0) == "INT") {
						two = PF::splitColinGet(tokens[i + 3], 1);
					}
					else {
						helperclasses::error("The value type \"" + PF::splitColinGet(tokens[i + 3], 0) + "\" is not suitable for an if statement.");
					}

					if (one == two) {
						LastGoto = LineNameToGoto;
						LineNameToGoto = PF::splitColinGet(tokens[i + 5], 1);
						//goto_arguments = ;
						vector<string> tempArgs = helperclasses::split(PF::splitColinGet(tokens[i + 6], 1), ',');

						int counter = 0;
						for each (string temp in tempArgs) {
							if (temp[0] == '$') {
								varManager.addVar("$args[" + to_string(counter) + "]", varManager.getVar(temp));
								counter++;
							}
							else {
								varManager.addVar("$args[" + to_string(counter) + "]", temp);
								counter++;
							}
						}

						fromThere = false;
						goto top;
					}
					else {
					}
					i += 5;
				}

				else if (tokens[i] + " " + PF::splitColinGet(tokens[i + 1], 0) + " EQUALS " + PF::splitColinGet(tokens[i + 3], 0) + " " + tokens[i + 4] + " " + PF::splitColinGet(tokens[i + 5], 0) == "IF VAR EQUALS VAR GOTO GODIR" ||
					tokens[i] + " " + PF::splitColinGet(tokens[i + 1], 0) + " EQUALS " + PF::splitColinGet(tokens[i + 3], 0) + " " + tokens[i + 4] + " " + PF::splitColinGet(tokens[i + 5], 0) == "IF VAR EQUALS STRING GOTO GODIR" ||
					tokens[i] + " " + PF::splitColinGet(tokens[i + 1], 0) + " EQUALS " + PF::splitColinGet(tokens[i + 3], 0) + " " + tokens[i + 4] + " " + PF::splitColinGet(tokens[i + 5], 0) == "IF STRING EQUALS VAR GOTO GODIR" ||
					tokens[i] + " " + PF::splitColinGet(tokens[i + 1], 0) + " EQUALS " + PF::splitColinGet(tokens[i + 3], 0) + " " + tokens[i + 4] + " " + PF::splitColinGet(tokens[i + 5], 0) == "IF STRING EQUALS STRING GOTO GODIR" ||
					tokens[i] + " " + PF::splitColinGet(tokens[i + 1], 0) + " EQUALS " + PF::splitColinGet(tokens[i + 3], 0) + " " + tokens[i + 4] + " " + PF::splitColinGet(tokens[i + 5], 0) == "IF INT EQUALS VAR GOTO GODIR" ||
					tokens[i] + " " + PF::splitColinGet(tokens[i + 1], 0) + " EQUALS " + PF::splitColinGet(tokens[i + 3], 0) + " " + tokens[i + 4] + " " + PF::splitColinGet(tokens[i + 5], 0) == "IF VAR EQUALS INT GOTO GODIR" ||
					tokens[i] + " " + PF::splitColinGet(tokens[i + 1], 0) + " EQUALS " + PF::splitColinGet(tokens[i + 3], 0) + " " + tokens[i + 4] + " " + PF::splitColinGet(tokens[i + 5], 0) == "IF INT EQUALS INT GOTO GODIR") {
					// finds the If statement here... If it's else still finds the if statement than finds the next else, 
					// when it's done ether way it goes to the endif.

					string one = "";
					if (PF::splitColinGet(tokens[i + 1], 0) == "VAR") {
						if (varManager.getVar(PF::splitColinGet(tokens[i + 1], 1))[0] == '"') {
							one = PF::removeQuotes(varManager.getVar(PF::splitColinGet(tokens[i + 1], 1)));
						}
						else {
							one = varManager.getVar(PF::splitColinGet(tokens[i + 1], 1));
						}
					}
					else if (PF::splitColinGet(tokens[i + 1], 0) == "STRING") {
						one = PF::removeQuotes(PF::splitColinGet(tokens[i + 1], 1));
					}
					else if (PF::splitColinGet(tokens[i + 1], 0) == "INT") {
						one = PF::splitColinGet(tokens[i + 1], 1);
					}
					else {
						helperclasses::error("The value type \"" + PF::splitColinGet(tokens[i + 1], 0) + "\" is not suitable for an if statement.");
					}


					string two = "";
					if (PF::splitColinGet(tokens[i + 3], 0) == "VAR") {
						if (varManager.getVar(PF::splitColinGet(tokens[i + 3], 1))[0] == '"') {
							two = PF::removeQuotes(varManager.getVar(PF::splitColinGet(tokens[i + 3], 1)));
						}
						else {
							two = varManager.getVar(PF::splitColinGet(tokens[i + 3], 1));
						}
					}
					else if (PF::splitColinGet(tokens[i + 3], 0) == "STRING") {
						two = PF::removeQuotes(PF::splitColinGet(tokens[i + 3], 1));
					}
					else if (PF::splitColinGet(tokens[i + 3], 0) == "INT") {
						two = PF::splitColinGet(tokens[i + 3], 1);
					}
					else {
						helperclasses::error("The value type \"" + PF::splitColinGet(tokens[i + 3], 0) + "\" is not suitable for an if statement.");
					}

					if (one == two) {
						LastGoto = LineNameToGoto;
						LineNameToGoto = PF::splitColinGet(tokens[i + 5], 1);
						//cout << "LABEL:" + PF::splitColinGet(tokens[i + 1],1) << endl;
						fromThere = false;
						goto top;
						i++;
					}
					else {
					}
					i += 5;
				}
			}

			if (trim_functions::trim_copy(PF::splitColinGet(tokens[i], 0)) == "VAR") {

				if (tokens[i + 1] == "++") {
					int nowInt = atoi(varManager.getVar(PF::splitColinGet(tokens[i], 1)).c_str());
					nowInt++;
					varManager.addVar(PF::splitColinGet(tokens[i], 1), to_string(nowInt));
				}

				if (tokens[i + 1] + " " + PF::splitColinGet(tokens[i + 2], 0) == "EQUALS STRING" ||
					tokens[i + 1] + " " + PF::splitColinGet(tokens[i + 2], 0) == "EQUALS INT") {
					string content = PF::splitColinGet(tokens[i + 2], 1);
					varManager.addVar(PF::splitColinGet(tokens[i], 1), content);
					i += 2;
				}
				else if (tokens[i + 1] + " " + PF::splitColinGet(tokens[i + 2], 0) == "EQUALS VAR") {
					string s = PF::splitColinGet(tokens[i + 2], 1);
					for (int i = 0; i < s.length(); i++) {
						if (s[i] == ' ') s.erase(i, 1);
					}

					if (s.find("+") == string::npos) {
						string toPrint = varManager.getVar(PF::splitColinGet(tokens[i + 2], 1));
						if (toPrint.find("\"") != string::npos) {
							varManager.addVar(PF::splitColinGet(tokens[i], 1), PF::removeQuotes(varManager.getVar(PF::splitColinGet(tokens[i + 2], 1))));
						}
						else {
							varManager.addVar(PF::splitColinGet(tokens[i], 1), varManager.getVar(PF::splitColinGet(tokens[i + 2], 1)));
						}
						i++;
					}
					else {
						string togther_temp = "";
						vector<string> splitFromPlus = helperclasses::split(s, '+');
						for each (string var in splitFromPlus)
						{
							if (var.find("\"") != string::npos) {
								var = PF::removeQuotes(var);
								togther_temp += var;
							}
							else {
								string var_ = varManager.getVar(var);
								if (var_.find("\"") != string::npos)
									var_ = PF::removeQuotes(var_);
								togther_temp += var_;
								i++;
							}
						}

						if (togther_temp.find("\"") != string::npos) {
							togther_temp = PF::removeQuotes(togther_temp);
						}

						varManager.addVar(PF::splitColinGet(tokens[i], 1), togther_temp);
						i++;
					}
				}
				else if (tokens[i + 1] + " " + PF::splitColinGet(tokens[i + 2], 0) == "+EQUALS INT" ||
					tokens[i + 1] + " " + PF::splitColinGet(tokens[i + 2], 0) == "-EQUALS INT" ||
					tokens[i + 1] + " " + PF::splitColinGet(tokens[i + 2], 0) == "*EQUALS INT" ||
					tokens[i + 1] + " " + PF::splitColinGet(tokens[i + 2], 0) == "/EQUALS INT" ||
					tokens[i + 1] + " " + PF::splitColinGet(tokens[i + 2], 0) == "%EQUALS INT") {

					if (tokens[i + 1] == "+EQUALS") {
						//varManager.addVar(PF::splitColinGet(token, 1), PF::splitColinGet(tokens[i + 2], 1));
						int nowInt = atoi(varManager.getVar(PF::splitColinGet(tokens[i], 1)).c_str());
						nowInt += atoi(PF::splitColinGet(tokens[i + 2], 1).c_str());
						varManager.addVar(PF::splitColinGet(tokens[i], 1), to_string(nowInt));
					}
					else if (tokens[i + 1] == "-EQUALS") {
						//varManager.addVar(PF::splitColinGet(token, 1), PF::splitColinGet(tokens[i + 2], 1));
						int nowInt = atoi(varManager.getVar(PF::splitColinGet(tokens[i], 1)).c_str());
						nowInt -= atoi(PF::splitColinGet(tokens[i + 2], 1).c_str());
						varManager.addVar(PF::splitColinGet(tokens[i], 1), to_string(nowInt));
					}
					else if (tokens[i + 1] == "*EQUALS") {
						//varManager.addVar(PF::splitColinGet(token, 1), PF::splitColinGet(tokens[i + 2], 1));
						int nowInt = atoi(varManager.getVar(PF::splitColinGet(tokens[i], 1)).c_str());
						nowInt *= atoi(PF::splitColinGet(tokens[i + 2], 1).c_str());
						varManager.addVar(PF::splitColinGet(tokens[i], 1), to_string(nowInt));
					}
					else if (tokens[i + 1] == "/EQUALS") {
						//varManager.addVar(PF::splitColinGet(token, 1), PF::splitColinGet(tokens[i + 2], 1));
						int nowInt = atoi(varManager.getVar(PF::splitColinGet(tokens[i], 1)).c_str());
						nowInt /= atoi(PF::splitColinGet(tokens[i + 2], 1).c_str());
						varManager.addVar(PF::splitColinGet(tokens[i], 1), to_string(nowInt));
					}
					else if (tokens[i + 1] == "%EQUALS") {
						//varManager.addVar(PF::splitColinGet(token, 1), PF::splitColinGet(tokens[i + 2], 1));
						int nowInt = atoi(varManager.getVar(PF::splitColinGet(tokens[i], 1)).c_str());
						nowInt %= atoi(PF::splitColinGet(tokens[i + 2], 1).c_str());
						varManager.addVar(PF::splitColinGet(tokens[i], 1), to_string(nowInt));
					}
					else {
						helperclasses::error(tokens[i + 1] + " is not a valid equals operator.");
					}
				}

				else if (tokens[i + 1] + " " + PF::splitColinGet(tokens[i + 2], 0) == "+EQUALS VAR" ||
					tokens[i + 1] + " " + PF::splitColinGet(tokens[i + 2], 0) == "-EQUALS VAR" ||
					tokens[i + 1] + " " + PF::splitColinGet(tokens[i + 2], 0) == "*EQUALS VAR" ||
					tokens[i + 1] + " " + PF::splitColinGet(tokens[i + 2], 0) == "/EQUALS VAR" ||
					tokens[i + 1] + " " + PF::splitColinGet(tokens[i + 2], 0) == "%EQUALS VAR") {

					int nowInt = atoi(varManager.getVar(PF::splitColinGet(tokens[i], 1)).c_str());
					if (tokens[i + 1] == "+EQUALS")
						nowInt += atoi(varManager.getVar(PF::splitColinGet(tokens[i + 2], 1)).c_str());
					else if (tokens[i + 1] == "-EQUALS")
						nowInt -= atoi(varManager.getVar(PF::splitColinGet(tokens[i + 2], 1)).c_str());
					else if (tokens[i + 1] == "*EQUALS")
						nowInt *= atoi(varManager.getVar(PF::splitColinGet(tokens[i + 2], 1)).c_str());
					else if (tokens[i + 1] == "/EQUALS")
						nowInt /= atoi(varManager.getVar(PF::splitColinGet(tokens[i + 2], 1)).c_str());
					else if (tokens[i + 1] == "%EQUALS")
						nowInt %= atoi(varManager.getVar(PF::splitColinGet(tokens[i + 2], 1)).c_str());

					varManager.addVar(PF::splitColinGet(tokens[i], 1), to_string(nowInt));
				}

				else if (tokens[i + 1] + " " + tokens[i + 2] + " " + PF::splitColinGet(tokens[i + 3], 0) == "EQUALS MSGBOX STRING") {
					int selected = MessageBoxA(nullptr, PF::removeQuotes(PF::splitColinGet(tokens[i + 3], 1)).c_str(), "~ Mouse Script Host ~", MB_YESNO);
					string content = "";
					if (selected == 6) { // yes
						content = "yes";
					}
					else if (selected == 7) {
						content = "no";
					}
					else {
						helperclasses::error("The msgbox input \"" + content + "\" is unidentifyable.");
					}
					varManager.addVar(PF::splitColinGet(tokens[i], 1), content);
					i += 2;
				}
				else if (tokens[i + 1] + " " + PF::splitColinGet(tokens[i + 2], 0) == "EQUALS ANY") {
					if (PF::splitColinGet(tokens[i + 2], 1) == "KEYDOWN") {
						string pressed = keys::logger(1);
						varManager.addVar(PF::splitColinGet(tokens[i], 1), pressed);
						i += 2;
					}
					else if (PF::splitColinGet(tokens[i + 2], 1) == "XMOUSEPOS") {
						POINT mouse;
						GetCursorPos(&mouse);
						varManager.addVar(PF::splitColinGet(tokens[i], 1), to_string(mouse.x));
						i += 2;
					}
					else if (PF::splitColinGet(tokens[i + 2], 1) == "YMOUSEPOS") {
						POINT mouse;
						GetCursorPos(&mouse);
						varManager.addVar(PF::splitColinGet(tokens[i], 1), to_string(mouse.y));
						i += 2;
					}
				}

			}


				if (tokens[i] == "RETURN") {
					LineNameToGoto = LastGoto;
					fromThere = false;
					goto top;
					i++;
				}
				else if (tokens[i] + " " + PF::splitColinGet(tokens[i + 1], 0) + " " + PF::splitColinGet(tokens[i + 2], 0) == "GOTO GODIR GOFUNC") {
					LastGoto = LineNameToGoto;
					LineNameToGoto = PF::splitColinGet(tokens[i + 1], 1);
					//goto_arguments = ;
					vector<string> tempArgs = helperclasses::split(PF::splitColinGet(tokens[i + 2], 1), ',');

					int counter = 0;
					for each (string temp in tempArgs) {
						if (temp[0] == '$') {
							varManager.addVar("$args[" + to_string(counter) + "]", varManager.getVar(temp));
							counter++;
						}
						else {
							varManager.addVar("$args[" + to_string(counter) + "]", temp);
							counter++;
						}
					}

					fromThere = false;
					goto top;
					i++;
				}
				else if (tokens[i] + " " + PF::splitColinGet(tokens[i + 1], 0) == "GOTO GODIR") {
					LastGoto = LineNameToGoto;
					LineNameToGoto = PF::splitColinGet(tokens[i + 1], 1);
					fromThere = false;
					goto top;
					i++;
				}

				else if (tokens[i] + " " + PF::splitColinGet(tokens[i + 1], 0) == "SLEEP INT") {
					Sleep(atoi(PF::splitColinGet(tokens[i + 1], 1).c_str()));
					i++;
				}
				else if (tokens[i] + " " + PF::splitColinGet(tokens[i + 1], 0) == "MSGBOX STRING") {
					MessageBoxA(nullptr, PF::removeQuotes(PF::splitColinGet(tokens[i + 1], 1)).c_str(), "~ Mouse Script Host ~", MB_OK);
					i++;
				}
				else if (PF::splitColinGet(tokens[i], 0) + " " + tokens[i + 1] + " " + tokens[i + 2] + tokens[i] + " " + PF::splitColinGet(tokens[i + 1], 0) == "FILEREAD STRING") {
					std::ifstream t(PF::splitColinGet(tokens[i + 1], 1)); // starts if sttream with file name
					string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>()); // read file and allocats memory to str
					varManager.addVar(PF::splitColinGet(tokens[i], 1), str);
					i++;
				}
				else if (tokens[i] == "DEFINE") {
					for each (string ks in tokens)
					{
						ks = helperclasses::ReplaceAll(ks, PF::splitColinGet(tokens[i + 1], 1), tokens[i + 2]);
					}
					i += 2;
				}
				else if (tokens[i] == "EXIT") {
					exit(0);
				}

				
				if (varManager.usingConsole) {
					if (tokens[i] + " " + PF::splitColinGet(tokens[i + 1], 0) == "PRINT VAR") {
						string s = PF::splitColinGet(tokens[i + 1], 1);
						bool canErese = true;
						for (int i = 0; i < s.length(); i++) {
							if (s[i] == '\"') {
								if (canErese)
									canErese = false;
								else
									canErese = true;
							}

							if (canErese) {
								if (s[i] == ' ') s.erase(i, 1);
							}
							else {
								cout << s[i] << endl;
								// do nothing
							}
						}

						if (s.find("+") == string::npos) {
							string toPrint = varManager.getVar(PF::splitColinGet(tokens[i + 1], 1));
							if (toPrint.find("\"") != string::npos) {
								cout << PF::removeQuotes(varManager.getVar(PF::splitColinGet(tokens[i + 1], 1)));
							}
							else {
								cout << varManager.getVar(PF::splitColinGet(tokens[i + 1], 1));
							}
							i++;
						}
						else {
							string togther_temp = "";
							vector<string> splitFromPlus = helperclasses::split(s, '+');
							for each (string var in splitFromPlus)
							{
								if (var.find("\"") != string::npos) {
									var = PF::removeQuotes(var);
									togther_temp += var;
								}
								else {
									string var_ = varManager.getVar(var);
									if (var_.find("\"") != string::npos)
										var_ = PF::removeQuotes(var_);
									togther_temp += var_;
									i++;
								}
							}

							if (togther_temp.find("\"") != string::npos) {
								togther_temp = PF::removeQuotes(togther_temp);
							}

							cout << togther_temp;
							i++;
						}
					}
					else if (tokens[i] + " " + PF::splitColinGet(tokens[i + 1], 0) == "ECHO VAR") {
						string s = PF::splitColinGet(tokens[i + 1], 1);
						bool canErese = true;
						for (int i = 0; i < s.length(); i++) {
							if (s[i] == '\"') {
								if (canErese)
									canErese = false;
								else
									canErese = true;
							}

							if (canErese) {
								if (s[i] == ' ') s.erase(i, 1);
							}
							else {
								cout << s[i] << endl;
								// do nothing
							}
						}

						if (s.find("+") == string::npos) {
							string toPrint = varManager.getVar(PF::splitColinGet(tokens[i + 1], 1));
							if (toPrint.find("\"") != string::npos) {
								cout << PF::removeQuotes(varManager.getVar(PF::splitColinGet(tokens[i + 1], 1))) << endl;
							}
							else {
								cout << varManager.getVar(PF::splitColinGet(tokens[i + 1], 1)) << endl;
							}
							i++;
						}
						else {
							string togther_temp = "";
							vector<string> splitFromPlus = helperclasses::split(s, '+');
							for each (string var in splitFromPlus)
							{
								if (var.find("\"") != string::npos) {
									var = PF::removeQuotes(var);
									togther_temp += var;
								}
								else {
									string var_ = varManager.getVar(var);
									if (var_.find("\"") != string::npos)
										var_ = PF::removeQuotes(var_);
									togther_temp += var_;
									i++;
								}
							}

							if (togther_temp.find("\"") != string::npos) {
								togther_temp = PF::removeQuotes(togther_temp);
							}

							cout << togther_temp << endl;
							i++;
						}
					}
					else if (tokens[i] + " " + PF::splitColinGet(tokens[i + 1], 0) == "CMD STRING") {
						system(PF::removeQuotes(PF::splitColinGet(tokens[i + 1], 1)).c_str());
						i++;
					}
					else if (tokens[i] + " " + PF::splitColinGet(tokens[i + 1], 0) == "INPUT VAR") {
						string userInput = "";
						std::getline(std::cin, userInput);
						varManager.addVar(PF::splitColinGet(tokens[i + 1], 1), userInput);
					}
					else if (tokens[i] + " " + PF::splitColinGet(tokens[i + 1], 0) == "PRINT STRING") {
						cout << PF::removeQuotes(PF::splitColinGet(tokens[i + 1], 1));
						i++;
					}
					else if (tokens[i] + " " + PF::splitColinGet(tokens[i + 1], 0) == "ECHO STRING") {
						cout << PF::removeQuotes(PF::splitColinGet(tokens[i + 1], 1)) << endl;;
						i++;
					}
					else if (tokens[i] + " " + PF::splitColinGet(tokens[i + 1], 0) == "PRINT INT") {
						cout << PF::splitColinGet(tokens[i + 1], 1);
						i++;
					}
					else if (PF::splitColinGet(tokens[i], 0) + " " + tokens[i + 1] + " " + tokens[i + 2] == "VAR EQUALS INPUT") {
						string userInput = "";
						std::getline(std::cin, userInput);
						varManager.addVar(PF::splitColinGet(tokens[i], 1), userInput);
						i += 2;
					}
					else if (tokens[i] + " " + tokens[i + 1] == "CONSOLE REFRESH") {
						cout << " HERE " << endl;
						CONSOLE_FONT_INFOEX cfi;
						cfi.cbSize = sizeof(cfi);
						cfi.nFont = 0;
						cfi.dwFontSize.X = 0; // Width of each character in the font
						cfi.dwFontSize.Y = atoi(trim_functions::trim_copy(varManager.font_size).c_str());// Height
						cfi.FontFamily = FF_DONTCARE;
						cfi.FontWeight = FW_NORMAL;
						string font = trim_functions::trim_copy(varManager.font_size);
						wstring wideFontName;
						for (int i = 0; i < font.length(); ++i)
							wideFontName += wchar_t(font[i]);

						const wchar_t* fontWChar = wideFontName.c_str();

						std::wcscpy(cfi.FaceName, fontWChar); // Choose your font
						SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), TRUE, &cfi);
					}
					else if (tokens[i] + " " + tokens[i + 1] == "CONSOLE SHOW") {
						ShowWindow(GetConsoleWindow(), SW_SHOW);
					}
					else if (tokens[i] + " " + tokens[i + 1] == "CONSOLE HIDE") {
						ShowWindow(GetConsoleWindow(), SW_HIDE);
					}
					else if (tokens[i] == "CLS") {
						system("cls");
					}
					else if (tokens[i] == "PAUSE") {
						system("pause");
					}
				}

				if (varManager.usingRandom) {
					if (trim_functions::trim_copy(PF::splitColinGet(tokens[i], 0) + " " + tokens[i + 1] + " " + tokens[i + 2]) + " " + PF::splitColinGet(tokens[i + 3], 0) + " " + PF::splitColinGet(tokens[i + 4], 0) == "VAR EQUALS RANDOM INT INT" ||
					trim_functions::trim_copy(PF::splitColinGet(tokens[i], 0) + " " + tokens[i + 1] + " " + tokens[i + 2]) + " " + PF::splitColinGet(tokens[i + 3], 0) + " " + PF::splitColinGet(tokens[i + 4], 0) == "VAR EQUALS RANDOM VAR VAR" ||
					trim_functions::trim_copy(PF::splitColinGet(tokens[i], 0) + " " + tokens[i + 1] + " " + tokens[i + 2]) + " " + PF::splitColinGet(tokens[i + 3], 0) + " " + PF::splitColinGet(tokens[i + 4], 0) == "VAR EQUALS RANDOM VAR INT" ||
					trim_functions::trim_copy(PF::splitColinGet(tokens[i], 0) + " " + tokens[i + 1] + " " + tokens[i + 2]) + " " + PF::splitColinGet(tokens[i + 3], 0) + " " + PF::splitColinGet(tokens[i + 4], 0) == "VAR EQUALS RANDOM INT VAR") {

					string first = "";
					if (PF::splitColinGet(tokens[i + 4], 1).find("$") != string::npos)
						first = varManager.getVar(PF::splitColinGet(tokens[i + 4], 1));
					else
						first = PF::splitColinGet(tokens[i + 4], 1);

					string last = "";
					if (PF::splitColinGet(tokens[i + 3], 1).find("$") != string::npos)
						last = varManager.getVar(PF::splitColinGet(tokens[i + 3], 1));
					else
						last = PF::splitColinGet(tokens[i + 3], 1);

					int random = rand() % atoi(first.c_str()) + atoi(last.c_str());
					varManager.addVar(PF::splitColinGet(tokens[i], 1), to_string(random));
					i += 4;
				}
				}
		}
	}
};
