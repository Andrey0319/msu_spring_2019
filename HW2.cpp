#include <iostream>
#include <string>

int is_number(char a);
int is_operation(char a);
long long read_number(int &index, int &stop, const std::string &args, int max_index);
char read_operator (int &index, int &stop, const std::string &args, int max_index);

/*read_number - read number, skipping all spaces and checking correct input
  read_operator - read operation, skipping all spaces and checking correct input
*/

long long add(int &index, int arg1, char op, int neg1, int &stop, const std::string &args, int max_index);
long long mul(int &index, int arg1, char op, int &neg, int &stop, const std::string &args, int max_index);

/* arg1      - first argument in calculating consecutive multiplications and divisions
   op        - first operation in calculating consecutive multiplications and divisions
   neg       - is the previous operation subtraction (yes -1, no 1)
   stop      - flag of expression: (stop == 0  - continue calculating)
                                   (stop == 1  - end of expression)
                                   (stop == 2  - error)
   args      - expression
   max_index - index of end of expression
*/


int is_number(char a)
{
    return (a >= '0') && (a <= '9');
}


int is_operation(char a)
{
    return (a == '+') || (a == '-') ||(a == '*') ||(a == '/');
}


long long read_number(int &index, int &stop, const std::string &args, int max_index)
{
    int neg = 1;
    long long num = 0;
    while (args[index] == ' ' && index < max_index)
        index++;

    if (args[index] == '-' && index < max_index) {
        neg = -1;
        index++;
    }
    while (args[index] == ' ' && index < max_index)
        index++;

    if (!is_number(args[index])) {
        stop = 2;
        return 1;
    }
    while (is_number(args[index]) && index < max_index) {
        num = (num * 10) + args[index] - '0';
        index++;
    }
    return num * neg;
}


char read_operator (int &index, int &stop, const std::string &args, int max_index)
{
    while (args[index] == ' ' && index < max_index)
        index++;

    if (index >= max_index) {
        stop = 1;
    }
    if (stop != 1 && !is_operation(args[index])) {
        stop = 2;
        return 1;
    }
    return args[index++];
}


long long add(int &index, int neg1, int &stop, const std::string &args, int max_index)
{
    long long arg2;
    char op;
    int neg = 1;

    if (stop == 1)
        return 0;

    arg2 = read_number(index, stop, args, max_index);
    if (stop == 2)
        return 1;
    if (stop == 1) {
        return neg1 * arg2;
    }
    op = read_operator(index, stop, args, max_index);
    if (stop == 2)
        return 1;
    if (stop == 1) {
        return neg1 * arg2;
    }

    if (op == '+'){
        int p = neg1 * arg2 + add(index, 1, stop, args, max_index);
        return p;
    }
    if (op == '-') {
        int p = neg1 * arg2 + add(index, -1, stop, args, max_index);
        return p;
    }

    //calculates consecutive multiplications
    if (op == '/')
    {    //calculate "new" neg to avoid conflict with "neg1"
        arg2 = neg1 * mul(index, arg2, op, neg, stop, args, max_index);
        return arg2 + add(index, neg, stop, args, max_index);
    }

    //calculates consecutive divisions
    if (op == '*')
    {
        arg2 = neg1 * mul(index, arg2, op, neg, stop, args, max_index);
        return arg2 + add(index, neg, stop, args, max_index);
    }

    return -1;
}


long long mul(int &index, int arg1, char op, int &neg, int &stop, const std::string &args, int max_index)
{
    long long arg2;
    // compute first operation
    arg2 = read_number(index, stop, args, max_index);
    if (stop == 2)
        return 1;

    if (op == '*')
        arg1 *= arg2;

    if (op == '/') {
        if (arg2 == 0) {
            stop = 2;
            return 1;
        }
        arg1 /= arg2;
    }

    while (1)
    {
        op = read_operator(index, stop, args, max_index);
        if (stop == 2)
            break;

        //end of mul and return
        if (op == '-') {     //if next operation is subtraction
            neg = -1;
            break;
        }
        if (op == '+')       //if next operation is addition
            break;

        if (stop == 1)       //if it is the end of expression
            break;

        //  calculates consecutive multiplications and divisions
        arg2 = read_number(index, stop, args, max_index);
        if (stop == 2)
            break;

        if (op == '*')
            arg1 *= arg2;

        if (op == '/') {
            if (arg2 == 0) {
                stop = 2;
                return 1;
            }
            arg1 /= arg2;
        }
    }
    return arg1;
}


int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cout << "error";
        return 1;
    }

    const std::string args = argv[1];                    //stop == 0  - continue calculating
    int i = 0, stop = 0, index = args.length();    //stop == 1  - end of expression
    long long res;                                 //stop == 2  - error
    res = add(i, 1, stop, args, index);   //expression value
    if (stop == 2) {                      //checking input
        std::cout << "error";
        return 1;
    }
    std::cout << res;

    return 0;
}
