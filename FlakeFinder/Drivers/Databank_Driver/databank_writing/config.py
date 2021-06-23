from configparser import ConfigParser
import os


def config(filename="database.ini", section="mysql"):
    # create a parser
    parser = ConfigParser()

    file_path = os.path.join(os.path.dirname(__file__), filename)

    # read config file
    parser.read(file_path)

    # get section, default to mysql
    db = {}
    if parser.has_section(section):
        params = parser.items(section)
        for param in params:
            db[param[0]] = param[1]
    else:
        raise Exception(
            "Section {0} not found in the {1} file".format(section, filename)
        )

    return db
