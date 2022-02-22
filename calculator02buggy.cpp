
//
// This is example code from Chapter 6.7 "Trying the second version" of
// "Software - Principles and Practice using C++" by Bjarne Stroustrup
//

/*
    This file is known as calculator02buggy.cpp

    I have inserted 5 errors that should cause this not to compile
    I have inserted 3 logic errors that should cause the program to give wrong results

    First try to find an remove the bugs without looking in the book.
    If that gets tedious, compare the code to that in the book (or posted source code)

    Happy hunting!

*/

#include "C:\Users\Coserea\Desktop\test c++\lib_facilities\std_lib_facilities.h"

//------------------------------------------------------------------------------

class Token{
public:
    char kind;        // what kind of token
    double value;     // for numbers: a value
    string name;
    Token(char ch)    // make a Token from a char
        :kind(ch), value(0) { }
    Token(char ch, double val)     // make a Token from a char and a double
        :kind(ch), value(val) { }
    Token(char ch, string n) : kind(ch), name(n) {}
};

const char number = '8'; //t.kind == number means that t is a number Token
const char quit   = 'Q';  //t.kind==quit means that t is a quit token
const char print  = ';'; //t.kind == print means that t is a print token
const string prompt = ">";
const string result = "=";  //used to indicate that what follows is a result
const char name = 'a'; //name token
const char let = 'L';  //declaration token
const string declkey = "let"; //declaration keyword
const char assignment_op = '=';
const char sqrt_op = 's';
const string sqrtkey = "sqrt";
const string constkey = "const";
const char const_op = 'c';

const char pow_op = 'p';
const string powkey = "pow";

const char help = 'H';

//------------------------------------------------------------------------------

class Token_stream {
public:
    Token_stream();   // make a Token_stream that reads from cin
    Token get();      // get a Token (get() is defined elsewhere)
    void putback(Token t);    // put a Token back
    void ignore(char c);  //discard characters up to and including a c
private:
    bool full;        // is there a Token in the buffer?
    Token buffer;     // here is where we keep a Token put back using putback()
};

class Variable {
public:
    string name;
    double value;
    char type; //let the user distinguish between constants and variables
    Variable(string n, double v, char t):name(n), value(v), type(t) { }
};

//------------------------------------------------------------------------------

// The constructor just sets full to indicate that the buffer is empty:
Token_stream::Token_stream()
    :full(false), buffer(0)    // no Token in buffer
{
}

//------------------------------------------------------------------------------

// The putback() member function puts its argument back into the Token_stream's buffer:
void Token_stream::putback(Token t)
{
    if (full) error("putback() into a full buffer");
    buffer = t;       // copy t to buffer
    full = true;      // buffer is now full
}

//------------------------------------------------------------------------------

Token Token_stream::get()
{
    if (full) {       // do we already have a Token ready?
        // remove token from buffer
        full = false;
        return buffer;
    }

    char ch;
//    cin >> ch;    // note that >> skips whitespace (space, newline, tab, etc.)
    cin.get(ch);
    while (isspace(ch)) {//discard whitespace but keep newline
        if (ch == '\n')
            break;
        else
            cin.get(ch);
    }
    switch (ch) {
    case'\n':
        return Token(print);
    case print:    // for "print"
    case quit:    // for "quit"
    case '(':
    case ')':
    case '+':
    case '-':
    case '*':
    case '/':
    case '{':
    case '}':
    case '!':
    case '%':
    case '=':
    case ',':
    case 'H':
        return Token(ch);        // let each character represent itself
    case '.':   //a floating point literal can start with a dot
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':   //numeric literal
    {
        cin.putback(ch);         // put digit back into the input stream
        double val;
        cin >> val;              // read a floating-point number
        return Token(number, val);   // let '8' represent "a number"
    }
    default:
        if (isalpha(ch) || ch == '_') {
           // cin.putback(ch);
            string s;
            s += ch;
            while (cin.get(ch) && (isalpha(ch) || isdigit(ch) || ch == '_'))
                s += ch;
            cin.putback(ch);
            if(s==declkey) return Token(let);  //declaration keyword
            if(s==sqrtkey) return Token(sqrt_op);
            if (s == constkey) return Token(const_op);
            if (s == powkey) return Token(pow_op);
            return Token(name,s);
        }
        error("Bad token");
    }
}

