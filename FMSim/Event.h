#pragma once
#include <string>

class Event {
public:
    virtual ~Event() = default;

    virtual double getEffortCoefficient() const = 0;
    virtual std::string getName() const = 0;
};
