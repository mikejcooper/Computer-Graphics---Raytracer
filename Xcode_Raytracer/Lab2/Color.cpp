//
//  Color.cpp
//  Lab2
//
//  Created by Mike Cooper on 21/04/2017.
//  Copyright © 2017 Bingjing Xu. All rights reserved.
//

#include "Color.hpp"

Color Color::operator+ (Color const &c) const {
  Color other;
  
  other.r = NTZ(c.r) + NTZ(r);
  other.g = NTZ(c.g) + NTZ(g);
  other.b = NTZ(c.b) + NTZ(b);
  
  return other;
}

Color Color::operator* (double amount) const {
  Color other;
  
  other.r = r * amount;
  other.g = g * amount;
  other.b = b * amount;
  
  return other;
}