void Token_stream::ignore(char c) {
    //c represents the kind of Token
    //first look in buffer
    if (full && c == buffer.kind) {
        full = false;
        return;
    }

    full = false;

    //now search input
    char ch = 0;
    while (cin >> ch)
        if (ch == c) return;
}

//------------------------------------------------------------------------------

Token_stream ts;        // provides get() and putback() 

struct Symbol_table {
    vector<Variable> var_table;

    double get_value(string s);
    void set_value(string s, double d);
    bool is_declared(string s);
    double define_name(string var, double val, char type);
};

Symbol_table st;




double Symbol_table::get_value(string s) {
    //return the value of the variable named s
    for (int i = 0; i < var_table.size(); ++i) {
        if (var_table[i].name == s) return var_table[i].value;
    }
    error("get: undefined variable ", s);
}

void Symbol_table::set_value(string s, double d) {
    //set the Variable named s to d
    //== give a Variable a new value
    for (int i = 0; i < var_table.size(); ++i) {
        if (var_table[i].name == s) {
            if (var_table[i].type == 'v') {
                var_table[i].value = d;
                return;
            }
            else {
                error("Can not change the value of constant!");
            }
        }
    }
    error("Set: undefined variable ", s);
}

//------------------------------------------------------------------------------

double expression();    // declaration so that primary() can call expression()
double term();
double factorial();
double declaration();
double declare_const();
//------------------------------------------------------------------------------

// deal with numbers and parentheses
double primary()
{
    Token t = ts.get();
    switch (t.kind) {
    case '(':    // handle '(' expression ')'
    {
        double d = expression();
        t = ts.get();
        if (t.kind != ')') error("')' expected");
            return d;
    }
    case '{':
    {
        double d = expression();
        t = ts.get();
        if (t.kind != '}') error("'}' expected");
        return d;
    }
    case number:
        return t.value;  // return the number's value
    case '-':
        return -primary();
    case '+':
        return primary();
    case name:
        return st.get_value(t.name);
    default:
        error("primary expected");
    }
}

//------------------------------------------------------------------------------

// deal with *, /, and %
double term()
{
    double left = factorial();
    Token t = ts.get();        // get the next token from token stream

    while (true) {
        switch (t.kind) {
        case '*':
            left *= factorial();
            t = ts.get();
            break;
        case '/':
        {
            double d = factorial();
            if (d == 0) error("divide by zero");
            left /= d;
            t = ts.get();
            break;
        }
        case '%':
        {
            int i1 = narrow_cast<int>(left);
            int i2 = narrow_cast<int>(term());
            if (i2 == 0) error("%: divide by zero");
            left = i1 % i2;
            t = ts.get();
            break;
        }
        case ',':
        {
            int i1 = narrow_cast<int>(term());
            left = pow(left, i1);
            t = ts.get();
            break;
        }
        default:
            ts.putback(t);     // put t back into the token stream
            return left;
        }
    }
}

double factorial() {
    double left = primary();
    Token t = ts.get();
    switch (t.kind) {
    case'!':
    {
        double d = 1;
        if (left == 0) {
            left = 1;
            return left;
        }
        else {
            for (int i = 1; i <= left; i++) {
                d *= i;
            }
            left = d;
            return left;
        }
    }
    default:
        ts.putback(t);
        return left;
    }
}
//------------------------------------------------------------------------------

// deal with + and -
double expression()
{
    double left = term();      // read and evaluate a Term
    Token t = ts.get();        // get the next token from token stream

    while (true) {
        switch (t.kind) {
        case '+':
            left += term();    // evaluate Term and add
            t = ts.get();
            break;
        case '-':
            left -= term();    // evaluate Term and subtract
            t = ts.get();
            break;
        default:
            ts.putback(t);     // put t back into the token stream
            return left;       // finally: no more + or -: return the answer
        }
    }
}

//------------------------------------------------------------------------------

