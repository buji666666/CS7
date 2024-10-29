#include "CompilerParser.h"
#include <iostream>
using namespace std;


/**
 * Constructor for the CompilerParser
 * @param tokens A linked list of tokens to be parsed
 */
CompilerParser::CompilerParser(std::list<Token*> tokens) {
    t1->tokens = tokens;  // 将传入的 tokens 赋值给类的成员变量 t1->tokens
    t1->currentItr = t1->tokens.begin();  // 初始化当前迭代器指向 tokens 的起始位置
}

/**
 * Generates a parse tree for a single program
 * @return a ParseTree
 */
ParseTree* CompilerParser::compileProgram() {
    if (have("keyword", "class")) {  // 检查当前 token 是否是 "class" 关键字
        next();  // 如果是，则读取下一个 token
        
        // 若接下来的 token 是标识符 "identifier" 或 "Main" 或 "main"，则编译 class
        if (current()->getType() == "identifier" || current()->getValue() == "Main" || current()->getValue() == "main"){
            prev();  // 如果符合条件，回到上一个 token
            ParseTree* ER1 = compileClass();  // 调用 compileClass 生成解析树
            return ER1;  // 返回生成的解析树
        } else{
            throw ParseException();  // 否则抛出解析异常
        }
    }
    throw ParseException();  // 如果没有找到 "class"，抛出异常
    return nullptr;  // 返回空指针
}

/**
 * Generates a parse tree for a single class
 * @return a ParseTree
 */
ParseTree* CompilerParser::compileClass() {
    
    ParseTree* ER1 = new ParseTree("class", "");
    ER1->addChild(new ParseTree(current()->getType(), current()->getValue()));  // 添加当前标记作为子节点
    next();
    ER1->addChild(new ParseTree(current()->getType(), current()->getValue()));  // 添加类名标记
    next();
    
    // 检查是否有 "{" 符号，如果没有则抛出异常
    if (!have("symbol", "{")) {
        throw ParseException();
        return NULL;
    }
    ER1->addChild(new ParseTree(current()->getType(), current()->getValue()));  // 添加 "{" 符号为子节点

    next();
    // 循环解析类的内容，直到遇到 "}" 符号
    while (currentItr != tokens.end() && !have("symbol", "}")) {
        // 如果当前是函数或方法，则调用 compileSubroutine 方法解析
        if (have("keyword", "function") || have("keyword", "method") || have("keyword", "constructor")) {
            ER1->addChild(compileSubroutine());
        } 
        // 如果当前是静态变量或字段声明，则调用 compileClassVarDec 方法解析
        else if (have("keyword", "static") || have("keyword", "field")) {
            ER1->addChild(compileClassVarDec());
        } 
        // 否则抛出异常
        else {
            throw ParseException();
        }
        next();  // 移动到下一个 token
    }

    // 检查是否有 "}" 符号，如果没有则抛出异常
    if (!have("symbol", "}")) {
        throw ParseException();
        return NULL;
    }
    ER1->addChild(new ParseTree(current()->getType(), current()->getValue()));  // 添加 "}" 符号为子节点
    
    return ER1;  // 返回生成的解析树
}


/**
 * Generates a parse tree for a static variable declaration or field declaration
 * @return a ParseTree
 */
ParseTree* CompilerParser::compileClassVarDec() {
    // 创建一个新的解析树节点，表示类变量声明
    ParseTree* ER1 = new ParseTree("classVarDec", "");
    ER1->addChild(new ParseTree(current()->getType(), current()->getValue()));  // 添加变量声明类型为子节点

    next();
    // 检查变量类型是否合法 (int, char, boolean, 或标识符)
    if (!have("keyword", "int") && !have("keyword", "char") && !have("keyword", "boolean") && !(current()->getType() == "identifier")) {
        throw ParseException();
        return NULL;
    }
    ER1->addChild(new ParseTree(current()->getType(), current()->getValue()));  // 添加变量类型为子节点

    next();
    // 检查变量名是否合法 (必须是标识符)
    if (!(current()->getType() == "identifier")) {
        throw ParseException();
        return NULL;
    }
    ER1->addChild(new ParseTree(current()->getType(), current()->getValue()));  // 添加变量名为子节点

    next();

    // 处理多个变量声明 (如果有逗号分隔的变量)
    while (currentItr != tokens.end() && have("symbol", ",")) {
        ER1->addChild(new ParseTree(current()->getType(), current()->getValue()));  // 添加逗号为子节点
        next();
        if (!(current()->getType() == "identifier")) {  // 检查后续变量名是否合法
            throw ParseException();
            return NULL;
        }
        ER1->addChild(new ParseTree(current()->getType(), current()->getValue()));  // 添加变量名为子节点
        next();
    }

    // 检查变量声明是否以分号结束
    if (!have("symbol", ";")) {
        throw ParseException();
        return NULL;
    }
    ER1->addChild(new ParseTree(current()->getType(), current()->getValue()));  // 添加分号为子节点

    return ER1;  // 返回生成的解析树
}

