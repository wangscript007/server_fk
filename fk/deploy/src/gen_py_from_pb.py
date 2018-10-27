# -*- coding: utf-8 -*-
import os
import platform
from loghelper import LogError


# 将PB转换成py格式
def gen_py_from_pb(proto_path):
    files = os.listdir(proto_path)
    for file in files:
        if not file.endswith('.proto'):
            continue
        pre_file = file[:len(file) - len('.proto')]
        pre_file += '_pb2.py'
        pre_file = '../proto/' + pre_file
        if os.path.exists(pre_file):
            pass
        if platform.system() == "Windows":
            command = "..\\..\\bin\\protoc.exe --proto_path=../proto --python_out=../proto/ " + file
        else:
            command = "protoc --proto_path=../proto --python_out=../proto " + file
        try:
            os.system(command)
        except BaseException:
            LogError('protoc %s failed' % file)
            raise


