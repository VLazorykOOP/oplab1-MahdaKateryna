#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <exception>

using namespace std;

// Exception classes
class ErrorRange : public exception
{
    string message;

public:
    double value;
    ErrorRange(double v) : value(v), message("ErrorRange: value out of range") {}
    const char* what() const noexcept override
    {
        return message.c_str();
    }
};

class ErrorNoFile : public exception
{
    string message;

public:
    ErrorNoFile(const string& filename) : message("ErrorNoFile: cannot open file " + filename) {}
    const char* what() const noexcept override
    {
        return message.c_str();
    }
};

class ErrorKrl : public exception
{
    string message;

public:
    double value;
    ErrorKrl(double v) : value(v), message("ErrorKrl: division by zero") {}
    const char* what() const noexcept override
    {
        return message.c_str();
    }
};

// Exception handler classes
class ExceptionHandler
{
public:
    virtual void handle(exception& e) = 0;
};

class ErrorRangeHandler : public ExceptionHandler
{
public:
    void handle(exception& e) override
    {
        auto& err = dynamic_cast<ErrorRange&>(e);
        cerr << err.what() << ": " << err.value << endl;
    }
};

class ErrorNoFileHandler : public ExceptionHandler
{
public:
    void handle(exception& e) override
    {
        auto& err = dynamic_cast<ErrorNoFile&>(e);
        cerr << err.what() << endl;
    }
};

class ErrorKrlHandler : public ExceptionHandler
{
public:
    void handle(exception& e) override
    {
        auto& err = dynamic_cast<ErrorKrl&>(e);
        cerr << err.what() << ": " << err.value << endl;
    }
};

// Function prototypes
double Tbl(double x);
double Krl(double x, double y, double z);
double Nrl(double x, double y);
double Grl(double x, double y, double z);
double fun(double x, double y, double z);

// Implementation of functions
double Tbl(double x)
{
    if (x > 10 || x < -10)
        throw ErrorRange(x);
    ifstream is("dat_1.dat");
    if (!is)
        throw ErrorNoFile("dat_1.dat");

    double yi, xi, yi1, xi1, y = 0;
    is >> xi1 >> yi1;
    if (xi1 == x)
        y = yi1;
    else
    {
        while (!is.eof())
        {
            xi = xi1;
            yi = yi1;
            is >> xi1 >> yi1;
            if (xi < x && x < xi1)
            {
                y = yi + (yi1 - yi) * (x - xi) / (xi1 - xi);
                break;
            }
            if (xi1 == x)
            {
                y = yi1;
                break;
            }
        }
    }
    is.close();
    return y;
}

double Krl(double x, double y, double z)
{
    if (x > 0 && y <= 1)
    {
        if (z == 0)
            throw ErrorKrl(x);
        return Tbl(x) + Tbl(y) / z;
    }
    else if (y > 1)
    {
        if (x == 0)
            throw ErrorKrl(x);
        return Tbl(y) + Tbl(z) / x;
    }
    else if (y == 0)
        throw ErrorKrl(x);
    else
        return Tbl(z) + Tbl(x) / z;
}

double Nrl(double x, double y)
{
    double temp = sqrt(x * x + y * y);
    if (temp == 0)
        throw ErrorKrl(temp);
    return x > y ? 0.42 * Krl(x / temp, y / temp, x / temp) : 0.57 * Krl(y / temp, x / temp, y / temp);
}

double Grl(double x, double y, double z)
{
    try
    {
        double temp = x + y;
        if (x + y >= z)
            temp += 0.3 * Nrl(x, z) + 0.7 * Nrl(y, z);
        else
            temp += 1.3 * Nrl(x, z) - 0.3 * Nrl(y, z);
        return temp;
    }
    catch (ErrorKrl& e)
    {
        ErrorKrlHandler handler;
        handler.handle(e);
        return e.value + 1;
    }
}

double fun(double x, double y, double z)
{
    double temp;
    try
    {
        temp = x * Grl(x, y, z) + y * Grl(y, z, x) + z * Grl(z, x, y);
    }
    catch (ErrorRange& e)
    {
        ErrorRangeHandler handler;
        handler.handle(e);
        z = e.value / 2;
        temp = x * Grl(x, y, z) + y * Grl(y, z, x) + z * Grl(z, x, y) - x * y * z * Grl(y, x, z);
    }
    catch (ErrorNoFile& e)
    {
        ErrorNoFileHandler handler;
        handler.handle(e);
        temp = 1.3498 * x + 2.2362 * y - 2.348 * x * y * z;
    }
    return temp;
}

int main()
{
    double x, y, z, f;
    cout << "Input x y z: ";
    cin >> x >> y >> z;

    try
    {
        f = fun(x, y, z);
    }
    catch (ErrorRange& e)
    {
        ErrorRangeHandler handler;
        handler.handle(e);
        f = e.value / 10;
    }
    catch (ErrorNoFile& e)
    {
        ErrorNoFileHandler handler;
        handler.handle(e);
        f = 1.3498 * x + 2.2362 * y - 2.348 * x * y * z;
    }
    catch (...)
    {
        cerr << "Unknown error occurred." << endl;
        f = 1.3498 * x + 2.2362 * y - 2.348 * x * y * z;
    }

    cout << "fun = " << f << endl;
    return 0;
}
