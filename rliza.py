import ctypes as ct

#https://cylab.be/blog/235/calling-c-from-python

libc = ct.cdll.LoadLibrary("./build/librlib.so")
libc.rliza_validate.argtypes = [ct.c_char_p]

def count(data):
    count = 0
    jsonu8 = data.encode('utf-8')
    while True:
        result = libc.rliza_validate(jsonu8)
        if not result:
            break
        jsonu8 = jsonu8[result:]
        count += 1
    return count 

def length(data):
    count = 0
    jsonu8 = data.encode('utf-8')
    return libc.rliza_validate(jsonu8)
         
import unittest 

class RlizaTestCase(unittest.TestCase):
    def test_count(self):
        self.assertEqual(count("{}[][]{"), 3)
        self.assertEqual(count("{}[][]{}"), 4)
    def test_length(self):
        self.assertEqual(length("{"),0)
        self.assertEqual(length("{}"),2)
        self.assertEqual(length("{}{}"),2)

def test():
    suite = unittest.TestLoader().loadTestsFromTestCase(RlizaTestCase)
    runner = unittest.TextTestRunner()
    runner.run(suite) 

if __name__ == '__main__':
    test()