double power() {
    //assume we have seen pow
    Token t = ts.get();
    if (t.kind != '(') error("'(' expected after pow call");
    double d = expression();
    t = ts.get();
    if(t.kind != ')') error("'(' expected to finish pow declaration");
    return d;
}

double statement() {
    Token t = ts.get();
    switch (t.kind) {
    case let:
        return declaration();
    case name: 
    {
        string s = t.name;
        ts.putback(t);
        double d = expression();
        if (cin.peek()) {
            t = ts.get();
            if (t.kind == '=') {
                double d2 = expression();
                st.set_value(s, d2);
                return st.get_value(s);
            }
            ts.putback(t);
        }

        return d;
    }
    case sqrt_op:
    {
        Token t2 = ts.get();
        if (t2.kind != '(') error("'(' expected after sqrt call");
        double d = expression();
        t2 = ts.get();
        if (t2.kind != ')') error("')' expected after sqrt call");
        if (d < 0) error("Attempt to take square root of negative number!");
        return sqrt(d);
    }
    case const_op:
        return declare_const();
    case pow_op:
        return power();
    default:
        ts.putback(t);
        return expression();
    }
}

bool Symbol_table::is_declared(string var) {
    //is var already declared in var_table?
    for (int i = 0; i < var_table.size(); ++i)
        if (var_table[i].name == var) return true;
    return false;
}

double Symbol_table::define_name(string var, double val, char type) {
    //add(var,val) to var_table
    if (is_declared(var)) error(var, " declared twice");
    var_table.push_back(Variable(var, val,type));
    return val;
}

double declaration() {
    //assume we have seen "let"
    //handle: name = expression
    //declare a variable called "name" with the initial value "expression"

    Token t = ts.get();
    if (t.kind != name) error("name expected in delcaration");
    string var_name = t.name;

    Token t2 = ts.get();
    //era != '='
    if (t2.kind != '=') error("= missing in delcaration of ", var_name);

    double d = expression();
    st.define_name(var_name, d,'v');
    return d;
}


double declare_const() {
    //assume we have seen const
    //handle name = expression
    //declare a constant called name with the initial value expression
    Token t = ts.get();
    if (t.kind != name) error("name expected in delcaration");
    string var_name = t.name;

    Token t2 = ts.get();
    //era != '='
    if (t2.kind != '=') error("= missing in delcaration of ", var_name);

    double d = expression();
    st.define_name(var_name, d, 'c');
    return d;
}

void clean_up_mess() {  //naive :)))))))
    ts.ignore(print);
}


void help_message() {
    cout << "pow(x,y) for x to power y\n"
        << "sqrt(exp)\n"
        << "define a variable: LET var_name = value\n"
        << "use = to change var_name value after definition\n"
        << "const var_name = value defines a constant var\n";
}


void calculate(){
    while (cin) {
        try {
            cout << prompt;
            Token t = ts.get();
            while (t.kind == print) t = ts.get(); //first discard all prints
            if (t.kind == quit) return; //return
            if (t.kind == help) {
                help_message();
                continue;
            }
            ts.putback(t);
            cout << result << statement() << endl;
        }
        catch (std::exception& e) {
            cerr << e.what() << endl;   //write error message
            clean_up_mess();
        }
    }
}

int main()
try
{
    cout << "Welcome to our simple calculator!" << '\n'
        << "Please enter expressions using floating-point numbers." << '\n'
        << "The available operators are: +,-,*,/" << '\n'
        << "To print a result, type ; after your expression or hit enter" << '\n'
        << "To exit, type x" << '\n'
        << "Type 'H' to see the help menu" << '\n';

    //predefine names

    st.define_name("pi", 3.1415926535,'c');
    st.define_name("e", 2.7182818284,'c');


    calculate();
    keep_window_open();
    return 0;
}
catch (exception& e) {
    cerr << "error: " << e.what() << '\n';
    //keep_window_open("1");
    cout << "Please enter the characer ~ to close the window" << endl;
    char ch;
    while (cin >> ch)
        if (ch == '~') 
            return 1;    
    return 1;
}
catch (...) {
    cerr << "Oops: unknown exception!\n";
    keep_window_open("3");
    return 2;
}

//------------------------------------------------------------------------------