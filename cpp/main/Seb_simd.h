// Synopsis: Small vector kernels with optional SIMD acceleration.

#ifndef SEB_SIMD_H
#define SEB_SIMD_H

#include <cstddef>
#include "Seb_point.h"

#if defined(__SSE2__) || defined(_M_X64) || \
    (defined(_M_IX86_FP) && _M_IX86_FP >= 2)
#define SEB_HAS_SSE2 1
#include <emmintrin.h>
#if defined(_MSC_VER)
#include <intrin.h>
#endif
#else
#define SEB_HAS_SSE2 0
#endif

namespace SEB_NAMESPACE {
namespace detail {

  template<typename Float>
  inline Float local_sqr(const Float x)
  {
    return x * x;
  }

  inline bool simd_available()
  {
#if SEB_HAS_SSE2
    // SSE2 is part of the x86-64 ABI.
#if defined(__x86_64__) || defined(_M_X64)
    return true;
#elif defined(_MSC_VER)
    int info[4];
    __cpuid(info, 1);
    return (info[3] & (1 << 26)) != 0;
#elif defined(__GNUC__) || defined(__clang__)
    return __builtin_cpu_supports("sse2");
#else
    return true;
#endif
#else
    return false;
#endif
  }

  template<typename Float>
  inline bool simd_available_for()
  {
    return false;
  }

  template<>
  inline bool simd_available_for<double>()
  {
    return simd_available();
  }

  template<typename Float, typename T>
  inline const Float* contiguous_data(const T&)
  {
    return 0;
  }

  template<typename Float>
  inline const Float* contiguous_data(const Float* p)
  {
    return p;
  }

  template<typename Float>
  inline const Float* contiguous_data(Float* p)
  {
    return p;
  }

  template<typename Float>
  inline const Float* contiguous_data(const Point<Float>& p)
  {
    return p.data();
  }

  template<typename Float>
  struct VectorOps {
    static Float dot(const Float* a, const Float* b, std::size_t n, bool)
    {
      Float sum = 0;
      for (std::size_t i = 0; i < n; ++i)
        sum += a[i] * b[i];
      return sum;
    }

    static Float squared_norm(const Float* a, std::size_t n, bool use_simd)
    {
      return dot(a, a, n, use_simd);
    }

    static Float squared_distance(const Float* a, const Float* b,
                                  std::size_t n, bool)
    {
      Float sum = 0;
      for (std::size_t i = 0; i < n; ++i)
        sum += local_sqr(a[i] - b[i]);
      return sum;
    }

    static void assign_difference(Float* out, const Float* a, const Float* b,
                                  std::size_t n, bool)
    {
      for (std::size_t i = 0; i < n; ++i)
        out[i] = a[i] - b[i];
    }

    static void axpy_inplace(Float* y, Float alpha, const Float* x,
                             std::size_t n, bool)
    {
      for (std::size_t i = 0; i < n; ++i)
        y[i] += alpha * x[i];
    }

    static Float subtract_inplace_and_squared_norm(Float* y, const Float* x,
                                                   std::size_t n, bool)
    {
      Float sum = 0;
      for (std::size_t i = 0; i < n; ++i) {
        y[i] -= x[i];
        sum += local_sqr(y[i]);
      }
      return sum;
    }

    static void blend(Float* out, const Float* a, const Float* b, Float alpha,
                      std::size_t n, bool)
    {
      for (std::size_t i = 0; i < n; ++i)
        out[i] = a[i] + alpha * (b[i] - a[i]);
    }

    static void rotate_pair(Float* a, Float* b, Float c, Float s,
                            std::size_t n, bool)
    {
      for (std::size_t i = 0; i < n; ++i) {
        const Float ai = a[i];
        const Float bi = b[i];
        a[i] = c * ai + s * bi;
        b[i] = c * bi - s * ai;
      }
    }
  };

  template<>
  struct VectorOps<double> {
    static double dot(const double* a, const double* b, std::size_t n,
                      bool use_simd)
    {
#if SEB_HAS_SSE2
      if (use_simd && n >= 2) {
        std::size_t i = 0;
        __m128d acc = _mm_setzero_pd();
        for (; i + 1 < n; i += 2)
          acc = _mm_add_pd(acc, _mm_mul_pd(_mm_loadu_pd(a + i),
                                           _mm_loadu_pd(b + i)));
        double tmp[2];
        _mm_storeu_pd(tmp, acc);
        double sum = tmp[0] + tmp[1];
        for (; i < n; ++i)
          sum += a[i] * b[i];
        return sum;
      }
#endif
      double sum = 0;
      for (std::size_t i = 0; i < n; ++i)
        sum += a[i] * b[i];
      return sum;
    }

