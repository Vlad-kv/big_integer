#include "big_integer.h"

void big_integer::upd_max_size(int new_max_size) {
  int w;
  loc_t *s = new loc_t[new_max_size];
  for (w = std::min(max_size, new_max_size) - 1; w >= 0; w--) {
    s[w] = a[w];
  }
  delete []a;
  a = s;
  for (w = max_size; w < new_max_size; w++) {
    a[w] = 0;
  }
  max_size = new_max_size;
}
void big_integer::check_max_size() {
  if ((max_size > start_max_size) && (max_size / real_size >= 4)) {
    int w;
    for (w = start_max_size; w < real_size; w *= 2) {
      ;
    }
    if (w != start_max_size) {
      upd_max_size(w);
    }
  }
}
void big_integer::upd_real_size() {
  int w;
  for (w = real_size - 1; (w > 0) && (a[w] == 0); w--) {
    ;
  }
  real_size = w + 1;
  if ((w == 0) && (a[0] == 0)) {
    inv = 1;
  }
  check_max_size();
}

void big_integer::add(big_integer const& s) {
  int w = max_size;
  while (w < std::max(real_size, s.real_size) + 1) {
    w <<= 1;
  }
  if (w != max_size) {
    upd_max_size(w);
  }
  real_size = std::max(real_size, s.real_size) + 1;
  loc_t overflow = 0;
  buf_t next;
  for (w = 0; w < s.real_size; w++) {
    next = (buf_t)a[w] + s.a[w] + overflow;
    overflow = 0;
    if (next >= size_loc_t) {
      next -= size_loc_t;
      overflow = 1;
    }
    a[w] = next;
  }
  
  for (; (w < max_size) && (overflow); w++) {
    next = (buf_t)a[w] + overflow;
    overflow = 0;
    if (next >= size_loc_t) {
      next -= size_loc_t;
      overflow = 1;
    }
    a[w] = next;
  }
  upd_real_size();
}
void big_integer::sub(big_integer const& s) {
  int w = max_size;
  while (w < std::max(real_size, s.real_size) + 1) {
    w <<= 1;
  }
  if (w != max_size) {
    upd_max_size(w);
  }
  loc_t overflow = 0;
  buf_t next;
  
  for (w = 0; w < s.real_size; w++) {
    next = (buf_t)a[w] - s.a[w] - overflow;
    overflow = 0;
    if (next < 0) {
      next += size_loc_t;
      overflow = 1;
    }
    a[w] = next;
  }
  
  for (; w < (max_size) && (overflow); w++) {
    next = a[w] - overflow;
    overflow = 0;
    if (next < 0) {
      next += size_loc_t;
      overflow = 1;
    }
    a[w] = next;
  }
  upd_real_size();
}

int big_integer::cmp(big_integer const& s) const{
  if ((inv < 0) && (s.inv > 0)) {
    return 1;
  }
  if ((inv > 0) && (s.inv < 0)) {
    return -1;
  }
  if (real_size < s.real_size) {
    return -inv;
  }
  if (real_size > s.real_size) {
    return inv;
  }
  for (int w = real_size - 1; w >= 0; w--) {
    if (a[w] < s.a[w]) {
      return -inv;
    }
    if (a[w] > s.a[w]) {
      return inv;
    }
  }
  return 0;
}
int big_integer::abs_cmp(big_integer const& s) {
  if (real_size < s.real_size) {
    return -1;
  }
  if (real_size > s.real_size) {
    return 1;
  }
  for (int w = real_size - 1; w >= 0; w--) {
    if (a[w] < s.a[w]) {
      return -1;
    }
    if (a[w] > s.a[w]) {
      return 1;
    }
  }
  return 0;
}

