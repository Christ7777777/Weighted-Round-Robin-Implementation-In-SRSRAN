/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "prach_detector_test_data.h"
#include "srsran/phy/lower/modulation/modulation_factories.h"
#include "srsran/phy/support/support_factories.h"
#include "srsran/phy/upper/channel_processors/channel_processor_factories.h"
#include "srsran/phy/upper/channel_processors/channel_processor_formatters.h"
#include "srsran/ran/prach/prach_preamble_information.h"
#include "srsran/srsvec/copy.h"
#include "fmt/ostream.h"
#include <gtest/gtest.h>

static bool is_recognized_conf(const srsran::prach_detector::configuration& conf)
{
  if (conf.nof_rx_ports != 2) {
    return false;
  }
  return ((conf.format == prach_format_type::zero) || (conf.format == prach_format_type::A1));
}

namespace srsran {

std::ostream& operator<<(std::ostream& os, test_case_t test_case)
{
  fmt::print(os, "{} {}", test_case.context.config, test_case.context.result);
  return os;
}

std::ostream& operator<<(std::ostream& os, phy_time_unit value)
{
  fmt::print(os, "{}usec", value.to_seconds() * 1e6);
  return os;
}

} // namespace srsran

using namespace srsran;

using PrachDetectorParams = test_case_t;

class PrachDetectorFixture : public ::testing::TestWithParam<PrachDetectorParams>
{
protected:
  std::unique_ptr<prach_detector>           detector;
  std::unique_ptr<prach_detector_validator> validator;

  void SetUp() override
  {
    std::shared_ptr<dft_processor_factory> dft_factory = create_dft_processor_factory_fftw_slow();
    if (!dft_factory) {
      dft_factory = create_dft_processor_factory_generic();
    }
    ASSERT_TRUE(dft_factory);

    std::shared_ptr<prach_generator_factory> generator_factory = create_prach_generator_factory_sw();
    ASSERT_TRUE(generator_factory);

    std::shared_ptr<prach_detector_factory> detector_factory =
        create_prach_detector_factory_sw(dft_factory, generator_factory);
    ASSERT_TRUE(detector_factory);

#if 0
    srslog::init();
    detector = detector_factory->create(srslog::fetch_basic_logger("PRACH"), true);
#else
    detector = detector_factory->create();
#endif
    ASSERT_TRUE(detector);

    validator = detector_factory->create_validator();
    ASSERT_TRUE(validator);
  }
};

TEST_P(PrachDetectorFixture, FromVector)
{
  const PrachDetectorParams&           params          = GetParam();
  const prach_detector::configuration& config          = params.context.config;
  const prach_detection_result&        expected_result = params.context.result;
  auto                                 sequence_data   = params.symbols.read();

  // Make sure configuration is valid.
  ASSERT_TRUE(validator->is_valid(config));

  // Get preamble information.
  prach_preamble_information preamble_info;
  if (is_long_preamble(config.format)) {
    preamble_info = get_prach_preamble_long_info(config.format);
  } else {
    preamble_info = get_prach_preamble_short_info(config.format, config.ra_scs, false);
  }

  // Calculate the number of symbols.
  unsigned nof_symbols =
      static_cast<unsigned>(preamble_info.symbol_length.to_seconds() * ra_scs_to_Hz(preamble_info.scs));

  // Get frequency domain data.
  prach_buffer_tensor sequence(sequence_data);
  ASSERT_EQ(sequence.get_sequence_length(), preamble_info.sequence_length);
  ASSERT_EQ(sequence.get_max_nof_symbols(), nof_symbols);
  ASSERT_EQ(sequence.get_max_nof_td_occasions(), 1);
  ASSERT_EQ(sequence.get_max_nof_fd_occasions(), 1);
  ASSERT_EQ(sequence.get_max_nof_ports(), config.nof_rx_ports);

  // Run generator.
  prach_detection_result result = detector->detect(sequence, config);

  // Calculate expected delay.
  phy_time_unit time_error_tolerance = phy_time_unit::from_seconds(1.04e-6F);
  if (config.ra_scs == prach_subcarrier_spacing::kHz15) {
    time_error_tolerance = phy_time_unit::from_seconds(0.52e-6F);
  } else if (config.ra_scs == prach_subcarrier_spacing::kHz30) {
    time_error_tolerance = phy_time_unit::from_seconds(0.26e-6F);
  }

  // TODO(david): work out a better set of test vectors.
  if (is_recognized_conf(config)) {
    // Assert a one preamble is found.
    ASSERT_EQ(1, result.preambles.size());

    // Verify the preamble index.
    const prach_detection_result::preamble_indication& preamble_indication = result.preambles.back();
    ASSERT_EQ(expected_result.preambles.front().preamble_index, preamble_indication.preamble_index);
    ASSERT_LE(preamble_indication.time_advance, time_error_tolerance);
  }
}

// Creates test suite that combines all possible parameters. Denote zero_correlation_zone exceeds the maximum by one.
INSTANTIATE_TEST_SUITE_P(PrachDetectorSimple, PrachDetectorFixture, ::testing::ValuesIn(prach_detector_test_data));
