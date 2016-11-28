/*!
 *  Copyright (c) 2016 by Contributors
 * \file operation.h
 * \brief Operation node can generate one or multiple Tensors
 */
#ifndef TVM_OPERATION_H_
#define TVM_OPERATION_H_

#include <string>
#include "./expr.h"
#include "./domain.h"
#include "./tensor.h"

namespace tvm {

/*!
 * \brief A Compute op that compute a tensor on certain domain.
 */
class ComputeOpNode : public OperationNode {
 public:
  /*! \brief the compute expression */
  Expr body;
  /*! \brief constructor */
  ComputeOpNode() {}

  const char* type_key() const final {
    return "ComputeOp";
  }
  size_t num_outputs() const final {
    return 1;
  }
  std::string output_name(size_t i) const final;
  Type output_dtype(size_t i) const final;
  Array<Expr> output_shape(size_t i) const final;
  void VisitAttrs(AttrVisitor* v) final {
    v->Visit("domain", &domain);
    v->Visit("name", &name);
    v->Visit("dim_var", &dim_var);
    v->Visit("body", &body);
  }
  static Operation make(Domain domain,
                        std::string name,
                        Array<Var> dim_var,
                        Expr body);
};


/*! \brief The compute function to specify the input source of a Tensor */
using FCompute = std::function<Expr (const Array<Var>& i)>;

/*!
 * \brief Construct a new tensor by computing over shape,
 *  using the computation rule: result_tensor[axis] = fcompute(axis)
 * \param shape Shape of the tensor.
 * \param fcompute The compute function to create the tensor.
 * \param name The optional name of the tensor.
 */
Tensor Compute(Array<Expr> shape, FCompute fcompute, std::string name = "tensor");

// same as compute, specialized for different fcompute function
inline Tensor Compute(Array<Expr> shape,
                      std::function<Expr(Var)> f,
                      std::string name = "tensor") {
  FCompute fc = [f] (const Array<Var>& i) { return f(i[0]); };
  return Compute(shape, fc, name);
}
inline Tensor Compute(Array<Expr> shape,
                      std::function<Expr(Var, Var)> f,
                      std::string name = "tensor") {
  FCompute fc = [f] (const Array<Var>& i) { return f(i[0], i[1]); };
  return Compute(shape, fc, name);
}
inline Tensor Compute(Array<Expr> shape,
                      std::function<Expr(Var, Var, Var)> f,
                      std::string name = "tensor") {
  FCompute fc = [f] (const Array<Var>& i) { return f(i[0], i[1], i[2]); };
  return  Compute(shape, fc, name);
}
inline Tensor Compute(Array<Expr> shape,
                      std::function<Expr(Var, Var, Var, Var)> f,
                      std::string name = "tensor") {
  FCompute fc = [f] (const Array<Var>& i) { return f(i[0], i[1], i[2], i[3]); };
  return Compute(shape, fc, name);
}

}  // namespace tvm

#endif  // TVM_OPERATION_H_