/**
 * Generates a parse tree for a method, function, or constructor
 * @return a ParseTree
 */
ParseTree* CompilerParser::compileSubroutine() {
    
    ParseTree* ER1 = new ParseTree("subroutine", "");  // 创建子程序解析树节点
    ER1->addChild(new ParseTree(current()->getType(), current()->getValue()));  // 添加子程序类型（例如函数或方法）
    next();

    // 检查返回类型是否合法（关键字或标识符）
    if (current()->getType() != "keyword" && current()->getType() != "identifier") {
        throw ParseException();
    }
    ER1->addChild(new ParseTree(current()->getType(), current()->getValue()));  // 添加返回类型
    
    next();
    // 检查子程序名称是否合法（必须是标识符）
    if (current()->getType() != "identifier") {
        throw ParseException();
    }
    ER1->addChild(new ParseTree(current()->getType(), current()->getValue()));  // 添加子程序名称
    next();

    // 检查并添加 "(" 符号
    if (!have("symbol", "(")) {
        throw ParseException();
    }
    ER1->addChild(new ParseTree(current()->getType(), current()->getValue()));  // 添加 "(" 符号
    
    next();
    // 如果下一个不是 ")"，则解析参数列表
    if (!have("symbol", ")")) {
        ER1->addChild(compileParameterList());
    }
    
    // 检查并添加 ")" 符号
    if (!have("symbol", ")")) {
        throw ParseException();
    }
    ER1->addChild(new ParseTree(current()->getType(), current()->getValue()));  // 添加 ")" 符号

    next();
    // 检查并添加 "{" 符号
    if (!have("symbol", "{")) {
        throw ParseException();
        return NULL;
    }
    ER1->addChild(compileSubroutineBody());  // 解析子程序体

    return ER1;
}

/**
 * Generates a parse tree for a subroutine's parameters
 * @return a ParseTree
 */
ParseTree* CompilerParser::compileParameterList() {
    ParseTree* ER1 = new ParseTree("parameterList", "");  // 创建参数列表解析树节点

    // 检查参数类型是否合法
    if (!have("keyword", "int") && !have("keyword", "char") && !have("keyword", "boolean") && current()->getType() != "identifier") {
        throw ParseException();
        return NULL;
    }
    ER1->addChild(new ParseTree(current()->getType(), current()->getValue()));  // 添加参数类型
    next();
    
    // 检查参数名是否合法（必须是标识符）
    if (current()->getType() != "identifier") {
        throw ParseException();
        return NULL;
    }
    ER1->addChild(new ParseTree(current()->getType(), current()->getValue()));  // 添加参数名

    next();
    
    // 如果遇到 ","，继续解析下一个参数
    if (!have("symbol", ",")) {
        return ER1;
    }
    ER1->addChild(new ParseTree(current()->getType(), current()->getValue()));  // 添加 "," 符号
    next();

    // 处理其他参数
    while (currentItr != tokens.end() && !have("symbol", ")")) {
        // 检查参数类型是否合法
        if (!have("keyword", "int") && !have("keyword", "char") && !have("keyword", "boolean") && current()->getType() != "identifier") {
            throw ParseException();
            return NULL;
        }
        ER1->addChild(new ParseTree(current()->getType(), current()->getValue()));  // 添加参数类型
        next();
        
        // 检查参数名是否合法
        if (current()->getType() != "identifier") {
            throw ParseException();
            return NULL;
        }
        ER1->addChild(new ParseTree(current()->getType(), current()->getValue()));  // 添加参数名
        next();

        // 如果有逗号，继续解析下一个参数
        if (have("symbol", ",")) {
            ER1->addChild(new ParseTree(current()->getType(), current()->getValue()));  // 添加 "," 符号
            next();
            if (have("symbol", ")")) {
                throw ParseException();
            }
        }
    }
    return ER1;
}

