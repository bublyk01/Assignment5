#include <iostream>
#include <string>
#include <cctype>
#include <map>
#include <functional>
#include <stdexcept>
#include <vector>
#include <sstream>

class Interpreter {
public:
    Interpreter() {
        functions["add"] = [](const std::vector<double>& args) { return args[0] + args[1]; };
        functions["sub"] = [](const std::vector<double>& args) { return args[0] - args[1]; };
        functions["mul"] = [](const std::vector<double>& args) { return args[0] * args[1]; };
        functions["div"] = [](const std::vector<double>& args) { return args[0] / args[1]; };
    }

    double evaluate(const std::string& expression) {
        position = 0;
        input = expression;
        return parseExpression();
    }

    void defineFunction(const std::string& name, const std::vector<std::string>& params, const std::string& body) {
        functions[name] = [this, params, body](const std::vector<double>& args) {
            std::map<std::string, double> localVariables;
            for (size_t i = 0; i < params.size(); ++i) {
                localVariables[params[i]] = args[i];
            }
            return evaluateWithLocalVariables(body, localVariables);
            };
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
        if (position < input.size() && std::isdigit(input[position])) {
            return parseNumber();
        }
        else if (position < input.size() && std::isalpha(input[position])) {
            return parseFunctionCall();
        }
        else if (position < input.size() && input[position] == '(') {
            ++position;
            double result = parseExpression();
            ++position;
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
        ++position;
        std::vector<double> args;
        while (position < input.size() && input[position] != ')') {
            args.push_back(parseExpression());
            if (input[position] == ',') ++position;
        }
        ++position;
        return functions[functionName](args);
    }

    double evaluateWithLocalVariables(const std::string& expression, const std::map<std::string, double>& localVariables) {
        position = 0;
        input = expression;
        this->localVariables = localVariables;
        return parseExpression();
    }

    std::map<std::string, double> localVariables;
};

int main() {
    Interpreter interpreter;
    interpreter.defineFunction("square", { "x" }, "x * x");
    interpreter.defineFunction("sum_of_squares", { "a", "b" }, "square(a) + square(b)");

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
