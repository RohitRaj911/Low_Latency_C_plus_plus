#include<bits/stdc++.h>
#include "../include/Order.hpp"
using namespace std;

Order :: Order(int OrderId, string symbol, int quantity, double price, Side side)
: m_OrderId(OrderId), m_symbol(symbol),m_quantity(quantity),m_price(price),m_side(side){

            cout << "{/*ORDER CONSTRUCTOR CALLED*/}\n[Order Created] ID: " << m_OrderId
              << ", Symbol: " << m_symbol
              << ", Qty: " << m_quantity
              << ", Price: " << m_price
              << ", Side: " << (m_side == Side::BUY ? "BUY" : "SELL") << "\n\n";
}
//Destructor
Order :: ~Order(){
    cout << "{/*ORDER DESTRUCTOR CALLED*/}\n[Order Destroyed] ID: " << m_OrderId << "\n\n";
}

// Move constructor
Order::Order(Order&& other) noexcept
    : m_OrderId(other.m_OrderId),
      m_symbol(std::move(other.m_symbol)),
      m_quantity(other.m_quantity),
      m_price(other.m_price),
      m_side(other.m_side)
{
    std::cout << "[Move Constructor] Order moved: " << m_OrderId << "\n\n";
}

// Move assignment operator
Order& Order::operator=(Order&& other) noexcept {
    if (this != &other) {
        m_OrderId = other.m_OrderId;
        m_symbol = std::move(other.m_symbol);
        m_quantity = other.m_quantity;
        m_price = other.m_price;
        m_side = other.m_side;
        std::cout << "[Move Assignment] Order moved: " << m_OrderId << "\n\n";
    }
    return *this;
}

int Order :: getId() const {return m_OrderId;}
string Order :: getSymbol() const {return m_symbol;}
int Order :: getQuantity() const {return m_quantity;}
double Order :: getPrice() const {return m_price;}
Side Order :: getSide() const {return m_side;}

void Order::printDetails() const {
    std::cout << "Order[ID=" << m_OrderId
              << ", Symbol=" << m_symbol
              << ", Quantity=" << m_quantity
              << ", Price=" << m_price
              << ", Side=" << (m_side == Side::BUY ? "BUY" : "SELL")
              << "]" << "\n\n";
}