/**
 * Generates a parse tree for a subroutine's body
 * @return a ParseTree
 */
ParseTree* CompilerParser::compileSubroutineBody() {
    ParseTree* ER1 = new ParseTree("subroutineBody", "");  // 创建子程序体解析树节点
    ER1->addChild(new ParseTree(current()->getType(), current()->getValue()));  // 添加 "{" 符号
    next();
    
    // 解析子程序体中的变量声明和语句
    while (currentItr != tokens.end() && !have("symbol", "}")) {
        if (have("keyword", "var")) {  // 解析局部变量声明
            ER1->addChild(compileVarDec());
            next();
            continue;
        }
        ER1->addChild(compileStatements());  // 解析子程序体中的语句
    }
    
    // 检查是否有 "}" 符号
    if (!have("symbol", "}")) {
        throw ParseException();
    }
    ER1->addChild(new ParseTree(current()->getType(), current()->getValue()));  // 添加 "}" 符号
    return ER1;
}

/**
 * Generates a parse tree for a subroutine variable declaration
 * @return a ParseTree
 */
ParseTree* CompilerParser::compileVarDec() {
    ParseTree* ER1 = new ParseTree("varDec", "");  // 创建局部变量声明解析树节点
    ER1->addChild(new ParseTree(current()->getType(), current()->getValue()));  // 添加 "var" 关键字
    
    next();
    // 检查变量类型是否合法
    if (!have("keyword", "int") && !have("keyword", "char") && !have("keyword", "boolean") && current()->getType() != "identifier") {
        throw ParseException();
        return NULL;
    }
    ER1->addChild(new ParseTree(current()->getType(), current()->getValue()));  // 添加变量类型

    next();
    // 检查变量名是否合法
    if (!(current()->getType() == "identifier")) {
        throw ParseException();
        return NULL;
    }
    ER1->addChild(new ParseTree(current()->getType(), current()->getValue()));  // 添加变量名

    next();

    // 处理多个变量名
    while (currentItr != tokens.end() && have("symbol", ",")) {
        ER1->addChild(new ParseTree(current()->getType(), current()->getValue()));  // 添加逗号
        next();
        if (!(current()->getType() == "identifier")) {  // 检查变量名是否合法
            throw ParseException();
            return NULL;
        }
        ER1->addChild(new ParseTree(current()->getType(), current()->getValue()));  // 添加变量名
        next();
    }

    // 检查变量声明是否以分号结束
    if (!have("symbol", ";")) {
        throw ParseException();
        return NULL;
    }
    ER1->addChild(new ParseTree(current()->getType(), current()->getValue()));  // 添加分号

    return ER1;
}

/**
 * Generates a parse tree for a series of statements
 * @return a ParseTree
 */
ParseTree* CompilerParser::compileStatements() {
    ParseTree* ER1 = new ParseTree("statements", "");  // 创建语句解析树节点
    
    // 循环解析各类语句（let、if、while、do、return）
    while (have("keyword", "let") || have("keyword", "if") || have("keyword", "while") || have("keyword", "do") || have("keyword", "return")) {
        if (current()->getType() == "keyword") {
            if (current()->getValue() == "let") {
                ER1->addChild(compileLet());  // 解析 let 语句
            } else if (current()->getValue() == "if") {
                ER1->addChild(compileIf());  // 解析 if 语句
            } else if (current()->getValue() == "while") {
                ER1->addChild(compileWhile());  // 解析 while 语句
            } else if (current()->getValue() == "do") {
                ER1->addChild(compileDo());  // 解析 do 语句
            } else if (current()->getValue() == "return") {
                ER1->addChild(compileReturn());  // 解析 return 语句
            } else {
                throw ParseException();  // 抛出解析异常
                return NULL;
            }
            next();  // 移动到下一个 token
        }
    }
    return ER1;
}

