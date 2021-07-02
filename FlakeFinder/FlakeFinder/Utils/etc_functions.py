"""
A collection of various functions
"""
import re
import functools
import time


def sorted_alphanumeric(data):
    """
    sorts an array of strings alphanumericly
    """

    def convert(text):
        return int(text) if text.isdigit() else text.lower()

    def alphanum_key(key):
        return [convert(c) for c in re.split("([0-9]+)", key)]

    return sorted(data, key=alphanum_key)


def timer(func):
    """
    A simple timer decorator to time my functions
    """

    @functools.wraps(func)
    def wrapper(*args, **kwargs):
        start = time.time()
        return_values = func(*args, **kwargs)
        print(time.time() - start)
        return return_values

    return wrapper
