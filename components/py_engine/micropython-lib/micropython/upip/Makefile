all:

# This target prepares snapshot of all dependency modules, for
# self-contained install
deps: upip_gzip.py upip_utarfile.py

upip_gzip.py: ../gzip/gzip.py
	cp $^ $@
upip_utarfile.py: ../utarfile/utarfile.py
	cp $^ $@

clean:
	rm upip_*.py
