#ifndef STAN_MATH_PRIM_SCAL_META_OPERANDSANDPARTIALSMV_HPP
#define STAN_MATH_PRIM_SCAL_META_OPERANDSANDPARTIALSMV_HPP

#include <stan/math/rev/core/precomputed_gradients.hpp>
#include <stan/math/prim/scal/meta/is_vector.hpp>
#include <stan/math/prim/mat/meta/is_vector.hpp>
#include <stan/math/prim/arr/meta/is_vector.hpp>
#include <stan/math/prim/arr/meta/length.hpp>
#include <stan/math/prim/mat/meta/length.hpp>
#include <stan/math/prim/scal/meta/length.hpp>
#include <stan/math/prim/scal/meta/length_mvt.hpp>
#include <stan/math/prim/mat/meta/is_vector_like.hpp>
#include <stan/math/prim/scal/meta/is_vector_like.hpp>
#include <stan/math/prim/scal/meta/VectorViewMat.hpp>
#include <stan/math/prim/scal/meta/is_constant_struct.hpp>
#include <stan/math/prim/scal/meta/contains_fvar.hpp>
#include <stan/math/prim/scal/meta/partials_return_type.hpp>
#include <stan/math/prim/scal/meta/partials_type.hpp>
#include <stan/math/prim/scal/meta/return_type.hpp>
#include <stan/math/rev/core.hpp>
#include <stan/math/rev/scal/meta/is_var.hpp>
#include <stan/math/rev/scal/meta/partials_type.hpp>
#include <stan/math/prim/scal/meta/contains_matrix.hpp>
#include <stan/math/prim/mat/meta/contains_matrix.hpp>

namespace stan {
  namespace agrad {

      template<typename T_return_type, typename T_partials_return,
               typename T1, typename T2, typename T3, typename T4,
               typename T5, typename T6,
               bool is_const = stan::is_constant_struct<T_return_type>::value>
      struct partials_to_var_mvt {
        inline
        T_return_type to_var(double logp, size_t /* nvaris */,
                             agrad::vari** /* all_varis */,
                             T_partials_return* /* all_partials */) {
          return logp;
        }
      };

      template<typename T_return_type, typename T_partials_return,
               typename T1, typename T2, typename T3, typename T4,
               typename T5, typename T6>
      struct partials_to_var_mvt<T_return_type, T_partials_return,
                             T1, T2, T3, T4, T5, T6,
                             false> {
        inline T_return_type to_var(T_partials_return logp, size_t nvaris,
                                    agrad::vari** all_varis,
                                    T_partials_return* all_partials) {
          return precomputed_gradients(logp, nvaris, all_varis,
                                              all_partials);
        }
      };

      template<typename T,
               bool is_vec = is_vector<T>::value,
               bool contains_mat = contains_matrix<T>::value,
               bool is_const = is_constant_struct<T>::value>
      struct set_varis_mvt {
        inline size_t set(agrad::vari** /*varis*/, const T& /*x*/) {
          return 0U;
        }
      };
      template<typename T>
      struct set_varis_mvt<T, true, false, false> {
        inline size_t set(agrad::vari** varis, const T& x) {
          for (size_t n = 0; n < length(x); ++n)
            varis[n] = x[n].vi_;
          return length(x);
        }
      };
      template<typename T>
      struct set_varis_mvt<T, true, true, false> {
        inline size_t set(agrad::vari** varis, const T& x) {
          size_t n = 0;
          for (size_t i = 0; i < length_mvt(x); ++i)
            for (int j = 0; j < length(x[i]); ++j) {
              varis[n] = x[i](j).vi_;
              ++n;
            }
          return n;
        }
      };
      template<typename T>
      struct set_varis_mvt<T, false, true, false> {
        inline size_t set(agrad::vari** varis, const T& x) {
          for (size_t n = 0; n < length(x); ++n)
            varis[n] = x(n).vi_;
          return length(x);
        }
      };
      template<>
      struct set_varis_mvt<agrad::var, false, false, false> {
        inline size_t set(agrad::vari** varis, const agrad::var& x) {
          varis[0] = x.vi_;
          return (1);
        }
      };
    

    /**
     * A variable implementation that stores operands and
     * derivatives with respect to the variable.
     */
    template<typename T1 = double, typename T2 = double, typename T3 = double,
             typename T4 = double, typename T5 = double, typename T6 = double>
    struct OperandsAndPartials {
      typedef
      typename stan::partials_return_type<T1, T2, T3, T4, T5, T6>::type
      T_partials_return;

