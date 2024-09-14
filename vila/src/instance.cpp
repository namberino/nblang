//------------------------------------//
// Copyright 2024 Nam Nguyen
// Licensed under Apache License v2.0
//------------------------------------//

#include "instance.hpp"

NblInstance::NblInstance(std::shared_ptr<NblClass> klass)
    : klass(std::move(klass)) {}

std::any NblInstance::get(const Token& name)
{
    auto element = fields.find(name.lexeme);

    if (element != fields.end())
        return element->second;

    std::shared_ptr<NblFunction> method = klass->find_method(name.lexeme);

    if (method != nullptr)
        return method->bind(shared_from_this());

    throw RuntimeError(name, "Trường chưa được định nghĩa: '" + name.lexeme + "'");
}

void NblInstance::set(const Token& name, std::any value)
{
    fields[name.lexeme] = std::move(value);
}

std::string NblInstance::to_string()
{
    return "Thực thể " + klass->name;
}
