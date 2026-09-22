# Task 4 - Simple Compiler Design Basics

## Objective

Implement a simple compiler in C++ that can parse and evaluate basic arithmetic expressions.

## Features

- Parses arithmetic expressions
- Supports addition (+)
- Supports subtraction (-)
- Supports multiplication (*)
- Supports division (/)
- Supports parentheses
- Supports decimal numbers
- Supports unary minus
- Follows operator precedence
- Detects division by zero
- Detects invalid expressions
- Displays the evaluated result

## Parsing Grammar

The program uses a simple recursive-descent parsing approach.

```text
Expression -> Term { (+|-) Term }
Term       -> Factor { (*|/) Factor }
Factor     -> Number | '(' Expression ')' | '-' Factor
Input: 2 + 3
Output: 5

Input: 2 + 3 * 4
Output: 14

Input: (2 + 3) * 4
Output: 20

Input: 10 / 2
Output: 5

Input: 10 / 0
Output: Parsing Error: Division by zero.