      typedef
      typename stan::return_type<T1, T2, T3, T4, T5, T6>::type T_return_type;

      static const bool all_constant = is_constant<T_return_type>::value;
      size_t nvaris;
      agrad::vari** all_varis;
      T_partials_return* all_partials;

      VectorViewMat<T_partials_return,
                 is_vector_like<T1>::value,
                 is_constant_struct<T1>::value> d_x1;
      VectorViewMat<T_partials_return,
                 is_vector_like<T2>::value,
                 is_constant_struct<T2>::value> d_x2;
      VectorViewMat<T_partials_return,
                 is_vector_like<T3>::value,
                 is_constant_struct<T3>::value> d_x3;
      VectorViewMat<T_partials_return,
                 is_vector_like<T4>::value,
                 is_constant_struct<T4>::value> d_x4;
      VectorViewMat<T_partials_return,
                 is_vector_like<T5>::value,
                 is_constant_struct<T5>::value> d_x5;
      VectorViewMat<T_partials_return,
                 is_vector_like<T6>::value,
                 is_constant_struct<T6>::value> d_x6;

      OperandsAndPartials(const T1& x1 = 0, const T2& x2 = 0, const T3& x3 = 0,
                          const T4& x4 = 0, const T5& x5 = 0, const T6& x6 = 0)
        : nvaris(!is_constant_struct<T1>::value * length(x1) +
                 !is_constant_struct<T2>::value * length(x2) +
                 !is_constant_struct<T3>::value * length(x3) +
                 !is_constant_struct<T4>::value * length(x4) +
                 !is_constant_struct<T5>::value * length(x5) +
                 !is_constant_struct<T6>::value * length(x6)),
          all_varis(static_cast<agrad::vari**>
                    (agrad::chainable::operator new
                     (sizeof(agrad::vari*) * nvaris))),
          all_partials(static_cast<T_partials_return*>
                       (agrad::chainable::operator new
                        (sizeof(T_partials_return) * nvaris))),
          d_x1(x1, all_partials),
          d_x2(x2, all_partials
               + (!is_constant_struct<T1>::value) * length(x1)),
          d_x3(x3, all_partials
               + (!is_constant_struct<T1>::value) * length(x1)
               + (!is_constant_struct<T2>::value) * length(x2)),
          d_x4(x4, all_partials
               + (!is_constant_struct<T1>::value) * length(x1)
               + (!is_constant_struct<T2>::value) * length(x2)
               + (!is_constant_struct<T3>::value) * length(x3)),
          d_x5(x5, all_partials
               + (!is_constant_struct<T1>::value) * length(x1)
               + (!is_constant_struct<T2>::value) * length(x2)
               + (!is_constant_struct<T3>::value) * length(x3)
               + (!is_constant_struct<T4>::value) * length(x4)),
          d_x6(x6, all_partials
               + (!is_constant_struct<T1>::value) * length(x1)
               + (!is_constant_struct<T2>::value) * length(x2)
               + (!is_constant_struct<T3>::value) * length(x3)
               + (!is_constant_struct<T4>::value) * length(x4)
               + (!is_constant_struct<T5>::value) * length(x5)) {
        size_t base = 0;
        if (!is_constant_struct<T1>::value)
          base += set_varis_mvt<T1>().set(&all_varis[base], x1);
        if (!is_constant_struct<T2>::value)
          base += set_varis_mvt<T2>().set(&all_varis[base], x2);
        if (!is_constant_struct<T3>::value)
          base += set_varis_mvt<T3>().set(&all_varis[base], x3);
        if (!is_constant_struct<T4>::value)
          base += set_varis_mvt<T4>().set(&all_varis[base], x4);
        if (!is_constant_struct<T5>::value)
          base += set_varis_mvt<T5>().set(&all_varis[base], x5);
        if (!is_constant_struct<T6>::value)
          set_varis_mvt<T6>().set(&all_varis[base], x6);
        std::fill(all_partials, all_partials+nvaris, 0);
      }

      T_return_type
      to_var(T_partials_return logp,
             const T1& x1 = 0, const T2& x2 = 0, const T3& x3 = 0,
             const T4& x4 = 0, const T5& x5 = 0, const T6& x6 = 0) {
        return partials_to_var_mvt
          <T_return_type, T_partials_return, T1,
           T2, T3, T4, T5, T6>().to_var(logp, nvaris, all_varis,
                                        all_partials);
      }
    };


  }
}


#endif