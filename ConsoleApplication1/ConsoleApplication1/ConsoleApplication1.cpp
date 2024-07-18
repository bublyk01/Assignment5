#include <iostream>
#include <string>
#include <cctype>
#include <map>
#include <functional>
#include <stdexcept>
#include <vector>
#include <cmath>

class Interpreter {
public:
    Interpreter() {
        functions["add"] = [](const std::vector<double>& args) { return args[0] + args[1]; };
        functions["sub"] = [](const std::vector<double>& args) { return args[0] - args[1]; };
        functions["mul"] = [](const std::vector<double>& args) { return args[0] * args[1]; };
        functions["div"] = [](const std::vector<double>& args) { return args[0] / args[1]; };
        functions["pow"] = [](const std::vector<double>& args) { return std::pow(args[0], args[1]); };
        functions["abs"] = [](const std::vector<double>& args) { return std::abs(args[0]); };
        functions["max"] = [](const std::vector<double>& args) { return std::max(args[0], args[1]); };
        functions["min"] = [](const std::vector<double>& args) { return std::min(args[0], args[1]); };
    }

    double evaluate(const std::string& expression) {
        position = 0;
        input = expression;
        return parseStatement();
    }

    void defineFunction(const std::string& name, const std::string& param1, const std::string& param2, const std::string& expr) {
        customFunctions[name] = [this, param1, param2, expr](const std::vector<double>& args) {
            localVariables[param1] = args[0];
            localVariables[param2] = args[1];
            double result = evaluateWithLocalVariables(expr);
            localVariables.erase(param1);
            localVariables.erase(param2);
            return result;
            };
    }

private:
    size_t position;
    std::string input;
    std::map<std::string, std::function<double(const std::vector<double>&)>> functions;
    std::map<std::string, std::function<double(const std::vector<double>&)>> customFunctions;
    std::map<std::string, double> localVariables;
    std::map<std::string, double> globalVariables;

    double parseStatement() {
        skipWhitespace();
        if (input.substr(position, 4) == "var ") {
            position += 4;
            return parseAssignment();
        }
        return parseExpression();
    }

    double parseAssignment() {
        std::string varName;
        while (position < input.size() && std::isalpha(input[position])) {
            varName += input[position++];
        }
        skipWhitespace();
        if (input[position] == '=') {
            ++position;
            double value = parseExpression();
            globalVariables[varName] = value;
            return value;
        }
        else {
            throw std::runtime_error("Expected '=' in assignment");
        }
    }

    double parseExpression() {
        double result = parseTerm();
        while (position < input.size() && (input[position] == '+' || input[position] == '-')) {
            char op = input[position++];
            double term = parseTerm();
            if (op == '+') result += term;
            else result -= term;
        }
        return result;
    }

    double parseTerm() {
        double result = parseFactor();
        while (position < input.size() && (input[position] == '*' || input[position] == '/')) {
            char op = input[position++];
            double factor = parseFactor();
            if (op == '*') result *= factor;
            else result /= factor;
        }
        return result;
    }

    double parseFactor() {
        if (position < input.size() && input[position] == '-') {
            ++position;
            return -parseFactor();
        }
        if (position < input.size() && std::isdigit(input[position])) {
            return parseNumber();
        }
        else if (position < input.size() && std::isalpha(input[position])) {
            std::string name;
            while (position < input.size() && std::isalpha(input[position])) {
                name += input[position++];
            }
            if (localVariables.find(name) != localVariables.end()) {
                return localVariables[name];
            }
            else if (globalVariables.find(name) != globalVariables.end()) {
                return globalVariables[name];
            }
            else {
                return parseFunctionCall(name);
            }
        }
        else if (position < input.size() && input[position] == '(') {
            ++position;
            double result = parseExpression();
            if (position < input.size() && input[position] == ')') {
                ++position;
            }
            else {
                throw std::runtime_error("Wrong order of parentheses");
            }
            return result;
        }
        else {
            throw std::runtime_error("Unexpected character");
        }
    }

    double parseNumber() {
        std::string number;
        while (position < input.size() && (std::isdigit(input[position]) || input[position] == '.')) {
            number += input[position++];
        }
        return std::stod(number);
    }

    double parseFunctionCall(const std::string& functionName) {
        if (position < input.size() && input[position] == '(') {
            ++position;
            std::vector<double> args;
            while (position < input.size() && input[position] != ')') {
                args.push_back(parseExpression());
                if (position < input.size() && input[position] == ',') ++position;
            }
            if (position < input.size() && input[position] == ')') {
                ++position;
            }
            else {
                throw std::runtime_error("Wrong order of parentheses");
            }
            if (functions.find(functionName) != functions.end()) {
                return functions[functionName](args);
            }
            else if (customFunctions.find(functionName) != customFunctions.end()) {
                if (args.size() != 2) {
                    throw std::runtime_error("These functions need 2 arguments");
                }
                return customFunctions[functionName](args);
            }
            else {
                throw std::runtime_error("Unknown function: " + functionName);
            }
        }
        else {
            throw std::runtime_error("Wrong function call");
        }
    }

    void skipWhitespace() {
        while (position < input.size() && std::isspace(input[position])) {
            ++position;
        }
    }

    double evaluateWithLocalVariables(const std::string& expression) {
        size_t savedPosition = position;
        std::string savedInput = input;
        auto savedLocalVariables = localVariables;

        input = expression;
        position = 0;

        double result = parseExpression();

        input = savedInput;
        position = savedPosition;
        localVariables = savedLocalVariables;

        return result;
    }
};

int main() {
    Interpreter interpreter;
    interpreter.defineFunction("Add", "x", "y", "x+y");
    interpreter.defineFunction("Subtract", "x", "y", "x-y");
    interpreter.defineFunction("Multiply", "x", "y", "x*y");
    interpreter.defineFunction("Divide", "x", "y", "x/y");

    std::string expression;

    while (true) {
        std::cout << "Enter expression: ";
        std::getline(std::cin, expression);
        if (expression.empty()) break;
        try {
            double result = interpreter.evaluate(expression);
            std::cout << "Result: " << result << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }

    return 0;
}
