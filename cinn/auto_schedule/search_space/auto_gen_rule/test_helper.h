// Copyright (c) 2022 CINN Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "cinn/backends/compiler.h"
#include "cinn/common/target.h"
#include "cinn/frontend/syntax.h"
#include "cinn/hlir/framework/scope.h"
#include "cinn/ir/ir_schedule.h"
#include "cinn/ir/lowered_func.h"

namespace cinn {
namespace auto_schedule {

/* @brief: Function pointer of executable code compiled by CINN.
 * @params-1: Pointers to all arguments, including input and output.
 * @params-2: The number of Arguments.
 * @return: void
 */
using test_func_type = void (*)(void**, int32_t);

// A base utility class for testing AutoGenRule
class TestAutoGenRuleBase : public ::testing::Test {
 public:
  void SetUp() override {
    srand(0);
    Context::Global().ResetNameId();
  }

  // Initialize a ir::IRSchedule by lowering the specified for following AutoGenRule test
  ir::IRSchedule InitSchedule(const frontend::Program& test_program, const common::Target& target);

  // build ir::Module from the original lowered funcs with their bodys updated by the schedule
  ir::Module BuildIRModule(const ir::IRSchedule& schedule);

  // generate source code with the built ir module
  std::string GenSourceCode(const ir::Module& ir_module);

  // generate executable kernel function with the built ir module
  test_func_type GenExecutableKernel(const ir::Module& ir_module);

 protected:
  common::Target target_;
  std::vector<ir::LoweredFunc> lowered_funcs_;
  std::unique_ptr<backends::Compiler> backend_compier_;
};

/* @brief: Packaging the naive matmul as a unified format required by the CheckResult interface.
 * @params-1: Pointers to the memory of input data, each input corresponds to a pointer.
 * @params-2: Pointers to the memory of output data, each output corresponds to a pointer.
 * @params-3: Shapes of the input data, each input corresponds to a std::vector<int>.
 * @params-4: Shapes of the output data, each output corresponds to a std::vector<int>.
 * @return: void
 */
void expected_func_matmul(const std::vector<float*>& inputs,
                          const std::vector<float*>& outputs,
                          const std::vector<std::vector<int>>& input_shapes,
                          const std::vector<std::vector<int>>& output_shapes);

/* @brief: Unified signature format as the expected function for comparison.
 * @params-1: Pointers to the memory of input data, each input corresponds to a pointer.
 * @params-2: Pointers to the memory of output data, each output corresponds to a pointer.
 * @params-3: Shapes of the input data, each input corresponds to a std::vector<int>.
 * @params-4: Shapes of the output data, each output corresponds to a std::vector<int>.
 * @return: void
 */
using expected_func_type = void (*)(const std::vector<float*>&,
                                    const std::vector<float*>&,
                                    const std::vector<std::vector<int>>&,
                                    const std::vector<std::vector<int>>&);

/* @brief: Interface for checking function correctness.
 * @params-1: Function pointer of the function to be tested.
 * @params-2: Expected function pointer for comparison.
 * @params-3: Names of input data.
 * @params-4: Names of output data.
 * @params-5: Shapes of the input data, each input corresponds to a std::vector<int>.
 * @params-6: Shapes of the output data, each output corresponds to a std::vector<int>.
 * @params-7: The Target expressing computing platform and architecture of the function to be tested.
 * @return: void
 */
void CheckResult(test_func_type test_func,
                 expected_func_type expected_func,
                 const std::vector<std::string>& input_names,
                 const std::vector<std::string>& output_names,
                 const std::vector<std::vector<int>>& input_shapes,
                 const std::vector<std::vector<int>>& output_shapes,
                 const common::Target& target);

}  // namespace auto_schedule
}  // namespace cinn