    static double squared_norm(const double* a, std::size_t n, bool use_simd)
    {
      return dot(a, a, n, use_simd);
    }

    static double squared_distance(const double* a, const double* b,
                                   std::size_t n, bool use_simd)
    {
#if SEB_HAS_SSE2
      if (use_simd && n >= 2) {
        std::size_t i = 0;
        __m128d acc = _mm_setzero_pd();
        for (; i + 1 < n; i += 2) {
          const __m128d diff = _mm_sub_pd(_mm_loadu_pd(a + i),
                                          _mm_loadu_pd(b + i));
          acc = _mm_add_pd(acc, _mm_mul_pd(diff, diff));
        }
        double tmp[2];
        _mm_storeu_pd(tmp, acc);
        double sum = tmp[0] + tmp[1];
        for (; i < n; ++i)
          sum += local_sqr(a[i] - b[i]);
        return sum;
      }
#endif
      double sum = 0;
      for (std::size_t i = 0; i < n; ++i)
        sum += local_sqr(a[i] - b[i]);
      return sum;
    }

    static void assign_difference(double* out, const double* a,
                                  const double* b, std::size_t n,
                                  bool use_simd)
    {
#if SEB_HAS_SSE2
      if (use_simd && n >= 2) {
        std::size_t i = 0;
        for (; i + 1 < n; i += 2)
          _mm_storeu_pd(out + i, _mm_sub_pd(_mm_loadu_pd(a + i),
                                            _mm_loadu_pd(b + i)));
        for (; i < n; ++i)
          out[i] = a[i] - b[i];
        return;
      }
#endif
      for (std::size_t i = 0; i < n; ++i)
        out[i] = a[i] - b[i];
    }

    static void axpy_inplace(double* y, double alpha, const double* x,
                             std::size_t n, bool use_simd)
    {
#if SEB_HAS_SSE2
      if (use_simd && n >= 2) {
        std::size_t i = 0;
        const __m128d alpha_v = _mm_set1_pd(alpha);
        for (; i + 1 < n; i += 2)
          _mm_storeu_pd(y + i,
                        _mm_add_pd(_mm_loadu_pd(y + i),
                                   _mm_mul_pd(alpha_v, _mm_loadu_pd(x + i))));
        for (; i < n; ++i)
          y[i] += alpha * x[i];
        return;
      }
#endif
      for (std::size_t i = 0; i < n; ++i)
        y[i] += alpha * x[i];
    }

    static double subtract_inplace_and_squared_norm(double* y, const double* x,
                                                    std::size_t n,
                                                    bool use_simd)
    {
#if SEB_HAS_SSE2
      if (use_simd && n >= 2) {
        std::size_t i = 0;
        __m128d acc = _mm_setzero_pd();
        for (; i + 1 < n; i += 2) {
          const __m128d diff = _mm_sub_pd(_mm_loadu_pd(y + i),
                                          _mm_loadu_pd(x + i));
          _mm_storeu_pd(y + i, diff);
          acc = _mm_add_pd(acc, _mm_mul_pd(diff, diff));
        }
        double tmp[2];
        _mm_storeu_pd(tmp, acc);
        double sum = tmp[0] + tmp[1];
        for (; i < n; ++i) {
          y[i] -= x[i];
          sum += local_sqr(y[i]);
        }
        return sum;
      }
#endif
      double sum = 0;
      for (std::size_t i = 0; i < n; ++i) {
        y[i] -= x[i];
        sum += local_sqr(y[i]);
      }
      return sum;
    }

    static void blend(double* out, const double* a, const double* b,
                      double alpha, std::size_t n, bool use_simd)
    {
#if SEB_HAS_SSE2
      if (use_simd && n >= 2) {
        std::size_t i = 0;
        const __m128d alpha_v = _mm_set1_pd(alpha);
        for (; i + 1 < n; i += 2) {
          const __m128d av = _mm_loadu_pd(a + i);
          const __m128d bv = _mm_loadu_pd(b + i);
          _mm_storeu_pd(out + i,
                        _mm_add_pd(av, _mm_mul_pd(alpha_v,
                                                  _mm_sub_pd(bv, av))));
        }
        for (; i < n; ++i)
          out[i] = a[i] + alpha * (b[i] - a[i]);
        return;
      }
#endif
      for (std::size_t i = 0; i < n; ++i)
        out[i] = a[i] + alpha * (b[i] - a[i]);
    }

