import pkg_resources


def get_data(package, resource):
    f = pkg_resources.resource_stream(package, resource)
    try:
        return f.read()
    finally:
        f.close()