void big_integer::left_shift(int shift) {
  int w_shift = shift / pw_loc_t;
  shift %= pw_loc_t;
  int w = max_size;
  while (w < real_size + w_shift + 1) {
    w <<= 1;
  }
  if (w != max_size) {
    upd_max_size(w);
  }
  loc_t e;
  for (w = real_size - 1; w >= 0; w--) {
    e = a[w];
    a[w] = 0;
    a[w + w_shift + 1] |= e >> (pw_loc_t - shift);
    a[w + w_shift] = (e & ((1 << (pw_loc_t - shift)) - 1) ) << shift;
  }
  real_size += w_shift + 1;
  upd_real_size();
}
void big_integer::right_shift(int shift) {
  int w_shift = shift / pw_loc_t;
  shift %= pw_loc_t;
  int w;
  bool overflow = ((a[0] & ((1<<shift) - 1)) != 0);
  
  for (w = 0; w < real_size; w++) {
    if (w) {
      a[w - 1] |= (a[w] & ((1 << shift) - 1)) << (pw_loc_t - shift);
    }
    a[w] >>= shift;
  }
  
  for (w = 0; w < real_size; w++) {
    if ((w < w_shift) && (a[w])) {
      overflow = 1;
    }
    if (w + w_shift < real_size) {
      a[w] = a[w + w_shift];
    } else {
      a[w] = 0;
    }
  }
  if ((inv < 0) && (overflow)) {
    --(*this);
  }
  upd_real_size();
}



big_integer::big_integer() {
  max_size = start_max_size;
  a = new loc_t[max_size];
  for (int w = 0; w < max_size; w++) {
    a[w] = 0;
  }
  inv = 1;
  real_size = 1;
}
big_integer::big_integer(big_integer const& v) {
  max_size = v.max_size;
  real_size = v.real_size;
  inv = v.inv;
  a = new loc_t[max_size];
  for (int w = 0; w < max_size; w++) {
    a[w] = v.a[w];
  }
}
big_integer::big_integer(int t) {
  max_size = start_max_size;
  a = new loc_t[max_size];
  real_size = 1;
  inv = -1;
  if (t >= 0) {
    t = -t;
    inv = 1;
  }
  for (int w = 0; w < max_size; w++) {
    a[w] = -(t % size_loc_t);
    t /= size_loc_t;
    if (a[w]) {
      real_size = w + 1;
    }
  }
}
big_integer::big_integer(std::string const& str) {
  int w, t, len = str.length();
  inv = 1;
  char *s = new char[len];
  for (w = 0; w < len; w++) {
    s[w] = str[w];
  }
  if (s[0] == '-') {
    inv = -1;
    s[0] = 0;
  } else {
    s[0] -= 48;
  }
  for (w = 1; w < len; w++) {
    s[w] -= 48;
  }
  max_size = start_max_size;
  a = new loc_t[max_size];
  t = 0;
  bool b = 1;
  buf_t next;
  while (b) {
    b = 0;
    next = 0;
    for (w = 0; w < len; w++) {
      next = next * 10 + s[w];
      s[w] = next / size_loc_t;
      if (s[w]) {
        b = 1;
      }
      next %= size_loc_t;
    }
    a[t] = next;
    t++;
    if (t == max_size) {
      upd_max_size(max_size * 2);
    }
  }
  for (w = t; w < max_size; w++) {
    a[w] = 0;
  }
  real_size = t;
  delete s;
  upd_real_size();
}

big_integer::~big_integer() {
  delete []a;
}



big_integer& big_integer::operator+=(big_integer const& v) {
  if (inv * v.inv == 1) {
    add(v);
  } else {
    if (abs_cmp(v) < 0) {
      big_integer buff((*this));
      (*this) = v;
      sub(buff);
    } else {
      sub(v);
    }
  }
  return *this;
}
big_integer& big_integer::operator-=(big_integer const& v) {
  if (inv * v.inv == -1) {
    add(v);
  } else {
    if (abs_cmp(v) < 0) {
      big_integer buff(*this);
      (*this) = v;
      sub(buff);
      inv *= -1;
    } else {
      sub(v);
    }
  }
  return *this;
}
  
