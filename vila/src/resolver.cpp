//------------------------------------//
// Copyright 2024 Nam Nguyen
// Licensed under Apache License v2.0
//------------------------------------//

#include "resolver.hpp"

Resolver::Resolver(Interpreter& interpreter, std::string& executed_path)
    : interpreter(interpreter), executed_path(executed_path) {}

void Resolver::resolve(const std::vector<std::shared_ptr<Stmt>>& statements)
{
    for (const std::shared_ptr<Stmt>& statement : statements)
        resolve(statement);
}


std::any Resolver::visitAssignExpr(std::shared_ptr<AssignExpr> expr)
{
    resolve(expr->value);
    resolve_local(expr, expr->name);
    return {};
}

std::any Resolver::visitBinaryExpr(std::shared_ptr<BinaryExpr> expr)
{
    resolve(expr->left);
    resolve(expr->right);
    return {};
}

std::any Resolver::visitGroupingExpr(std::shared_ptr<GroupingExpr> expr)
{
    resolve(expr->expression);
    return {};
}

std::any Resolver::visitLiteralExpr(std::shared_ptr<LiteralExpr> expr)
{
    return {};
}

std::any Resolver::visitUnaryExpr(std::shared_ptr<UnaryExpr> expr)
{
    resolve(expr->right);
    return {};
}

std::any Resolver::visitMutExpr(std::shared_ptr<MutExpr> expr)
{
    if (!scopes.empty())
    {
        auto& current_scope = scopes.back();
        auto element = current_scope.find(expr->name.lexeme);

        if (element != current_scope.end() && element->second == false)
            Error::error(expr->name, "Không thể đọc biến cục bộ trong phần khởi tạo chính nó");

    }
    resolve_local(expr, expr->name);
    return {};
}

std::any Resolver::visitLogicalExpr(std::shared_ptr<LogicalExpr> expr)
{
    resolve(expr->left);
    resolve(expr->right);
    return {};
}

std::any Resolver::visitCallExpr(std::shared_ptr<CallExpr> expr)
{
    resolve(expr->callee);

    for (const std::shared_ptr<Expr>& argument : expr->arguments)
        resolve(argument);

    return {};
}

std::any Resolver::visitFunctionExpr(std::shared_ptr<FunctionExpr> expr)
{
    resolve_function(expr, FunctionType::FUNCTION);
    return {};
}

std::any Resolver::visitGetExpr(std::shared_ptr<GetExpr> expr)
{
    resolve(expr->object);
    return {};
}

std::any Resolver::visitSetExpr(std::shared_ptr<SetExpr> expr)
{
    resolve(expr->value);
    resolve(expr->object);
    return {};
}

std::any Resolver::visitThisExpr(std::shared_ptr<ThisExpr> expr)
{
    if (current_class == ClassType::NONE)
    {
        Error::error(expr->keyword, "Không thể dùng câu lệnh 'đây' ở ngoài lớp");
        return {};
    }
    resolve_local(expr, expr->keyword);

    return {};
}


std::any Resolver::visitSuperExpr(std::shared_ptr<SuperExpr> expr)
{
    // check if we're currently in a scope where super is allowed
    if (current_class == ClassType::NONE)
        Error::error(expr->keyword, "Không thể dùng câu lệnh 'gốc' ở ngoài lớp");
    else if (current_class != ClassType::SUBCLASS)
        Error::error(expr->keyword, "Không thể dùng câu lệnh 'gốc' trong lớp không kế thừa từ lớp khác");

    resolve_local(expr, expr->keyword);
    return {};
}

std::any Resolver::visitListExpr(std::shared_ptr<ListExpr> expr)
{
    for (std::shared_ptr<Expr> element : expr->elements)
        resolve(element);
    return {};
}

std::any Resolver::visitSubscriptExpr(std::shared_ptr<SubscriptExpr> expr)
{
    resolve(expr->name);
    resolve(expr->index);

    if (expr->value != nullptr)
        resolve(expr->value);

    return {};
}

std::any Resolver::visitBlockStmt(std::shared_ptr<BlockStmt> stmt)
{
    begin_scope();
    resolve(stmt->statements);
    end_scope();
    return {};
}

std::any Resolver::visitExpressionStmt(std::shared_ptr<ExpressionStmt> stmt)
{
    resolve(stmt->expression);
    return {};
}

std::any Resolver::visitPrintStmt(std::shared_ptr<PrintStmt> stmt)
{
    resolve(stmt->expression);
    return {};
}

std::any Resolver::visitMutStmt(std::shared_ptr<MutStmt> stmt)
{
    declare(stmt->name);

    if (stmt->initializer != nullptr)
        resolve(stmt->initializer);
    define(stmt->name);

    return {};
}

std::any Resolver::visitIfStmt(std::shared_ptr<IfStmt> stmt)
{
    resolve(stmt->condition);
    resolve(stmt->then_branch);

    if (stmt->else_branch != nullptr)
        resolve(stmt->else_branch);

    return {};
}

std::any Resolver::visitWhileStmt(std::shared_ptr<WhileStmt> stmt)
{
    resolve(stmt->condition);
    resolve(stmt->body);
    return {};
}

