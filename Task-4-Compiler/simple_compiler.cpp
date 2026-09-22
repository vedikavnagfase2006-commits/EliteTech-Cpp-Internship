#include <iostream>
#include <string>
#include <cctype>
#include <stdexcept>

using namespace std;

// ---------------------------------------------------------
// Simple Arithmetic Expression Parser
//
// Grammar:
//
// Expression -> Term { (+|-) Term }
// Term       -> Factor { (*|/) Factor }
// Factor     -> Number | '(' Expression ')' | '-' Factor
//
// This grammar gives * and / higher precedence than + and -.
// ---------------------------------------------------------

class Parser
{
private:
    string expression;
    size_t position;

    // Skip spaces in the expression
    void skipSpaces()
    {
        while (position < expression.length() &&
               isspace(static_cast<unsigned char>(expression[position])))
        {
            position++;
        }
    }

    // Parse a number
    double parseNumber()
    {
        skipSpaces();

        size_t start = position;

        bool hasDecimal = false;

        while (position < expression.length())
        {
            char ch = expression[position];

            if (isdigit(static_cast<unsigned char>(ch)))
            {
                position++;
            }
            else if (ch == '.' && !hasDecimal)
            {
                hasDecimal = true;
                position++;
            }
            else
            {
                break;
            }
        }

        if (start == position)
        {
            throw runtime_error("Expected a number.");
        }

        return stod(expression.substr(start, position - start));
    }

    // Factor handles numbers, parentheses and unary minus
    double parseFactor()
    {
        skipSpaces();

        if (position >= expression.length())
        {
            throw runtime_error("Unexpected end of expression.");
        }

        // Handle negative numbers
        if (expression[position] == '-')
        {
            position++;
            return -parseFactor();
        }

        // Handle parentheses
        if (expression[position] == '(')
        {
            position++;

            double result = parseExpression();

            skipSpaces();

            if (position >= expression.length() ||
                expression[position] != ')')
            {
                throw runtime_error("Missing closing parenthesis.");
            }

            position++;

            return result;
        }

        return parseNumber();
    }

    // Term handles multiplication and division
    double parseTerm()
    {
        double result = parseFactor();

        while (true)
        {
            skipSpaces();

            if (position >= expression.length())
            {
                break;
            }

            char operation = expression[position];

            if (operation != '*' && operation != '/')
            {
                break;
            }

            position++;

            double nextValue = parseFactor();

            if (operation == '*')
            {
                result *= nextValue;
            }
            else
            {
                if (nextValue == 0)
                {
                    throw runtime_error("Division by zero.");
                }

                result /= nextValue;
            }
        }

        return result;
    }

    // Expression handles addition and subtraction
    double parseExpression()
    {
        double result = parseTerm();

        while (true)
        {
            skipSpaces();

            if (position >= expression.length())
            {
                break;
            }

            char operation = expression[position];

            if (operation != '+' && operation != '-')
            {
                break;
            }

            position++;

            double nextValue = parseTerm();

            if (operation == '+')
            {
                result += nextValue;
            }
            else
            {
                result -= nextValue;
            }
        }

        return result;
    }

public:

    Parser(const string& input)
    {
        expression = input;
        position = 0;
    }

    double parse()
    {
        double result = parseExpression();

        skipSpaces();

        // Make sure the entire expression was processed
        if (position != expression.length())
        {
            throw runtime_error(
                "Invalid expression near: " +
                expression.substr(position)
            );
        }

        return result;
    }
};

// ---------------------------------------------------------
// Main Program
// ---------------------------------------------------------

int main()
{
    cout << "========================================\n";
    cout << "       SIMPLE C++ EXPRESSION PARSER\n";
    cout << "========================================\n";

    cout << "\nSupported operators: +  -  *  /\n";
    cout << "Parentheses are supported.\n";
    cout << "Type 'exit' to close the program.\n";

    while (true)
    {
        string input;

        cout << "\nEnter an expression: ";
        getline(cin, input);

        if (input == "exit")
        {
            cout << "Exiting compiler...\n";
            break;
        }

        if (input.empty())
        {
            cout << "Please enter an expression.\n";
            continue;
        }

        try
        {
            Parser parser(input);

            double result = parser.parse();

            cout << "Parsed successfully!\n";
            cout << "Result: " << result << "\n";
        }
        catch (const exception& error)
        {
            cout << "Parsing Error: "
                 << error.what() << "\n";
        }
    }

    return 0;
}