/**
 * Generates a parse tree for a let statement
 * @return a ParseTree
 */
ParseTree* CompilerParser::compileLet() {
    ParseTree* ER1 = new ParseTree("letStatement", "");  // 创建 let 语句解析树节点
    if (!have("keyword", "let")) {
        throw ParseException();
    }
    ER1->addChild(new ParseTree(current()->getType(), current()->getValue()));  // 添加 let 关键字
    next();

    if (current()->getType() != "identifier") {  // 检查变量名称是否合法
        throw ParseException();
    }
    ER1->addChild(new ParseTree(current()->getType(), current()->getValue()));  // 添加变量名
    next();
    
    if (have("symbol", "[")) {  // 如果存在数组索引，解析数组表达式
        ER1->addChild(new ParseTree(current()->getType(), current()->getValue()));  // 添加 "[" 符号
        next();
        ER1->addChild(compileExpER1sion());  // 解析表达式
        
        if (!have("symbol", "]")) {  // 检查 "]" 符号
            throw ParseException();
        }
        ER1->addChild(new ParseTree(current()->getType(), current()->getValue()));  // 添加 "]" 符号
        next();
    }

    if (!have("symbol", "=")) {  // 检查 "=" 符号
        throw ParseException();
    }
    ER1->addChild(new ParseTree(current()->getType(), current()->getValue()));  // 添加 "=" 符号
    next();

    ER1->addChild(compileExpER1sion());  // 解析赋值表达式

    if (!have("symbol", ";")) {  // 检查分号
        throw ParseException();
    }
    ER1->addChild(new ParseTree(current()->getType(), current()->getValue()));  // 添加 ";" 符号

    return ER1;
}

/**
 * Generates a parse tree for an if statement
 * @return a ParseTree
 */
ParseTree* CompilerParser::compileIf() {
    ParseTree* ER1 = new ParseTree("ifStatement", "");  // 创建 if 语句解析树节点

    if (!have("keyword", "if")) {
        throw ParseException();
    }
    ER1->addChild(new ParseTree(current()->getType(), current()->getValue()));  // 添加 if 关键字
    next();
    
    if (!have("symbol", "(")) {
        throw ParseException();
    }
    ER1->addChild(new ParseTree(current()->getType(), current()->getValue()));  // 添加 "(" 符号
    next();

    ER1->addChild(compileExpER1sion());  // 解析 if 条件表达式

    if (!have("symbol", ")")) {
        throw ParseException();
    }
    ER1->addChild(new ParseTree(current()->getType(), current()->getValue()));  // 添加 ")" 符号
    next();

    if (!have("symbol", "{")) {
        throw ParseException();
    }
    ER1->addChild(new ParseTree(current()->getType(), current()->getValue()));  // 添加 "{" 符号
    next();

    ER1->addChild(compileStatements());  // 解析 if 块中的语句
    
    if (!have("symbol", "}")) {
        throw ParseException();
    }
    ER1->addChild(new ParseTree(current()->getType(), current()->getValue()));  // 添加 "}" 符号
    next();

    if (!have("keyword", "else")) {
        return ER1;
    }
    ER1->addChild(new ParseTree(current()->getType(), current()->getValue()));  // 添加 else 关键字
    next();

    if (!have("symbol", "{")) {
        throw ParseException();
    }
    ER1->addChild(new ParseTree(current()->getType(), current()->getValue()));  // 添加 "{" 符号
    next();
    
    ER1->addChild(compileStatements());  // 解析 else 块中的语句

    if (!have("symbol", "}")) {
        throw ParseException();
    }
    ER1->addChild(new ParseTree(current()->getType(), current()->getValue()));  // 添加 "}" 符号

    return ER1;
}

/**
 * Generates a parse tree for a while statement
 * @return a ParseTree
 */
