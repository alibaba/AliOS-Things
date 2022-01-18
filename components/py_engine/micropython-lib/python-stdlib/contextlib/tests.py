import sys
import unittest
from contextlib import closing, suppress, ExitStack


class ClosingTestCase(unittest.TestCase):
    class Closable:
        def __init__(self):
            self.closed = False

        def close(self):
            self.closed = True

    def test_closing(self):
        closable = self.Closable()
        with closing(closable) as c:
            self.assertFalse(c.closed)
        self.assertTrue(closable.closed)

    def test_closing_after_error(self):
        closable = self.Closable()
        exc = Exception()
        try:
            with closing(closable) as c:
                raise exc
        except Exception as e:
            self.assertEqual(exc, e)
        self.assertTrue(closable.closed)


class SuppressTestCase(unittest.TestCase):
    def test_suppress(self):
        with suppress(ValueError, TypeError):
            raise ValueError()
            raise TypeError()
        self.assertTrue(True)


class TestExitStack(unittest.TestCase):

    # @support.requires_docstrings
    def _test_instance_docs(self):
        # Issue 19330: ensure context manager instances have good docstrings
        cm_docstring = ExitStack.__doc__
        obj = ExitStack()
        self.assertEqual(obj.__doc__, cm_docstring)

    def test_no_resources(self):
        with ExitStack():
            pass

    def test_callback(self):
        expected = [
            ((), {}),
            ((1,), {}),
            ((1, 2), {}),
            ((), dict(example=1)),
            ((1,), dict(example=1)),
            ((1, 2), dict(example=1)),
        ]
        result = []

        def _exit(*args, **kwds):
            """Test metadata propagation"""
            result.append((args, kwds))

        with ExitStack() as stack:
            for args, kwds in reversed(expected):
                if args and kwds:
                    f = stack.callback(_exit, *args, **kwds)
                elif args:
                    f = stack.callback(_exit, *args)
                elif kwds:
                    f = stack.callback(_exit, **kwds)
                else:
                    f = stack.callback(_exit)
                self.assertIs(f, _exit)
            # for wrapper in stack._exit_callbacks:
            #     self.assertIs(wrapper.__wrapped__, _exit)
            #     self.assertNotEqual(wrapper.__name__, _exit.__name__)
            #     self.assertIsNone(wrapper.__doc__, _exit.__doc__)
        self.assertEqual(result, expected)

    def test_push(self):
        exc_raised = ZeroDivisionError

        def _expect_exc(exc_type, exc, exc_tb):
            self.assertIs(exc_type, exc_raised)

        def _suppress_exc(*exc_details):
            return True

        def _expect_ok(exc_type, exc, exc_tb):
            self.assertIsNone(exc_type)
            self.assertIsNone(exc)
            self.assertIsNone(exc_tb)

        class ExitCM(object):
            def __init__(self, check_exc):
                self.check_exc = check_exc

            def __enter__(self):
                self.fail("Should not be called!")

            def __exit__(self, *exc_details):
                self.check_exc(*exc_details)

        with ExitStack() as stack:
            stack.push(_expect_ok)
            self.assertIs(tuple(stack._exit_callbacks)[-1], _expect_ok)
            cm = ExitCM(_expect_ok)
            stack.push(cm)
            # self.assertIs(stack._exit_callbacks[-1].__self__, cm)
            stack.push(_suppress_exc)
            self.assertIs(tuple(stack._exit_callbacks)[-1], _suppress_exc)
            cm = ExitCM(_expect_exc)
            stack.push(cm)
            # self.assertIs(stack._exit_callbacks[-1].__self__, cm)
            stack.push(_expect_exc)
            self.assertIs(tuple(stack._exit_callbacks)[-1], _expect_exc)
            stack.push(_expect_exc)
            self.assertIs(tuple(stack._exit_callbacks)[-1], _expect_exc)
            1 / 0

    def test_enter_context(self):
        class TestCM(object):
            def __enter__(self):
                result.append(1)

            def __exit__(self, *exc_details):
                result.append(3)

        result = []
        cm = TestCM()
        with ExitStack() as stack:

            @stack.callback  # Registered first => cleaned up last
            def _exit():
                result.append(4)

            self.assertIsNotNone(_exit)
            stack.enter_context(cm)
            # self.assertIs(stack._exit_callbacks[-1].__self__, cm)
            result.append(2)
        self.assertEqual(result, [1, 2, 3, 4])

    def test_close(self):
        result = []
        with ExitStack() as stack:

            @stack.callback
            def _exit():
                result.append(1)

            self.assertIsNotNone(_exit)
            stack.close()
            result.append(2)
        self.assertEqual(result, [1, 2])

    def test_pop_all(self):
        result = []
        with ExitStack() as stack:

            @stack.callback
            def _exit():
                result.append(3)

            self.assertIsNotNone(_exit)
            new_stack = stack.pop_all()
            result.append(1)
        result.append(2)
        new_stack.close()
        self.assertEqual(result, [1, 2, 3])

    def test_exit_raise(self):
        with self.assertRaises(ZeroDivisionError):
            with ExitStack() as stack:
                stack.push(lambda *exc: False)
                1 / 0

    def test_exit_suppress(self):
        with ExitStack() as stack:
            stack.push(lambda *exc: True)
            1 / 0

    def test_exit_exception_chaining_reference(self):
        # Sanity check to make sure that ExitStack chaining matches
        # actual nested with statements
        exc_chain = []

        class RaiseExc:
            def __init__(self, exc):
                self.exc = exc

            def __enter__(self):
                return self

            def __exit__(self, *exc_details):
                exc_chain.append(exc_details[0])
                raise self.exc

        class RaiseExcWithContext:
            def __init__(self, outer, inner):
                self.outer = outer
                self.inner = inner

            def __enter__(self):
                return self

            def __exit__(self, *exc_details):
                try:
                    exc_chain.append(exc_details[0])
                    raise self.inner
                except:
                    exc_chain.append(sys.exc_info()[0])
                    raise self.outer

        class SuppressExc:
            def __enter__(self):
                return self

            def __exit__(self, *exc_details):
                type(self).saved_details = exc_details
                return True

        try:
            with RaiseExc(IndexError):
                with RaiseExcWithContext(KeyError, AttributeError):
                    with SuppressExc():
                        with RaiseExc(ValueError):
                            1 / 0
        except IndexError as exc:
            # self.assertIsInstance(exc.__context__, KeyError)
            # self.assertIsInstance(exc.__context__.__context__, AttributeError)
            # Inner exceptions were suppressed
            # self.assertIsNone(exc.__context__.__context__.__context__)
            exc_chain.append(type(exc))
            assert tuple(exc_chain) == (
                ZeroDivisionError,
                None,
                AttributeError,
                KeyError,
                IndexError,
            )
        else:
            self.fail("Expected IndexError, but no exception was raised")
        # Check the inner exceptions
        inner_exc = SuppressExc.saved_details[1]
        self.assertIsInstance(inner_exc, ValueError)
        # self.assertIsInstance(inner_exc.__context__, ZeroDivisionError)

    def test_exit_exception_chaining(self):
        # Ensure exception chaining matches the reference behaviour
        exc_chain = []

        def raise_exc(exc):
            frame_exc = sys.exc_info()[0]
            if frame_exc is not None:
                exc_chain.append(frame_exc)
            exc_chain.append(exc)
            raise exc

        saved_details = None

        def suppress_exc(*exc_details):
            nonlocal saved_details
            saved_details = exc_details
            assert exc_chain[-1] == exc_details[0]
            exc_chain[-1] = None
            return True

        try:
            with ExitStack() as stack:
                stack.callback(raise_exc, IndexError)
                stack.callback(raise_exc, KeyError)
                stack.callback(raise_exc, AttributeError)
                stack.push(suppress_exc)
                stack.callback(raise_exc, ValueError)
                1 / 0
        except IndexError as exc:
            # self.assertIsInstance(exc.__context__, KeyError)
            # self.assertIsInstance(exc.__context__.__context__, AttributeError)
            # Inner exceptions were suppressed
            # self.assertIsNone(exc.__context__.__context__.__context__)
            assert tuple(exc_chain) == (
                ZeroDivisionError,
                None,
                AttributeError,
                KeyError,
                IndexError,
            )
        else:
            self.fail("Expected IndexError, but no exception was raised")
        # Check the inner exceptions
        inner_exc = saved_details[1]
        self.assertIsInstance(inner_exc, ValueError)
        # self.assertIsInstance(inner_exc.__context__, ZeroDivisionError)

    def test_exit_exception_non_suppressing(self):
        # http://bugs.python.org/issue19092
        def raise_exc(exc):
            raise exc

        def suppress_exc(*exc_details):
            return True

        try:
            with ExitStack() as stack:
                stack.callback(lambda: None)
                stack.callback(raise_exc, IndexError)
        except Exception as exc:
            self.assertIsInstance(exc, IndexError)
        else:
            self.fail("Expected IndexError, but no exception was raised")

        try:
            with ExitStack() as stack:
                stack.callback(raise_exc, KeyError)
                stack.push(suppress_exc)
                stack.callback(raise_exc, IndexError)
        except Exception as exc:
            self.assertIsInstance(exc, KeyError)
        else:
            self.fail("Expected KeyError, but no exception was raised")

    def _test_exit_exception_with_correct_context(self):
        # http://bugs.python.org/issue20317
        @contextmanager
        def gets_the_context_right(exc):
            try:
                yield
            finally:
                raise exc

        exc1 = Exception(1)
        exc2 = Exception(2)
        exc3 = Exception(3)
        exc4 = Exception(4)

        # The contextmanager already fixes the context, so prior to the
        # fix, ExitStack would try to fix it *again* and get into an
        # infinite self-referential loop
        try:
            with ExitStack() as stack:
                stack.enter_context(gets_the_context_right(exc4))
                stack.enter_context(gets_the_context_right(exc3))
                stack.enter_context(gets_the_context_right(exc2))
                raise exc1
        except Exception as exc:
            self.assertIs(exc, exc4)
            self.assertIs(exc.__context__, exc3)
            self.assertIs(exc.__context__.__context__, exc2)
            self.assertIs(exc.__context__.__context__.__context__, exc1)
            self.assertIsNone(exc.__context__.__context__.__context__.__context__)

    def _test_exit_exception_with_existing_context(self):
        # Addresses a lack of test coverage discovered after checking in a
        # fix for issue 20317 that still contained debugging code.
        def raise_nested(inner_exc, outer_exc):
            try:
                raise inner_exc
            finally:
                raise outer_exc

        exc1 = Exception(1)
        exc2 = Exception(2)
        exc3 = Exception(3)
        exc4 = Exception(4)
        exc5 = Exception(5)
        try:
            with ExitStack() as stack:
                stack.callback(raise_nested, exc4, exc5)
                stack.callback(raise_nested, exc2, exc3)
                raise exc1
        except Exception as exc:
            self.assertIs(exc, exc5)
            self.assertIs(exc.__context__, exc4)
            self.assertIs(exc.__context__.__context__, exc3)
            self.assertIs(exc.__context__.__context__.__context__, exc2)
            self.assertIs(exc.__context__.__context__.__context__.__context__, exc1)
            self.assertIsNone(exc.__context__.__context__.__context__.__context__.__context__)

    def test_body_exception_suppress(self):
        def suppress_exc(*exc_details):
            return True

        try:
            with ExitStack() as stack:
                stack.push(suppress_exc)
                1 / 0
        except IndexError as exc:
            self.fail("Expected no exception, got IndexError")

    def test_exit_exception_chaining_suppress(self):
        with ExitStack() as stack:
            stack.push(lambda *exc: True)
            stack.push(lambda *exc: 1 / 0)
            stack.push(lambda *exc: {}[1])

    def test_excessive_nesting(self):
        # The original implementation would die with RecursionError here
        with ExitStack() as stack:
            for i in range(10000):
                stack.callback(int)

    def test_instance_bypass(self):
        class Example(object):
            pass

        cm = Example()
        cm.__exit__ = object()
        stack = ExitStack()
        self.assertRaises(AttributeError, stack.enter_context, cm)
        stack.push(cm)
        self.assertIs(tuple(stack._exit_callbacks)[-1], cm)


if __name__ == "__main__":
    unittest.main()
