# This script performs bootstrap installation of upip package manager from PyPI
# All the other packages can be installed using it.

if [ -z "$TMPDIR" ]; then
    cd /tmp
else
    cd $TMPDIR
fi

# Remove any stale old version
rm -rf micropython-upip-*
wget -nd -rH -l1 -D files.pythonhosted.org https://pypi.org/project/micropython-upip/ --reject=html

tar xfz micropython-upip-*.tar.gz
mkdir -p ~/.micropython/lib/
cp micropython-upip-*/upip*.py ~/.micropython/lib/

echo "upip is installed. To use:"
echo "micropython -m upip --help"