big_integer& big_integer::operator*=(big_integer const& v) {
  inv *= v.inv;
  int new_max_size = max_size;
  while (new_max_size < real_size + v.real_size) {
    new_max_size <<= 1;
  }
  loc_t *s = new loc_t[new_max_size];
  int w, e, r;
  for (w = 0; w < new_max_size; w++) {
    s[w] = 0;
  }
  buf_t next;
  for (e = 0; e < real_size; e++) {
    for (r = 0; r < v.real_size; r++) {
      next = ((buf_t)a[e]) * v.a[r] + s[e + r];
      s[e + r] = next & (size_loc_t - 1);
      next >>= pw_loc_t;
      if (next) {
        w = e + r + 1;
        s[w] += next;
        while (s[w] >= size_loc_t) {
          s[w] -= size_loc_t;
          w++;
          s[w]++;
        }
      }
    }
  }
  
  delete []a;
  a = s;
  max_size = new_max_size;
  real_size += v.real_size;
  upd_real_size();
  return *this;
}
big_integer& big_integer::operator/=(big_integer const& divider1) {
  if (*this == 0) {
    return *this;
  }
  if (abs_cmp(divider1) < 0) {
    *this = 0;
    return *this;
  }
  big_integer divider(divider1);
  int new_inv = inv * divider.inv;
  inv = divider.inv = 1;
  
  int w, sz = divider.real_size;
  int new_max_size = start_max_size;
  
  while (new_max_size < real_size - sz + 1) {
    new_max_size <<= 1;
  }
  
  loc_t *s = new loc_t[new_max_size];
  for (w = 0; w < new_max_size; w++) {
    s[w] = 0;
  }
  int old_real_size = real_size;
  
  divider <<= pw_loc_t * (real_size - sz);
  buf_t next, c, v;
  
  int e;
  double c1;
  
  for (w = real_size - 1; w >= sz - 1; w--) {
    c1 = ((buf_t)a[w]) * size_loc_t;
    v = ((buf_t)divider.a[w]) * size_loc_t;
    
    if (w > 0) {
      c1 += a[w - 1];
      v += divider.a[w - 1];
    }
    if (w + 1 < max_size) {
      c1 += ((double)a[w + 1]) * size_loc_t * size_loc_t;
    }
    
    c1 /= v + 1;
    c = (loc_t)(c1 - 1e-10);
    
    s[w - sz + 1] = c;
    next = 0;
    
    for (e = w - sz + 1; e <= w; e++) {
      next += a[e] - divider.a[e] * c;
      v = next % size_loc_t;
      next /= size_loc_t;
      if (v < 0) {
        v += size_loc_t;
        next--;
      }
      a[e] = v;
    }
    if (next) {
      a[w + 1] += next;
    }
    
    while ((real_size > 1) && (a[real_size - 1] == 0)) {
      real_size--;
    }
    
    while (cmp(divider) >= 0) {
      s[w - sz + 1]++;
      next = 0;
      for (e = w - sz + 1; e <= w; e++) {
        next += (buf_t)a[e] - divider.a[e];
        v = next;
        next = 0;
        while (v < 0) {
          v += size_loc_t;
          next--;
        }
        a[e] = v;
      }
      if (next) {
        a[w + 1] += next;
      }
      while ((real_size > 1) && (a[real_size - 1] == 0)) {
        real_size--;
      }
    }
    for (e = std::max(w - sz, 0); e < w; e++) {
      divider.a[e] = divider.a[e + 1];
    }
    divider.a[w] = 0;
    divider.real_size--;
  }
  delete a;
  a = s;
  max_size = new_max_size;
  inv = new_inv;
  
  real_size = old_real_size - sz + 1;
  upd_real_size();
  return *this;
}
big_integer& big_integer::operator%=(big_integer const& v) {
  big_integer buf = *this;
  buf /= v;
  buf *= v;
  *this -= buf;
  return *this;
}

big_integer& big_integer::operator=(big_integer const& v) {
  if (max_size != v.max_size) {
    delete a;
    max_size = v.max_size;
    a = new loc_t[max_size];
  }
  real_size = v.real_size;
  inv = v.inv;
  for (int w = 0; w < max_size; w++) {
    a[w] = v.a[w];
  }
  return *this;
}

big_integer& big_integer::operator++() {
  if (real_size == max_size) {
    upd_max_size(max_size * 2);
  }
  int w, overflow = inv;
  buf_t next;
  for (w = 0; w < (real_size) && (overflow); w++) {
    next = overflow + a[w];
    overflow = 0;
    if (next < 0) {
      next += size_loc_t;
      overflow = -1;
    }
    if (next >= size_loc_t) {
      next -= size_loc_t;
      overflow = 1;
    }
    a[w] = next;
  }
  upd_real_size();
  return *this;
}
big_integer big_integer::operator++(int) {
  big_integer res = *this;
  ++(*this);
  return res;
}
big_integer& big_integer::operator--() {
  if (real_size == max_size) {
    upd_max_size(max_size * 2);
  }
  int w, overflow = -inv;
  buf_t next;
  for (w = 0; w < (real_size) && (overflow); w++) {
    next = overflow + a[w];
    overflow = 0;
    if (next < 0) {
      next += size_loc_t;
      overflow = -1;
    }
    if (next >= size_loc_t) {
      next -= size_loc_t;
      overflow = 1;
    }
    a[w] = next;
  }
  upd_real_size();
  if (overflow != 0) {
    (*this) = -1;
  }
  return *this;
}
big_integer big_integer::operator--(int) {
  big_integer res = *this;
  --(*this);
  return res;
}

