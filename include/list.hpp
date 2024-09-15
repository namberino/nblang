//------------------------------------//
// Bản quyền © 2024 Nguyễn Bình Nam
// Được cấp phép theo Giấy phép Apache v2.0
//------------------------------------//

#ifndef LIST_HPP
#define LIST_HPP

#pragma once
#include <vector>
#include <any>

#include "expr.hpp"

struct ListType
{
    std::vector<std::any> elements;

    void append(std::any value);
    std::any get_element_at(int index);
    bool set_element_at(int index, std::any value);
    int get_length();
};

#endif
