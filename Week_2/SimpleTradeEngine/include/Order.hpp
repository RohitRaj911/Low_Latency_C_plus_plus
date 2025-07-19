#pragma once

#include<bits/stdc++.h>
using namespace std;

enum class Side{
    BUY,
    SELL
};

class Order{
    public:
        Order(int OrderId, string symbol, int quantity, double price, Side side);
        ~Order();

        // Disallow copy
        Order(const Order&) = delete;
        Order& operator=(const Order&) = delete;

        // Allow move
        Order(Order&& other) noexcept;
        Order& operator=(Order&& other) noexcept;

        int getId() const;
        string getSymbol() const;
        int getQuantity() const;
        double getPrice() const;
        void printDetails() const;
        Side getSide() const;

    private:
        int m_OrderId;
        string m_symbol;
        int m_quantity;
        double m_price;
        Side m_side;
};