big_integer& big_integer::operator&=(big_integer const& v) {
  int w;
  if (max_size < v.max_size) {
    upd_max_size(v.max_size);
  }
  if (inv < 0) {
    ++(*this);
    for (w = 0; w < max_size; w++) {
      a[w] ^= (loc_t)(size_loc_t - 1);
    }
    inv = -1;
  }
  
  buf_t next = 0;
  int overflow = 1;
  for (w = 0; w < v.max_size; w++) {
    if (v.inv > 0) {
      a[w] &= v.a[w];
    } else {
      next = (v.a[w] ^ (loc_t)(size_loc_t - 1)) + overflow;
      overflow = 0;
      if (next == size_loc_t) {
        next -= size_loc_t;
        overflow = 1;
      }
      a[w] &= next;
    }
  }
  for (; w < max_size; w++) {
    if (v.inv > 0) {
      a[w] &= 0;
    } else {
      next = (0 ^ (loc_t)(size_loc_t - 1)) + overflow;
      overflow = 0;
      if (next == size_loc_t) {
        next -= size_loc_t;
        overflow = 1;
      }
      a[w] &= next;
    }
  }
  
  if ((inv < 0) && (v.inv < 0)) {
    for (w = 0; w < max_size; w++) {
      a[w] ^= (loc_t)(size_loc_t - 1);
    }
    --(*this);
    inv = -1;
  } else {
    inv = 1;
  }
  real_size = max_size;
  upd_real_size();
  return *this;
}
big_integer& big_integer::operator|=(big_integer const& v) {
  int w;
  if (max_size < v.max_size) {
    upd_max_size(v.max_size);
  }
  if (inv < 0) {
    inv = -1;
    ++(*this);
    for (w = 0; w < max_size; w++) {
      a[w] ^= (loc_t)(size_loc_t - 1);
    }
  }
  
  buf_t next = 0;
  int overflow = 1;
  for (w = 0; w < v.max_size; w++) {
    if (v.inv > 0) {
      a[w] |= v.a[w];
    } else {
      next = (v.a[w] ^ (loc_t)(size_loc_t - 1)) + overflow;
      overflow = 0;
      if (next == size_loc_t) {
        next -= size_loc_t;
        overflow = 1;
      }
      a[w] |= next;
    }
  }
  for (; w < max_size; w++) {
    if (v.inv > 0) {
      a[w] |= 0;
    } else {
      next = (0 ^ (loc_t)(size_loc_t - 1)) + overflow;
      overflow = 0;
      if (next == size_loc_t) {
        next -= size_loc_t;
        overflow = 1;
      }
      a[w] |= next;
    }
  }
  
  if ((inv < 0) || (v.inv < 0)) {
    inv = -1;
    for (w = 0; w < max_size; w++) {
      a[w] ^= (loc_t)(size_loc_t - 1);
    }
    --(*this);
  }
  real_size = max_size;
  upd_real_size();
  return *this;
}
big_integer& big_integer::operator^=(big_integer const& v) {
  int w;
  if (max_size < v.max_size) {
    upd_max_size(v.max_size);
  }
  if (inv < 0) {
    ++(*this);
    inv = -1;
    for (w = 0; w < max_size; w++) {
      a[w] ^= (loc_t)(size_loc_t - 1);
    }
  }
  
  buf_t next = 0;
  int overflow = 1;
  for (w = 0; w < v.max_size; w++) {
    if (v.inv > 0) {
      a[w] ^= v.a[w];
    } else {
      next = (v.a[w] ^ (loc_t)(size_loc_t - 1)) + overflow;
      overflow = 0;
      if (next == size_loc_t) {
        next -= size_loc_t;
        overflow = 1;
      }
      a[w] ^= next;
    }
  }
  for (; w < max_size; w++) {
    if (v.inv > 0) {
      a[w] ^= 0;
    } else {
      next = (0 ^ (loc_t)(size_loc_t - 1)) + overflow;
      overflow = 0;
      if (next == size_loc_t) {
        next -= size_loc_t;
        overflow = 1;
      }
      a[w] ^= next;
    }
  }
  
  if ((inv < 0) ^ (v.inv < 0)) {
    inv = -1;
    for (w = 0; w < max_size; w++) {
      a[w] ^= (loc_t)(size_loc_t - 1);
    }
    --(*this);
  } else {
    inv = 1;
  }
  real_size = max_size;
  upd_real_size();
  return *this;
}