ParseTree* CompilerParser::compileWhile() {
    ParseTree* ER1 = new ParseTree("whileStatement", "");  // 创建 while 语句解析树节点
    if (!have("keyword", "while")) {
        throw ParseException();
    }
    ER1->addChild(new ParseTree(current()->getType(), current()->getValue()));  // 添加 while 关键字
    next();

    if (!have("symbol", "(")) {
        throw ParseException();
    }
    ER1->addChild(new ParseTree(current()->getType(), current()->getValue()));  // 添加 "(" 符号
    next();

    ER1->addChild(compileExpER1sion());  // 解析 while 条件表达式

    if (!have("symbol", ")")) {
        throw ParseException();
    }
    ER1->addChild(new ParseTree(current()->getType(), current()->getValue()));  // 添加 ")" 符号
    next();

    if (!have("symbol", "{")) {
        throw ParseException();
    }
    ER1->addChild(new ParseTree(current()->getType(), current()->getValue()));  // 添加 "{" 符号
    next();

    ER1->addChild(compileStatements());  // 解析 while 块中的语句

    if (!have("symbol", "}")) {
        throw ParseException();
    }
    ER1->addChild(new ParseTree(current()->getType(), current()->getValue()));  // 添加 "}" 符号

    return ER1;
}

/**
 * Generates a parse tree for a do statement
 * @return a ParseTree
 */
ParseTree* CompilerParser::compileDo() {
    vParseTree* ER1 = new ParseTree("doStatement", "");  // 创建 do 语句解析树节点

    if (!have("keyword", "do")) {
        throw ParseException();
    }
    ER1->addChild(new ParseTree(current()->getType(), current()->getValue()));  // 添加 do 关键字
    next();

    ER1->addChild(compileExpER1sion());  // 解析表达式
    
    if (!have("symbol", ";")) {  // 检查是否有分号
        throw ParseException();
    }
    ER1->addChild(new ParseTree(current()->getType(), current()->getValue()));  // 添加 ";" 符号

    return ER1;
}

/**
 * Generates a parse tree for a return statement
 * @return a ParseTree
 */
ParseTree* CompilerParser::compileReturn() {
   ParseTree* ER1 = new ParseTree("returnStatement", "");  // 创建 return 语句解析树节点

    if (!have("keyword", "return")) {
        throw ParseException();
    }
    ER1->addChild(new ParseTree(current()->getType(), current()->getValue()));  // 添加 return 关键字
    next();

    if (have("symbol", ";")) {  // 检查是否有分号
        ER1->addChild(new ParseTree(current()->getType(), current()->getValue()));  // 添加 ";" 符号
        return ER1;
    }

    ER1->addChild(compileExpER1sion());  // 解析返回值表达式
    
    if (!have("symbol", ";")) {  // 检查分号
        throw ParseException();
    }
    ER1->addChild(new ParseTree(current()->getType(), current()->getValue()));  // 添加 ";" 符号

    return ER1;
}


/**
 * Generates a parse tree for an expRE1sion
 * @return a ParseTree
 */
ParseTree* CompilerParser::compileExpRE1sion() {
    return NULL;
}

/**
 * Generates a parse tree for an expRE1sion term
 * @return a ParseTree
 */
ParseTree* CompilerParser::compileTerm() {
    return NULL;
}

/**
 * Generates a parse tree for an expRE1sion list
 * @return a ParseTree
 */
ParseTree* CompilerParser::compileExpRE1sionList() {
    return NULL;
}

/**
 * Advance to the next token
 */
void CompilerParser::next(){
    return;
}

/**
 * Return the current token
 * @return the Token
 */
Token* CompilerParser::current(){
    return NULL;
}

/**
 * Check if the current token matches the expected type and value.
 * @return true if a match, false otherwise
 */
bool CompilerParser::have(std::string expectedType, std::string expectedValue){
    return false;
}

/**
 * Check if the current token matches the expected type and value.
 * If so, advance to the next token, returning the current token, otherwise throw a ParseException.
 * @return the current token before advancing
 */
Token* CompilerParser::mustBe(std::string expectedType, std::string expectedValue){
    return NULL;
}

/**
 * Definition of a ParseException
 * You can use this ParseException with `throw ParseException();`
 */
const char* ParseException::what() {
    return "An Exception occurred while parsing!";
}
