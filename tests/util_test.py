from __future__ import print_function
import unittest
import gridpp
import numpy as np


class Test(unittest.TestCase):
    def test_get_statistic(self):
        self.assertEqual(gridpp.get_statistic("mean"), gridpp.Mean)
        self.assertEqual(gridpp.get_statistic("min"), gridpp.Min)
        self.assertEqual(gridpp.get_statistic("max"), gridpp.Max)
        self.assertEqual(gridpp.get_statistic("median"), gridpp.Median)
        self.assertEqual(gridpp.get_statistic("quantile"), gridpp.Quantile)
        self.assertEqual(gridpp.get_statistic("std"), gridpp.Std)
        self.assertEqual(gridpp.get_statistic("sum"), gridpp.Sum)

    def test_unknown_statistic(self):
        self.assertEqual(gridpp.get_statistic("mean1"), gridpp.Unknown)

    """ Check that it doesn't cause any errors """
    def test_version(self):
        gridpp.version()

    def test_clock(self):
        time = gridpp.clock()
        self.assertTrue(time > 0)

    def test_is_valid(self):
        self.assertTrue(gridpp.is_valid(1))
        self.assertTrue(gridpp.is_valid(-1))
        self.assertTrue(gridpp.is_valid(-999))  # Check that the old missing value indicator is valid now
        self.assertFalse(gridpp.is_valid(np.inf))
        self.assertFalse(gridpp.is_valid(np.nan))

    def test_calc_statistic(self):
        self.assertEqual(gridpp.calc_statistic([0, 1, 2], gridpp.Mean), 1)
        self.assertEqual(gridpp.calc_statistic([0, 1, np.nan], gridpp.Mean), 0.5)
        self.assertEqual(gridpp.calc_statistic([np.nan, 1, np.nan], gridpp.Mean), 1)
        self.assertTrue(np.isnan(gridpp.calc_statistic([np.nan, np.nan, np.nan], gridpp.Mean)))
        self.assertTrue(np.isnan(gridpp.calc_statistic([], gridpp.Mean)))

    def test_calc_quantile(self):
        self.assertEqual(gridpp.calc_quantile([0, 1, 2], 0), 0)
        self.assertEqual(gridpp.calc_quantile([0, 1, 2], 0.5), 1)
        self.assertEqual(gridpp.calc_quantile([0, 1, 2], 1), 2)
        self.assertEqual(gridpp.calc_quantile([0, np.nan, 2], 1), 2)
        self.assertEqual(gridpp.calc_quantile([0, np.nan, 2], 0), 0)
        self.assertEqual(gridpp.calc_quantile([0, np.nan, 2], 0.5), 1)
        self.assertTrue(np.isnan(gridpp.calc_quantile([np.nan, np.nan, np.nan], 0.5)))
        self.assertTrue(np.isnan(gridpp.calc_quantile([np.nan], 0.5)))
        # BUG: This should work:
        # self.assertTrue(np.isnan(gridpp.calc_quantile([], 0.5)))

        self.assertEqual(gridpp.calc_quantile([[0, 1, 2]], 0), [0])
        self.assertEqual(gridpp.calc_quantile([[0, 1, 2]], 0.5), [1])
        self.assertEqual(gridpp.calc_quantile([[0, 1, 2]], 1), [2])
        self.assertEqual(gridpp.calc_quantile([[0, np.nan, 2]], 1), [2])
        self.assertEqual(gridpp.calc_quantile([[0, np.nan, 2]], 0), [0])
        self.assertEqual(gridpp.calc_quantile([[0, np.nan, 2]], 0.5), [1])
        quantile_of_nan_list = gridpp.calc_quantile([[np.nan, np.nan, np.nan]], 0.5)
        self.assertEqual(len(quantile_of_nan_list), 1)
        self.assertTrue(np.isnan(quantile_of_nan_list[0]))

    def test_num_missing_values(self):
        self.assertEqual(gridpp.num_missing_values([[0, np.nan, 1, np.inf]]), 2)
        self.assertEqual(gridpp.num_missing_values([[np.nan, np.inf]]), 2)
        self.assertEqual(gridpp.num_missing_values([[0, 0, 1, 1]]), 0)
        self.assertEqual(gridpp.num_missing_values([[0, np.nan], [1, np.inf]]), 2)
        self.assertEqual(gridpp.num_missing_values([[np.nan, np.nan], [np.nan, np.inf]]), 4)
        self.assertEqual(gridpp.num_missing_values([[0, 0], [1, 1]]), 0)
        self.assertEqual(gridpp.num_missing_values([[]]), 0)


if __name__ == '__main__':
    unittest.main()
