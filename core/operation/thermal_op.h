// -----------------------------------------------------------------------------
//
// Copyright (C) The BioDynaMo Project.
// All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
//
// See the LICENSE file distributed with this work for details.
// See the NOTICE file distributed with this work for additional information
// regarding copyright ownership.
//
// -----------------------------------------------------------------------------

#ifndef CORE_OPERATION_THERMAL_OP_H_
#define CORE_OPERATION_THERMAL_OP_H_

#include <string>
#include <utility>
#include <vector>

#include "core/container/inline_vector.h"
#include "core/thermal.h"
#include "core/grid.h"
#include "core/param/param.h"
#include "core/resource_manager.h"
#include "core/simulation.h"

namespace bdm {

/// A class that sets up diffusion grids of the substances in this simulation
class ThermalOp {
 public:
  ThermalOp() {}
  virtual ~ThermalOp() {}

  void operator()() {
    auto* sim = Simulation::GetActive();
    auto* rm = sim->GetResourceManager();
    auto* grid = sim->GetGrid();
    auto* param = sim->GetParam();

    rm->ApplyOnAllThermalGrids([&](ThermalGrid* tg) {
      // Update the diffusion grid dimension if the neighbor grid dimensions
      // have changed. If the space is bound, we do not need to update the
      // dimensions, because these should not be changing anyway
      if (grid->HasGrown() && !param->bound_space_) {
        Log::Info("ThermalOp",
                  "Your simulation objects are getting near the edge of the "
                  "simulation space. Be aware of boundary conditions that may "
                  "come into play!");
        tg->Update(grid->GetDimensionThresholds());
      }

      if (param->leaking_edges_) {
        tg->DiffuseEulerLeakingEdge();
      } else {
        tg->DiffuseEuler();
      }

      if (param->calculate_gradients_) {
        tg->CalculateGradient();
      }
    });
  }
};

}  // namespace bdm

#endif  // CORE_OPERATION_DIFFUSION_OP_H_