    static void rotate_pair(double* a, double* b, double c, double s,
                            std::size_t n, bool use_simd)
    {
#if SEB_HAS_SSE2
      if (use_simd && n >= 2) {
        std::size_t i = 0;
        const __m128d c_v = _mm_set1_pd(c);
        const __m128d s_v = _mm_set1_pd(s);
        for (; i + 1 < n; i += 2) {
          const __m128d av = _mm_loadu_pd(a + i);
          const __m128d bv = _mm_loadu_pd(b + i);
          _mm_storeu_pd(a + i,
                        _mm_add_pd(_mm_mul_pd(c_v, av),
                                   _mm_mul_pd(s_v, bv)));
          _mm_storeu_pd(b + i,
                        _mm_sub_pd(_mm_mul_pd(c_v, bv),
                                   _mm_mul_pd(s_v, av)));
        }
        for (; i < n; ++i) {
          const double ai = a[i];
          const double bi = b[i];
          a[i] = c * ai + s * bi;
          b[i] = c * bi - s * ai;
        }
        return;
      }
#endif
      for (std::size_t i = 0; i < n; ++i) {
        const double ai = a[i];
        const double bi = b[i];
        a[i] = c * ai + s * bi;
        b[i] = c * bi - s * ai;
      }
    }
  };

  template<typename Float, typename A, typename B>
  inline Float dot(const A& a, const B& b, std::size_t n, bool use_simd)
  {
    const Float* ap = contiguous_data<Float>(a);
    const Float* bp = contiguous_data<Float>(b);
    if (ap != 0 && bp != 0)
      return VectorOps<Float>::dot(ap, bp, n,
                                   use_simd && simd_available_for<Float>());

    Float sum = 0;
    for (std::size_t i = 0; i < n; ++i)
      sum += a[i] * b[i];
    return sum;
  }

  template<typename Float, typename A>
  inline Float squared_norm(const A& a, std::size_t n, bool use_simd)
  {
    const Float* ap = contiguous_data<Float>(a);
    if (ap != 0)
      return VectorOps<Float>::squared_norm(
          ap, n, use_simd && simd_available_for<Float>());

    Float sum = 0;
    for (std::size_t i = 0; i < n; ++i)
      sum += local_sqr(a[i]);
    return sum;
  }

  template<typename Float, typename A, typename B>
  inline Float squared_distance(const A& a, const B& b, std::size_t n,
                                bool use_simd)
  {
    const Float* ap = contiguous_data<Float>(a);
    const Float* bp = contiguous_data<Float>(b);
    if (ap != 0 && bp != 0)
      return VectorOps<Float>::squared_distance(ap, bp, n,
                                                use_simd &&
                                                simd_available_for<Float>());

    Float sum = 0;
    for (std::size_t i = 0; i < n; ++i)
      sum += local_sqr(a[i] - b[i]);
    return sum;
  }

  template<typename Float, typename A, typename B>
  inline void assign_difference(Float* out, const A& a, const B& b,
                                std::size_t n, bool use_simd)
  {
    const Float* ap = contiguous_data<Float>(a);
    const Float* bp = contiguous_data<Float>(b);
    if (ap != 0 && bp != 0) {
      VectorOps<Float>::assign_difference(out, ap, bp, n,
                                          use_simd &&
                                          simd_available_for<Float>());
      return;
    }

    for (std::size_t i = 0; i < n; ++i)
      out[i] = a[i] - b[i];
  }

  template<typename Float>
  inline void axpy_inplace(Float* y, Float alpha, const Float* x,
                           std::size_t n, bool use_simd)
  {
    VectorOps<Float>::axpy_inplace(y, alpha, x, n,
                                   use_simd && simd_available_for<Float>());
  }

  template<typename Float>
  inline Float subtract_inplace_and_squared_norm(Float* y, const Float* x,
                                                 std::size_t n, bool use_simd)
  {
    return VectorOps<Float>::subtract_inplace_and_squared_norm(
        y, x, n, use_simd && simd_available_for<Float>());
  }

  template<typename Float>
  inline void blend(Float* out, const Float* a, const Float* b, Float alpha,
                    std::size_t n, bool use_simd)
  {
    VectorOps<Float>::blend(out, a, b, alpha, n,
                            use_simd && simd_available_for<Float>());
  }

  template<typename Float, typename B>
  inline void blend_from(Float* out, const Float* a, const B& b, Float alpha,
                         std::size_t n, bool use_simd)
  {
    const Float* bp = contiguous_data<Float>(b);
    if (bp != 0) {
      blend(out, a, bp, alpha, n, use_simd);
      return;
    }

    for (std::size_t i = 0; i < n; ++i)
      out[i] = a[i] + alpha * (b[i] - a[i]);
  }

  template<typename Float>
  inline void rotate_pair(Float* a, Float* b, Float c, Float s,
                          std::size_t n, bool use_simd)
  {
    VectorOps<Float>::rotate_pair(a, b, c, s, n,
                                  use_simd && simd_available_for<Float>());
  }

} // namespace detail
} // namespace SEB_NAMESPACE

#endif // SEB_SIMD_H
