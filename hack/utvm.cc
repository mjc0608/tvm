/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include <random>

#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

#ifdef USE_MICRO_STANDALONE_RUNTIME

#include <tvm/runtime/micro/standalone/utvm_runtime.h>
#include <tvm/runtime/packed_func.h>

#include <spawn.h>
#include <sys/wait.h>

using namespace tvm;

int main(int argc, char** argv) {
  assert(argc == 3);

  std::ifstream ifs(argv[2]);
  std::stringstream ss;
  ss << ifs.rdbuf();
  std::string json = ss.str();

  // initialize a runtime
  std::string so_fname = argv[1];
  auto* dsoModule = UTVMRuntimeDSOModuleCreate(so_fname.c_str(), so_fname.size());
  assert(dsoModule != nullptr);
  auto* handle = UTVMRuntimeCreate(json.c_str(), json.size(), dsoModule);
  assert(handle != nullptr);

  // initialize the tensor
  auto A = tvm::runtime::NDArray::Empty({2, 3}, {kDLFloat, 32, 1}, {kDLCPU, 0});
  auto B = tvm::runtime::NDArray::Empty({2, 3}, {kDLFloat, 32, 1}, {kDLCPU, 0});
  auto C = tvm::runtime::NDArray::Empty({2, 3}, {kDLFloat, 32, 1}, {kDLCPU, 0});

  auto pA = (float*)A.ToDLPack()->dl_tensor.data;
  auto pB = (float*)B.ToDLPack()->dl_tensor.data;
  auto pC = (float*)C.ToDLPack()->dl_tensor.data;

  for (int i = 0; i < 6; ++i) {
    pA[i] = i;
    pB[i] = i + 1;
    pC[i] = i + 2;
  }

  // set the input and run the runtime
  UTVMRuntimeSetInput(handle, 0, &A.ToDLPack()->dl_tensor);
  UTVMRuntimeSetInput(handle, 1, &B.ToDLPack()->dl_tensor);
  UTVMRuntimeSetInput(handle, 2, &C.ToDLPack()->dl_tensor);
  UTVMRuntimeRun(handle);
  auto Y = tvm::runtime::NDArray::Empty({2, 3}, {kDLFloat, 32, 1}, {kDLCPU, 0});
  UTVMRuntimeGetOutput(handle, 0, &Y.ToDLPack()->dl_tensor);
  auto* pY = (float*)Y.ToDLPack()->dl_tensor.data;
  for (int i = 0; i < 6; ++i) {
    CHECK_LT(fabs(pY[i] - (i + (i + 1) + (i + 2))), 1e-4);
  }
  UTVMRuntimeDestroy(handle);
  UTVMRuntimeDSOModuleDestroy(dsoModule);
}

#endif
