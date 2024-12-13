/******************************************************************************
Copyright (c) 2024, 1X Technologies. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
******************************************************************************/

#include <humanoid_common_mpc_ros2/benchmarks/SqpBenchmarksPublisher.h>

namespace ocs2::humanoid {

SqpBenchmarksPublisher::SqpBenchmarksPublisher(rclcpp::Node::SharedPtr node, const ocs2::SqpSolver* sqpSolver) : sqpSolver_(sqpSolver) {
  rclcpp::QoS qos(1);
  qos.best_effort();
  benchmarksPublisher_ = node->create_publisher<ocs2_ros2_msgs::msg::Benchmarks>("/humanoid/mpc_benchmarks", qos);
}

void SqpBenchmarksPublisher::postSolverRun(const ocs2::PrimalSolution& primalSolution) {
  if (primalSolution.timeTrajectory_.size() == 0) return;

  const SqpSolver::Benchmarks benchmarks = sqpSolver_->getBenchmarks();

  ocs2_ros2_msgs::msg::Benchmarks bmMsg;
  bmMsg.time = primalSolution.timeTrajectory_.front();
  auto addBenchmark = [&](const std::string& description, const double& value) {
    ocs2_ros2_msgs::msg::IndividualBenchmarks ibmMsg;
    ibmMsg.description = description;
    ibmMsg.values.emplace_back(value);
    bmMsg.benchmarks.emplace_back(ibmMsg);
  };
  addBenchmark("linearQuadraticApproximationTime", benchmarks.linearQuadraticApproximationTime);
  addBenchmark("solveQpTime", benchmarks.solveQpTime);
  addBenchmark("linesearchTime", benchmarks.linesearchTime);
  addBenchmark("computeControllerTime", benchmarks.computeControllerTime);

  benchmarksPublisher_->publish(bmMsg);
}

}  // namespace ocs2::humanoid