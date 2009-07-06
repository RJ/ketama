import ketama
import unittest
import os

class KetamaTest(unittest.TestCase):
    
    def setUp(self):
        self.valid_list_file = os.tmpnam()
        self.valid_list = file(self.valid_list_file, "w")
        self.valid_list.write("127.0.0.1:11211\t600\n")
        self.valid_list.write("127.0.0.1:11212\t400\n")
        self.valid_list.flush()
        self.invalid_list_file = os.tmpnam()
        self.invalid_list = file(self.invalid_list_file, "w")
        self.invalid_list.write("127.0.0.1:11211 600\n")
        self.invalid_list.write("127.0.0.1:11212 two\n")
        self.invalid_list.flush()

    def tearDown(self):
        self.valid_list.close()
        os.unlink(self.valid_list_file)
        self.invalid_list.close()
        os.unlink(self.invalid_list_file)

    def test_valid(self):
        cont = ketama.Continuum(self.valid_list_file)
        self.assertEqual(type(cont), ketama.Continuum)

    def test_invalid_null(self):
        self.assertRaises(ketama.KetamaError, ketama.Continuum, "/dev/null")

    def test_invalid_data(self):
        self.assertRaises(ketama.KetamaError, ketama.Continuum,
            self.invalid_list_file)

    def test_hashing(self):
        cont = ketama.Continuum(self.valid_list_file)
        self.assertEqual(cont.get_server("test"),
            (3454255383L, '127.0.0.1:11211'))

    def test_hashi(self):
        self.assertEqual(ketama.hashi("test"), 3446378249L)

    def test_points(self):
        cont = ketama.Continuum(self.valid_list_file)
        self.assertEqual(len(cont.get_points()), 160 * 2)

if __name__ == "__main__":
    unittest.main()
