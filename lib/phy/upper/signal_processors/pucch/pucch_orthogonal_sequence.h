/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/ran/pucch/pucch_constants.h"
#include "srsran/support/srsran_assert.h"
#include <array>

namespace srsran {

class pucch_orthogonal_sequence
{
private:
  using w_array =
      std::array<std::array<std::array<cf_t, pucch_constants::FORMAT1_N_MAX>, pucch_constants::FORMAT1_N_MAX>,
                 pucch_constants::FORMAT1_N_MAX>;

  // TS 38.211 Table 6.3.2.4.1-2: Orthogonal sequences for PUCCH format 1
  const unsigned pucch_format1_rho
      [pucch_constants::FORMAT1_N_MAX][pucch_constants::FORMAT1_N_MAX][pucch_constants::FORMAT1_N_MAX] = {
          {{0}, {0, 0}, {0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0}},
          {{}, {0, 1}, {0, 1, 2}, {0, 2, 0, 2}, {0, 1, 2, 3, 4}, {0, 1, 2, 3, 4, 5}, {0, 1, 2, 3, 4, 5, 6}},
          {{}, {}, {0, 2, 1}, {0, 0, 2, 2}, {0, 2, 4, 1, 3}, {0, 2, 4, 0, 2, 4}, {0, 2, 4, 6, 1, 3, 5}},
          {{}, {}, {}, {0, 2, 2, 0}, {0, 3, 1, 4, 2}, {0, 3, 0, 3, 0, 3}, {0, 3, 6, 2, 5, 1, 4}},
          {{}, {}, {}, {}, {0, 4, 3, 2, 1}, {0, 4, 2, 0, 4, 2}, {0, 4, 1, 5, 2, 6, 3}},
          {{}, {}, {}, {}, {}, {0, 5, 4, 3, 2, 1}, {0, 5, 3, 1, 6, 4, 2}},
          {{}, {}, {}, {}, {}, {}, {0, 6, 5, 4, 3, 2, 1}}};

  w_array orthogonal_sequence;

public:
  pucch_orthogonal_sequence()
  {
    for (unsigned n_pucch = 1, max_n_pucch = pucch_constants::FORMAT1_N_MAX + 1; n_pucch != max_n_pucch; ++n_pucch) {
      for (unsigned i = 0; i != pucch_constants::FORMAT1_N_MAX; ++i) {
        for (unsigned m = 0; m != pucch_constants::FORMAT1_N_MAX; ++m) {
          auto rho                               = static_cast<float>(pucch_format1_rho[i][n_pucch - 1][m]);
          orthogonal_sequence[i][n_pucch - 1][m] = std::polar(1.0F, TWOPI * rho / static_cast<float>(n_pucch));
        }
      }
    }
  }

  cf_t get_sequence_value(unsigned n_pucch, unsigned i, unsigned m) const
  {
    srsran_assert(n_pucch >= 1 && n_pucch <= pucch_constants::FORMAT1_N_MAX, "Invalid n_pucch");
    srsran_assert(i < pucch_constants::FORMAT1_N_MAX, "Invalid i");
    srsran_assert(m < pucch_constants::FORMAT1_N_MAX, "Invalid m");
    // Get value
    return orthogonal_sequence[i][n_pucch - 1][m];
  }
};

} // namespace srsran
