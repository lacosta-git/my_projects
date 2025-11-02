# Decrypt password-protected PDF in Python.
# 
# Requirements:
# pip install PyPDF2

from PyPDF2 import PdfFileReader

def decrypt_pdf(input_path):
  with open(input_path, 'rb') as input_file:
    reader = PdfFileReader(input_file)
    n = 0
    nn = 0
    while(n<10000):
        p = str(n).zfill(4)
        if (n > nn + 100):
            nn = n
            print(n)
        if (reader.decrypt(p) > 0):
            print("Password = " + p)
            print(reader.decrypt(p))
            break
        else:
            n = n + 1

if __name__ == '__main__':
    decrypt_pdf('p.PDF')
    