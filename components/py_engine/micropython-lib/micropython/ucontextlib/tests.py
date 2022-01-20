import unittest
from ucontextlib import contextmanager


class ContextManagerTestCase(unittest.TestCase):
    def setUp(self):
        self._history = []

        @contextmanager
        def manager(x):
            self._history.append("start")
            try:
                yield x
            finally:
                self._history.append("finish")

        self._manager = manager

    def test_context_manager(self):
        with self._manager(123) as x:
            self.assertEqual(x, 123)
        self.assertEqual(self._history, ["start", "finish"])

    def test_context_manager_on_error(self):
        exc = Exception()
        try:
            with self._manager(123) as x:
                raise exc
        except Exception as e:
            self.assertEqual(exc, e)
        self.assertEqual(self._history, ["start", "finish"])


if __name__ == "__main__":
    unittest.main()
