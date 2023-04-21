#ifndef _DEBUG_H_
#define _DEBUG_H_

#pragma once

#include "stdbool.h"
#include "WarehousePickingOpt.h"

#include <iostream>
using namespace std;

/// @brief property enabling debug info
inline bool is_debug_on = false;

class Customer;
class Cart;
class BoxItem;
class Box;

#define DEBUG_MSG(msg, ...) is_debug_on ? fprintf(stderr, msg, ##__VA_ARGS__) : 0

void debugCustomerMultimap(multimap<int, Customer, greater<int>> cust_mult);
void debugCustomer(Customer cust);
void debugCustomerBoxItems(vector<BoxItem> boxitem_vect);
void debugCustomerBoxes(vector<Box> box_vect);

void debugCartsCustomers(list<Cart> cart_list);
void debugCartsOItems(list<Cart> cart_list, bool additional_spec = false);
void debugCartsBoxes(list<Cart> cart_list);

#endif