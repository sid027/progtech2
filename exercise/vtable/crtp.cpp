/*******************************************************************************
 *
 * We are looking at v(ege)tables.
 * Requires -std=c++14.
 * 
 * Programming Techniques for Scientific Simulations II, ETH Zürich, 2015
 * For free use, no rights reserved, no warranty whatsoever.
 * 
 ******************************************************************************/

#include <iostream>
#include <iomanip>

// Literal converter: days => hours
constexpr unsigned int operator"" _days(const unsigned long long int d) {
    return 24 * d;
}
// Literal converter: growth per month => growth per hour
constexpr double operator"" _growth_per_month(const long double d) {
    return d/30/24;
}
constexpr double operator"" _growth_per_month(const unsigned long long int d) {
    return d/30./24;
}

// c++11 strongly typed (uint8_t) enum
enum class VColor: uint8_t {orange, yellow, green, brown, none};

// print a nice string for the entries of VColor
std::string print_color(VColor const & color) {
    switch(color) {
        case VColor::orange:
            return "nice orange";
        case VColor::yellow:
            return "not so ripe yellow";
        case VColor::green:
            return "juicy green";
        case VColor::brown:
            return "rotten brown";
        default:
            return "non existent";
    }
}

// Class for common capabilities of one-dimensional vegetables
// Since this is used in a CRTP context, we don't have to use a vtable (no virtual methods)
template <typename D>
class OneDimVegetable {
    public:

    // We can call a static method we don't "know" yet exists.
    std::string get_name() {
        return print_color(color_) + " " + D::get_name();
    }

    void grow(const double & d) {      // default growth per hour
        ++age_;
        // The static cast allows us to bypass the vtable lookup. We still have
        // to pay the dereferenced function call (operator->), but nothing more.
        static_cast<D*>(this)->expire();
        length_ += d;
        weight_ += d * D::linear_density_;
    }

    inline double get_length() { return length_; }
    inline double get_weight() { return weight_; }

    protected:
    uint32_t age_ = 0;                         // hours
    double length_ = 0;                        // length in cm
    double weight_ = 0;                        // weight in grams
    VColor color_ = VColor::none;              // color
};

// Class for carrots
class Carrot : public OneDimVegetable<Carrot> {
    using super = OneDimVegetable<Carrot>;
    public:
    friend super;
    static const std::string get_name() {
        return "carrot";
    }
    void expire() {
        if(age_ > expiration_time_) // rotten :-/
            color_ = VColor::brown;
        else if(age_ > 10_days)     // ready for consumption
            color_ = VColor::orange;
        else                        // too early
            color_ = VColor::yellow;
    }
    private:
    static constexpr double linear_density_ = 4.5;
    static constexpr uint32_t expiration_time_ = 100_days;
};

// Class for cucumbers
class Cucumber : public OneDimVegetable<Cucumber> {
    using super = OneDimVegetable<Cucumber>;
    public:
    friend super;
    static const std::string get_name() {
        return "cucumber";
    }
    void expire() {
        if(age_ > expiration_time_) // rotten
            color_ = VColor::brown;
        else                        // ready to harvest
            color_ = VColor::green;
    }
    private:
    static constexpr double linear_density_ = 7.5;
    static constexpr uint32_t expiration_time_ = 130_days;
};

int main() {

    int time = 14_days;  // OneDimVegetables grow by the hour, but we use days
    Carrot ca;
    Cucumber cu;
    auto cap = new Carrot();
    OneDimVegetable<Carrot> * vp = new Carrot();

    // Growth rates per month curtesy from Wikipedia. They vary.
    while(time--) {
        ca.grow(4.9_growth_per_month);
        cu.grow(100_growth_per_month);
        cap->grow(5_growth_per_month);
        vp->grow(5.1_growth_per_month);
    }

    // age one carrot until it rots
    time = 100_days;
    while(time--)
        vp->grow(5.1_growth_per_month);

    // print all the grown vegetables
    std::cout << std::setprecision(3);
    std::cout << ca.get_name() << ":\t\t length " << ca.get_length()
              << " cm, weight " << ca.get_weight() << " g"  << std::endl;
    std::cout << cu.get_name() << ":\t\t length " << cu.get_length()
              << " cm, weight " << cu.get_weight() << " g" << std::endl;
    std::cout << cap->get_name() << " pointer:\t length " << cap->get_length()
              << " cm, weight " << cap->get_weight() << " g" << std::endl;
    std::cout << vp->get_name() << " from base:\t length " << vp->get_length()
              << " cm, weight " << vp->get_weight() << " g" << std::endl;
    std::cout << "\nCarrots take three months to grow fully." << std::endl;

    return 0;
}
