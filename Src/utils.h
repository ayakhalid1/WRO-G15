#ifndef UTILS_H
#define UTILS_H

template<typename T>
T clampT(T v, T lo, T hi) {
  if (v < lo) return lo;
  if (v > hi) return hi;
  return v;
}

#endif