big_integer& big_integer::operator<<=(int shift) {
  if (shift < 0) {
    right_shift(-shift);
  } else {
    left_shift(shift);
  }
  return *this;
}
big_integer& big_integer::operator>>=(int shift) {
  if (shift < 0) {
    left_shift(-shift);
  } else {
    right_shift(shift);
  }
  return *this;
}

big_integer big_integer::operator+() const {
  big_integer rez = (*this);
  return rez;
}
big_integer big_integer::operator-() const {
  big_integer rez = (*this);
  rez.inv *= -1;
  rez.upd_real_size();
  return rez;
}
big_integer big_integer::operator~() const {
  big_integer res = (*this);
  ++res;
  res.inv *= -1;
  res.upd_real_size();
  return res;
}

bool operator==(big_integer const& a, big_integer const& s) {
  return (a.cmp(s) == 0);
}
bool operator!=(big_integer const& a, big_integer const& s) {
  return (a.cmp(s) != 0);
}
bool operator< (big_integer const& a, big_integer const& s) {
  return (a.cmp(s) < 0);
}
bool operator> (big_integer const& a, big_integer const& s) {
  return (a.cmp(s) > 0);
}
bool operator<=(big_integer const& a, big_integer const& s) {
  return (a.cmp(s) <= 0);
}
bool operator>=(big_integer const& a, big_integer const& s) {
  return (a.cmp(s) >= 0);
}



big_integer operator+(big_integer c, big_integer const& v) {
  return c += v;
}
big_integer operator-(big_integer c, big_integer const& v) {
  return c -= v;
}
big_integer operator*(big_integer c, big_integer const& v) {
  return c *= v;
}
big_integer operator/(big_integer c, big_integer const& v) {
  return c /= v;
}
big_integer operator%(big_integer c, big_integer const& v) {
  return c %= v;
}

big_integer operator&(big_integer c, big_integer const& v) {
  return c &= v;
}
big_integer operator|(big_integer c, big_integer const& v) {
  return c |= v;
}
big_integer operator^(big_integer c, big_integer const& v) {
  return c ^= v;
}

big_integer operator<<(big_integer c, int shift) {
  return (c <<= shift);
}
big_integer operator>>(big_integer c, int shift) {
  return (c >>= shift);
}

std::string to_string(big_integer const& a) {
  int w = a.real_size - 1, e, r, t = 0;
  while ((w > 0) && (a.a[w] == 0)) {
    w--;
  }
  big_integer::loc_t *f = new big_integer::loc_t[w + 1];
  int size_res = 2;
  for (long long c = big_integer::size_loc_t; c; c /= 10) {
    size_res += w + 1;
  }
  char *res = new char[size_res];
  for (e = 0; e <= w; e++) {
    f[e] = a.a[e];
  }
  bool b = 1;
  big_integer::buf_t next;
  while (b) {
    b = 0;
    next = 0;
    for (e = w; e >= 0; e--) {
      next = next * big_integer::size_loc_t + f[e];
      f[e] = next / 10;
      next %= 10;
      if (f[e]) {
        b = 1;
      }
    }
    res[t] = 48 + next;
    t++;
    while ((w > 0) && (!f[w])) {
      w--;
    }
  }
  if (a.inv < 0) {
    res[t] = '-';
    t++;
  }
  res[t] = 0;
  for (e = 0, r = t - 1; e < r; e++, r--) {
    std::swap(res[e], res[r]);
  }
  std::string *s = new std::string(res);
  delete res;
  return *s;
}
std::ostream& operator<<(std::ostream& out, big_integer const& c) {
   out << to_string(c);
   return out;
}