std::any Resolver::visitFunctionStmt(std::shared_ptr<FunctionStmt> stmt)
{
    declare(stmt->name);
    define(stmt->name);

    resolve_function(stmt->fn, FunctionType::FUNCTION);
    return {};
}

std::any Resolver::visitReturnStmt(std::shared_ptr<ReturnStmt> stmt)
{
    if (current_func == FunctionType::NONE)
        Error::error(stmt->keyword, "Không thể sử dụng câu lệnh 'trả' ở ngoài hàm");

    if (stmt->value != nullptr)
    {
        if (current_func == FunctionType::INITIALIZER)
            Error::error(stmt->keyword, "Không thể sử dụng câu lệnh 'trả' trong hàm tạo");
        resolve(stmt->value);
    }

    return {};
}

std::any Resolver::visitBreakStmt(std::shared_ptr<BreakStmt> stmt)
{
    return {};
}

std::any Resolver::visitClassStmt(std::shared_ptr<ClassStmt> stmt)
{
    ClassType enclosing_class = current_class;
    current_class = ClassType::CLASS;

    declare(stmt->name);
    define(stmt->name);

    if (stmt->superclass != nullptr && stmt->name.lexeme == stmt->superclass->name.lexeme)
        Error::error(stmt->superclass->name, "Lớp không thể kế thừa từ chính nó");
    
    if (stmt->superclass != nullptr)
    {
        current_class = ClassType::SUBCLASS;
        resolve(stmt->superclass);
    }

    if (stmt->superclass != nullptr)
    {
        begin_scope();
        scopes.back()["super"] = true;
    }

    begin_scope();
    scopes.back()["this"] = true;
    for (std::shared_ptr<FunctionStmt> method : stmt->methods)
    {
        FunctionType declaration = FunctionType::METHOD;

        if (method->name.lexeme == "khởi_tạo")
            declaration = FunctionType::INITIALIZER;

        resolve_function(method->fn, declaration);
    }
    end_scope();

    if (stmt->superclass != nullptr)
        end_scope(); // end superclass scope

    current_class = enclosing_class;

    return {};
}

std::any Resolver::visitImportStmt(std::shared_ptr<ImportStmt> stmt)
{
    std::string target = std::any_cast<std::string>(stmt->target->value);
    bool is_core = false;

    if (target.find("lõi:") != 0)
    {
        fs::path cwd = fs::current_path();
        fs::path relative_cwd = cwd / executed_path;
        target = relative_cwd.parent_path().string() + (target[0] == '/' ? target : "/" + target) + ".nbl";
    }
    else
    {
        is_core = true;
        target = target.substr(5); // remove "core:" prefix
        target = core_lib_dir + "/" + target + ".nbl";
    }

    std::ifstream file(target);
    if (!file.good())
        Error::error(stmt->keyword, (is_core ? "Thư viện" : "File") + std::string(" '") + target + "' không thể tìm được");

    stmt->target->value = target;

    return {};
}


void Resolver::resolve(std::shared_ptr<Stmt> stmt)
{
    stmt->accept(*this);
}

void Resolver::resolve(std::shared_ptr<Expr> expr)
{
    expr->accept(*this);
}

void Resolver::resolve_function(std::shared_ptr<FunctionExpr> fn, FunctionType type)
{
    FunctionType enclosing_func = current_func;
    current_func = type;

    begin_scope();
    for (const Token& param : fn->parameters)
    {
        declare(param);
        define(param);
    }
    resolve(fn->body);
    end_scope();

    current_func = enclosing_func;
}

void Resolver::resolve_local(std::shared_ptr<Expr> expr, const Token& name)
{
    for (int i = scopes.size() - 1; i >= 0; i--)
    {
        if (scopes[i].find(name.lexeme) != scopes[i].end())
        {
            interpreter.resolve(expr, scopes.size() - i - 1);
        }
    }
}

void Resolver::declare(const Token& name)
{
    if (scopes.empty())
        return;
    
    std::map<std::string, bool>& scope = scopes.back();

    if (scope.find(name.lexeme) != scope.end())
        Error::error(name, "Đã có biến với tên này trong phạm vi hiện tại");
    scope[name.lexeme] = false;
}

void Resolver::define(const Token& name)
{
    if (scopes.empty())
        return;
    scopes.back()[name.lexeme] = true;
}

void Resolver::begin_scope()
{
    scopes.push_back(std::map<std::string, bool>{});
}

void Resolver::end_scope()
{
    scopes.pop_back();
}

fs::path Resolver::get_base_path()
{
    fs::path current_path = fs::current_path();
    fs::path known_directory = "namlang"; // change if known directory is different

    // find the known directory in current path
    auto it = std::find(current_path.begin(), current_path.end(), known_directory);
    if (it == current_path.end())
        throw std::runtime_error("Folder không tìm được trong path hiện tại");

    // create base path by joining parts of current path up to the known directory
    fs::path base_path;
    for (auto part = current_path.begin(); part != it; ++part)
        base_path /= *part;
    base_path /= known_directory;

    return base_path;
}
