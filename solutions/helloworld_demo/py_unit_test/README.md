

# Python 自动化测试对接方法

Python应用采用的测试框架是unittest, unittest中最核心的四三个概念是：test case, test suite, test runner

* 一个TestCase的实例就是一个测试用例。什么是测试用例呢？就是一个完整的测试流程，包括测试前准备环境的搭建(setUp)，执行测试代码(run)，以及测试后环境的还原(tearDown)。元测试(unit test)的本质也就在这里，一个测试用例是一个完整的测试单元，通过运行这个测试单元，可以对某一个问题进行验证。

* 而多个测试用例集合在一起，就是TestSuite，而且TestSuite也可以嵌套TestSuite。

* TestLoader是用来加载TestCase到TestSuite中的，其中有几个loadTestsFrom__()方法，就是从各个地方寻找TestCase，创建它们的实例，然后add到TestSuite中，再返回一个TestSuite实例。TextTestRunner是来执行测试用例的，其中的run(test)会执行TestSuite/TestCase中的run(result)方法。
测试的结果会保存到TextTestResult实例中，包括运行了多少测试用例，成功了多少，失败了多少等信息。


## 如何编辑一个测试模块
* 参考 device 目录的 mod_iot.py,要点如下：
  * 类名继承 unittest.TestCase
  * 文件入口，调用 unittest.main()
  * 模块的所有测试用例必须以 test_ 开头
  * 测试用例的结果必须是 self.assertXXXXXX ,具体支持哪些函数，可以参考 unittest.py
  * 类中可以按需实现setUp 方法， setUp 方法会在每个测试用例执行之前调用
  * 类中可以按需实现tearDown 方法， tearDown 方法会在每个测试用例执行之后调用
  * 如果测试用例依赖外部条件才能完成，需要在测试用例中添加 条件打印（print(getAutoTestConditionTag(CON_SET_DEV_PROPS))） ，并且在pc 端触发条件

* 测试模块名必须以 mod_ 开头，并且位于test_cases/py/common 或者 test_cases/py/开发板名称 目录


## 如何测试

```
execfile('/data/pyamp/mod_test.py')
```
