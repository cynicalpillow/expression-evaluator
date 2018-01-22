#include <bits/stdc++.h>
using namespace std;

/* TO-DO:
 * equation solver (?)
 */

const int OPERATORS = 17;
int EVALUATION_ERROR = 0;

//Tokenizing
string original_expression;
vector<string> tokens;

//Operators and precedence values
string operators[OPERATORS] = {"+", "-", "*", "/", "^", "sin", "cos", "tan", "ln", "log", "log10", "log2", "exp", "exp2", "sqrt", "-u", "!"};
map<string, int> precedences;
map<string, int> associativity;

//Shunting yard
stack<string> operator_stack;
vector<string> output_queue;

//Evaluation
stack<string> operand_stack;

string remove_spaces(){
	int n = original_expression.length();
	string ret = "";
	for(int i = 0; i < n; i++){
		if(original_expression[i] >= 'A' && original_expression[i] <= 'Z')ret += tolower(original_expression[i]);
		else if(original_expression[i] != ' ')ret += original_expression[i];
	}
	return ret;
}

//Split string into tokens
void tokenize(){
	original_expression = remove_spaces();
	int n = original_expression.length();
	int right = 0;
	int left = 0;
	bool update = false;
	for(int i = 0; i < n; i++){
		if(update){
			left = i;
			right = i;
			update = false;
		}
		if(isdigit(original_expression[i]) 
			|| original_expression[i] == '.' 
			|| (original_expression[i] >= 'a' && original_expression[i] <= 'z')){
			right++;
		} else {
			if(left != right)tokens.push_back(original_expression.substr(left, right-left)); //Don't add an empty string, otherwise add the number
			tokens.push_back(original_expression.substr(i, 1)); //Add the operator
			update = true; //move up left and right pointer to next position after operator
		}
	}
	if(left != right && !update)tokens.push_back(original_expression.substr(left, right-left)); //Don't add an empty string, otherwise add the number
}

void initialize_operator_precedence(){
	for(int i = 0; i < OPERATORS; i++){
		if(i < 2)precedences[operators[i]] = 2;
		else if(i < 4)precedences[operators[i]] = 3;
		else if(i < 5)precedences[operators[i]] = 4;
		else if(i == 15)precedences[operators[i]] = 4;
		else precedences[operators[i]] = 5;
	}
}

void initialize_operator_associativity(){
	for(int i = 0; i < OPERATORS; i++){
		if(i != 4 && i != 15)associativity[operators[i]] = 1;
		else associativity[operators[i]] = 0;
	}
}

//Check if the current token is a operator
bool is_operator(string x){
	for(int i = 0; i < OPERATORS; i++)
		if(x == operators[i])return true;
	return false;
}

int shunting_yard(){
	bool beginning = true;
	bool prev_was_operator = false;
	bool unary = false;
	for(string s : tokens){
		if(s == "-" && (beginning || prev_was_operator || unary)){
			unary = true;
			operator_stack.push("-u");
		} else if(is_operator(s)){
			while(operator_stack.size() > 0 && ((precedences[operator_stack.top()] > precedences[s]) 
				|| (precedences[operator_stack.top()] == precedences[s] && associativity[operator_stack.top()])) 
				&& operator_stack.top() != "("){
				output_queue.push_back(operator_stack.top());
				operator_stack.pop();
			}
			operator_stack.push(s);
			if(s != "!")prev_was_operator = true;
		} else if(s == "("){
			if(!prev_was_operator && !unary)operator_stack.push("*");
			operator_stack.push(s);
			prev_was_operator = true;
		} else if(s == ")"){
			while(operator_stack.size() > 0 && operator_stack.top() != "("){
				output_queue.push_back(operator_stack.top());
				operator_stack.pop();
			}
			if(operator_stack.size() < 1){
				cout << "Error with brackets" << endl;
				return -1;
			}
			operator_stack.pop();
			prev_was_operator = false;
		} else {
			output_queue.push_back(s);
			prev_was_operator = false;
			unary = false;
		}
		beginning = false;
	}
	while(operator_stack.size() > 0){
		if(operator_stack.top() == "("){
			cout << "Error with brackets" << endl;
			return -1;
		}
		output_queue.push_back(operator_stack.top());
		operator_stack.pop();
	}
	return 0;
}

double get_value(double operand1, double operand2, string op){
	if(op == "+"){
		return operand1+operand2;
	} else if(op == "-"){
		return operand1-operand2;
	} else if(op == "*"){
		return operand1*operand2;
	} else if(op == "^"){
		return pow(operand1, operand2);
	} else if(op == "/"){
		return operand1/operand2;
	}
}

double factorial(double x){
	return (x <= 1)?1:(x * factorial(x-1));
}

double get_value(double operand1, string op){
	if(op == "sin"){
		return sin(operand1);
	} else if(op == "cos"){
		return cos(operand1);
	} else if(op == "tan"){
		return tan(operand1);
	} else if(op == "log" || op == "ln"){
		return log(operand1);
	} else if(op == "log10"){
		return log10(operand1);
	} else if(op == "log2"){
		return log2(operand1);
	} else if(op == "exp"){
		return exp(operand1);
	} else if(op == "exp2"){
		return exp2(operand1);
	} else if(op == "sqrt"){
		return sqrt(operand1);
	} else if(op == "-u"){
		return -1*operand1;
	} else if(op == "!"){
		return factorial(round(operand1));
	}
}

bool is_function_unary_operator(string s){
	for(int i = 0; i < OPERATORS; i++)
		if(operators[i] == s && i > 4)return true;
	return false;
}

double evaluate(){
	for(string s : output_queue)
		cout << s << endl;
	for(string s : output_queue){
		if(is_function_unary_operator(s)){
			if(operand_stack.size() < 1){
				EVALUATION_ERROR = 1;
				return 0;
			}
			double operand_1 = atof(operand_stack.top().c_str()); operand_stack.pop();
			operand_stack.push(to_string(get_value(operand_1, s)));
		}else if(is_operator(s)){
			if(operand_stack.size() < 2){
				EVALUATION_ERROR = 1;
				return 0;
			}
			double operand_2 = atof(operand_stack.top().c_str()); operand_stack.pop();
			double operand_1 = atof(operand_stack.top().c_str()); operand_stack.pop();
			operand_stack.push(to_string(get_value(operand_1, operand_2, s)));
		} else {
			operand_stack.push(s);
		}
	}
	double ret = atof(operand_stack.top().c_str()); operand_stack.pop();
	return ret;
}

int main(){
	freopen("input.txt", "r", stdin);
	//Initialization and tokenizing
	getline(cin, original_expression);
	tokenize();
	initialize_operator_precedence();
	initialize_operator_associativity();

	//Shunting yard algorithm to get expression into post-fix notation
	int error = shunting_yard();
	if(error == -1)return 0;

	//Evaluate the post-fix expression
	double ans = evaluate();
	cout << original_expression << "=" << endl;
	if(EVALUATION_ERROR)cout << "Error in evaluating" << endl;
	else cout << ans << endl;
	return 0;
}