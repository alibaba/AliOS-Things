import unittest


class TestUnittestAssertions(unittest.TestCase):
    def testFail(self):
        with self.assertRaises(AssertionError):
            self.fail("failure")

    def testEqual(self):
        self.assertEqual(0, 0)
        self.assertEqual([0, 1, 2], [0, 1, 2])
        with self.assertRaises(AssertionError):
            self.assertEqual(0, None)
        with self.assertRaises(AssertionError):
            self.assertEqual([0, 1, 2], [1, 2, 3])

    def test_AlmostEqual(self):
        self.assertAlmostEqual(1.00000001, 1.0)
        self.assertNotAlmostEqual(1.0000001, 1.0)
        with self.assertRaises(AssertionError):
            self.assertAlmostEqual(1.0000001, 1.0)
        with self.assertRaises(AssertionError):
            self.assertNotAlmostEqual(1.00000001, 1.0)

        self.assertAlmostEqual(1.1, 1.0, places=0)
        with self.assertRaises(AssertionError):
            self.assertAlmostEqual(1.1, 1.0, places=1)

        self.assertAlmostEqual(0, 0.1 + 0.1j, places=0)
        self.assertNotAlmostEqual(0, 0.1 + 0.1j, places=1)
        with self.assertRaises(AssertionError):
            self.assertAlmostEqual(0, 0.1 + 0.1j, places=1)
        with self.assertRaises(AssertionError):
            self.assertNotAlmostEqual(0, 0.1 + 0.1j, places=0)

        self.assertAlmostEqual(float("inf"), float("inf"))
        with self.assertRaises(AssertionError):
            self.assertNotAlmostEqual(float("inf"), float("inf"))

    def test_AmostEqualWithDelta(self):
        self.assertAlmostEqual(1.1, 1.0, delta=0.5)
        self.assertAlmostEqual(1.0, 1.1, delta=0.5)
        self.assertNotAlmostEqual(1.1, 1.0, delta=0.05)
        self.assertNotAlmostEqual(1.0, 1.1, delta=0.05)

        self.assertAlmostEqual(1.0, 1.0, delta=0.5)
        with self.assertRaises(AssertionError):
            self.assertNotAlmostEqual(1.0, 1.0, delta=0.5)
        with self.assertRaises(AssertionError):
            self.assertAlmostEqual(1.1, 1.0, delta=0.05)
        with self.assertRaises(AssertionError):
            self.assertNotAlmostEqual(1.1, 1.0, delta=0.5)
        with self.assertRaises(TypeError):
            self.assertAlmostEqual(1.1, 1.0, places=2, delta=2)
        with self.assertRaises(TypeError):
            self.assertNotAlmostEqual(1.1, 1.0, places=2, delta=2)

    def testNotEqual(self):
        self.assertNotEqual([0, 1, 2], [0, 2, 1])
        with self.assertRaises(AssertionError):
            self.assertNotEqual(0, 0)
        with self.assertRaises(AssertionError):
            self.assertNotEqual([0, 1, 2], [0, 1, 2])

    def testIs(self):
        self.assertIs(None, None)
        with self.assertRaises(AssertionError):
            self.assertIs([1, 2, 3], [1, 2, 3])

    def testIsNot(self):
        self.assertIsNot([1, 2, 3], [1, 2, 3])
        with self.assertRaises(AssertionError):
            self.assertIsNot(None, None)

    def testIsNone(self):
        self.assertIsNone(None)
        with self.assertRaises(AssertionError):
            self.assertIsNone(0)

    def testIsNotNone(self):
        self.assertIsNotNone(0)
        with self.assertRaises(AssertionError):
            self.assertIsNotNone(None)

    def testTrue(self):
        self.assertTrue(True)
        with self.assertRaises(AssertionError):
            self.assertTrue(False)

    def testFalse(self):
        self.assertFalse(False)
        with self.assertRaises(AssertionError):
            self.assertFalse(True)

    def testIn(self):
        self.assertIn("t", "cat")
        with self.assertRaises(AssertionError):
            self.assertIn("x", "cat")

    def testIsInstance(self):
        self.assertIsInstance("cat", str)
        with self.assertRaises(AssertionError):
            self.assertIsInstance(7, str)

    def testRaises(self):
        with self.assertRaises(ZeroDivisionError):
            1 / 0
            pass

    @unittest.skip("test of skipping")
    def testSkip(self):
        self.assertFail("this should be skipped")


if __name__ == "__main__":
    unittest.main()
