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
        return parseExpression();
    }

private:
    size_t position;
    std::string input;
    std::map<std::string, std::function<double(const std::vector<double>&)>> functions;

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
            return parseFunctionCall();
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

    double parseFunctionCall() {
        std::string functionName;
        while (position < input.size() && std::isalpha(input[position])) {
            functionName += input[position++];
        }
        if (position < input.size() && input[position] == '(') {
            ++position;
            std::vector<double> args;
            while (position < input.size() && input[position] != ')') {
                args.push_back(parseExpression());
                if (input[position] == ',') ++position;
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
            else {
                throw std::runtime_error("Unknown function: " + functionName);
            }
        }
        else {
            throw std::runtime_error("Wrong function call");
        }
    }
};

int main() {
    Interpreter interpreter;

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
