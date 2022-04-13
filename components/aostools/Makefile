#
# Setup local pypi server:
#   - Refer to https://github.com/codekoala/docker-pypi
# Local Publish:
#   - Start local pypi server with "make run"
#   - Run "python setup.py sdist upload -r local"
#

# Install aostools from internal pypi server
#   $ pip install -i http://11.238.148.46:9002/ --trusted-host 11.238.148.46 -U aostools

all: help

build-aos:
	@python setup.py sdist

buildall: build-aos
	@:

release:
	@echo "Run blew commond to upload aostools to https://upload.pypi.org/legacy:"
	@echo "  $$ python setup.py sdist"
	@echo "  $$ twine upload dist/aos-tools-*.tar.gz"

# Install and start pypi server refer to: https://github.com/codekoala/docker-pypi
upload:
	@python setup.py sdist
	@twine upload -r internal dist/aos-tools-*.tar.gz

check:
	@flake8
help:
	@echo ""
	@echo "Release aostools:"
	@echo "  $$ make upload    build and upload aostools to internal pypi server"
	@echo "  $$ make release   show build and release steps"
	@echo "  $$ make clean     clean build dir and files"
	@echo ""
	@echo "Install aostools from internal server:"
	@echo "  $$ pip install -i http://11.238.148.46:9002/ --trusted-host 11.238.148.46 -U aos-tools"
	@echo ""
	@echo ""

clean:
	@rm -rf build dist aostools*.egg-info
	@find . -name *.pyc -exec rm -f {} \;
